#include "QLinReadByID.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_linreadbyid.h"

#include <QtGlobal>
#include <QtGui>
#include <QStyle>

struct QLinReadByIDMenu {
    QLinReadByIDMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinReadByID", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_QLinReadByIDMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinReadByID_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinReadByIDMenu* ctx, Logger* log) {
    log->message("QLinReadByID_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QLinReadByID_register(): application is non gui not registering");
        return false;
    }

    auto linbusMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));

    if (linbusMenu == nullptr) {
        log->message("QLinReadByID_register(): linbus menu not found");
        return false;
    }

    windowAddInstanceSettings(linbusMenu, &ctx->m_settings, &ctx->m_newInstance, "QLinReadByID", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QLinReadByID, QLinReadByIDMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLinReadByID"), gtx, ctx, log);
    });
    
    return true;
}

static bool QLinReadByID_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinReadByIDMenu* ctx, Logger* log) {
    log->message("QLinReadByID_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QLinReadByID,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinReadByID_register,
    QLinReadByID_unregister,
    QLinReadByIDMenu,
    {"QLin"},
    true,
    900,
    LinReadByIDSettings
)

QLinReadByID::QLinReadByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, LinReadByIDSettings* set, const QString& uuid)
    : Widget(ld, plugins, parent, settingsPath, set, uuid)
    , m_data(new Ui::QLinReadByIDUI) {
    m_data.m_ui->setupUi(this);
    //settingsChanged();
    QObject::connect(m_data.m_ui->pushButton, &QPushButton::clicked, this, &QLinReadByID::readById);
    //QTimer::singleShot(0, this, &QLinReadByID::init);
}

bool QLinReadByID::initialize() {
    emit message("QLinReadByID::init()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<LinReadByIDSettings>();
   
    m_data.m_ui->title->setText(set->title());
    
    if (set->linDevice().isEmpty()) {
        emit message("QLinReadByID::initialize: !!! lin device is empty please updated settings !!!");
        return false;
    }

    auto plugin = plugins()->instance(set->linDevice(), 0);

    if (set->previous().isEmpty() == false) {
        emit message("QLinReadByID::settingsChanged: previous not empty");
        auto prev = plugins()->findByObjectName(set->previous());
        if (prev.isNull() == false) {
            emit message("QLinReadByID::settingsChanged: connecting previous");
            connect(prev.dynamicCast<QObject>().data(), SIGNAL(success(const QByteArray&)), this, SLOT(previousSuccess(const QByteArray&)));
        }
        else {
            emit message(QString("QLinReadByID::settingsChanged: failed to find previous: %1").arg(set->previous()));
        }
    }

    if (plugin.isNull() == true) {
        emit message(QString("QLinReadByID::init lin device == nullptr").arg(set->linDevice()));
        return false;
    }

    m_data.m_lin = plugin.dynamicCast<IODevice>();

    connect(m_data.m_lin.data(), &IODevice::dataReady, this, &QLinReadByID::dataReady);
    if (m_data.m_lin->isOpen() == false) {
        if (m_data.m_lin->open() == false) {
            emit message(QString("QLinReadByID::init: failed to open lin device %1").arg(set->linDevice()));
            return false;
        }
    }

    QObject::connect(m_data.m_lin.data(), &IODevice::closed, this, &QLinReadByID::linClosed);
    return true;
}

void QLinReadByID::settingsChanged() {
    emit message("QLinReadByID::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<LinReadByIDSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<LinReadByIDSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

bool QLinReadByID::deinitialize() {

    return true;
}

SettingsMdi* QLinReadByID::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinReadByID::settingsChanged);
    return ret;
}

QLinReadByID::~QLinReadByID() {
}

void QLinReadByID::dataReady(const QByteArray& data) {
    emit message("QLinReadByID::dataReady(const QByteArray& data)");
    emit message(QString("QLinReadByID::dataReady: recv: %1").arg(data));

    if (m_data.m_state != QLinReadByIDState::READ) {
        emit message("QLinReadByID::dataReady: not in read state");
        return;
    }
    auto set = settings<LinReadByIDSettings>();
    auto response = dataFromResponse(data);
    const UDSFrame* frame;

    if (response.has_value() == false) {
        emit message("QLinReadByID::dataReady: no answer response request sent to early looks like device needs more time to process");
        
        if (m_data.m_resched < set->maxReschedules()) {
            ++m_data.m_resched;
            QByteArray data(1, 0);
            data[0] = QString("0x3d").toUInt(nullptr, 16);

            QThread::msleep(set->rescheduleInterval());

            if (m_data.m_lin->write(data) != -1) {
                emit message("QLinReadByID::dataReady: rescheduling read event successful");
                return;
            }
        }

        emit message(QString("QLinReadByID::dataReady: failed to read response after max: %1 reschedules").arg(set->maxReschedules()));
        m_data.m_state = QLinReadByIDState::ERR;
    }

    switch (m_data.m_state) {
    case QLinReadByIDState::READ:
        switch (response.value().ID) {
        case 0x3c:
            emit message("QLinReadByID::dataReady: got 0x3c");
            return;

        case 0x3d:
            emit message("QLinReadByID::dataReady: got 0x3d");
            frame = reinterpret_cast<const UDSFrame*>(response.value().payload);
            switch (static_cast<PCITypes>(frame->df.PCI.type)) {
            case PCITypes::CF:
                if (processConsecutiveFrame(&frame->cf) == true) {
                    return;
                }
                break;
            case PCITypes::FF:
                if (processFirstFrame(&frame->ff) == true) {
                    return;
                }
                break;
            case PCITypes::SF:
                if (processSingleFrame(&frame->sf) == true) {
                    return;
                }
                break;
            }
        }
        
        [[fallthrough]];

    case QLinReadByIDState::ERR:
        if (m_data.m_try < set->tries()) {
            emit message(QString("QLinReadByID::dataReady: try %1 of %3").arg(m_data.m_try).arg(set->tries()));
            if (m_data.m_try < set->tries()) {
                startRead();
            }
            ++m_data.m_try;
        } else {
            emit message("QLinReadByID::dataReady: entering failed state");
            m_data.m_state = QLinReadByIDState::ERR;
            stateFailed();
            emit failed();
        }
    }
}

bool QLinReadByID::processSingleFrame(const UDSsingleFrame* frame) {
    emit message("QLinReadByID::processSingleFrame(const UDSsingleFrame* frame)");
    if (frame->SID == 0x7F) {
        return false;
    }

    if (frame->SID - m_data.m_frame.data[0] != 0x40) {
        return false;
    }

    if (frame->DIDH != m_data.m_frame.data[1] || frame->DIDL != m_data.m_frame.data[2]) {
        return false;
    }

    if (m_data.m_result.size() > 0) {
        emit message("QLinReadByID::processSingleFrame: internal frame data not empty looks like first frame already processed");
        return false;
    }

    for (int i = 0; i < frame->PCI.len - 3; ++i) {
        const QString tmp = QString::number(frame->data[i], 16);
        m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
    }

    m_data.m_ui->result->setText(m_data.m_result.trimmed());
    setAsciiResult(m_data.m_result);
    m_data.m_state = QLinReadByIDState::SUCCESS;
    success();
    emit message(QString("QLinReadByID::processSingleFrame: '%1'").arg(m_data.m_result.trimmed()));
    emit success(m_data.m_result.trimmed().toLocal8Bit());
    
    return true;
}

bool QLinReadByID::processFirstFrame(const UDSfirstFrame* frame) {
    emit message("QLinReadByID::processFirstFrame(const UDSfirstFrame* frame)");
    if (frame->SID == 0x7F) {
        return false;
    }

    if (frame->SID - m_data.m_frame.data[0] != 0x40) {
        return false;
    }

    if (frame->DIDH != m_data.m_frame.data[1] || frame->DIDL != m_data.m_frame.data[2]) {
        return false;
    }

    auto set = settings<LinReadByIDSettings>();
    m_data.m_remaining = static_cast<int>(frame->LEN) - 5;

    QString tmp = QString::number(frame->data[0], 16);
    m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';

    tmp = QString::number(frame->data[1], 16);
    m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
    
    emit message(QString("QLinReadByID::processFirstFrame: data after processing: %1").arg(m_data.m_result));
    
    m_data.m_lin->write(QByteArray(1, 0x3d));
    QThread::msleep(set->interval());

    return true;
}

bool QLinReadByID::processConsecutiveFrame(const UDSconsecutiveFrame* frame) {
    emit message("QLinReadByID::processConsecutiveFrame(const UDSconsecutiveFrame* frame)");
    if (frame->NAD != m_data.m_frame.NAD) {
        return false;
    }

    auto set = settings<LinReadByIDSettings>();

    if (m_data.m_frames.indexOf(frame->PCI.len) != -1) {
        emit message(QString("QLinReadByID::processConsecutiveFrame: frame with index %1 already processed, discarding").arg(frame->PCI.len));
        QThread::msleep(set->interval());
        m_data.m_lin->write(QByteArray(1, 0x3d));
        return false;
    } else {
        emit message(QString("QLinReadByID::processConsecutiveFrame: adding frame with index %1 to processed list").arg(frame->PCI.len));
    }
    
    if (m_data.m_remaining > 6) {
        for (int i = 0; i < 6; ++i) {
            const QString tmp = QString::number(frame->data[i], 16);
            m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
        }
        QThread::msleep(set->interval());
        m_data.m_lin->write(QByteArray(1, 0x3d));
        m_data.m_remaining -= 6;
    } else {
        for (int i = 0; i < m_data.m_remaining; ++i) {
            const QString tmp = QString::number(frame->data[i], 16);
            m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
        }

        m_data.m_ui->result->setText(m_data.m_result);
        setAsciiResult(m_data.m_result);
        m_data.m_state = QLinReadByIDState::SUCCESS;
        success();
        emit message(QString("QLinReadByID::processConsecutiveFrame: '%1'").arg(m_data.m_result.trimmed()));
        emit success(m_data.m_result.trimmed().toLocal8Bit());
    }

    emit message(QString("QLinReadByID::processConsecutiveFrame: data after processing: %1").arg(m_data.m_result));

    return true;
}

std::optional<LinFrame> QLinReadByID::dataFromResponse(const QByteArray& data) const {

    if (data.startsWith("LIN NOANS")) {
        return std::nullopt;
    } 

    if (data.startsWith("ID")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }

        if (list[5].trimmed().toLongLong() < m_data.m_startTime) {
            emit message(QString("QLinReadByID::dataFromResponse: discarding packet older than start time %1 %2").arg(list[5]).arg(m_data.m_startTime));
            return std::nullopt;
        }

        QByteArray ret = QByteArray(1 + (list[2].size() - 2) / 2, 0);
        quint32 value;

        list[2] = list[2].trimmed();

        for (int i = 2; i < list[2].size(); i += 2) {
            QString tmp = list[2].mid(i, 2);
            value = tmp.toUInt(nullptr, 16);
            ret[1 + i / 2 - 1] = value;
        }

        LinFrame frame;
        frame.ID = list[0].toUInt(nullptr, 16);
        memcpy(frame.payload, ret.constData()+1, ret.size()-1);

        return frame;
    }

    return std::nullopt;
}

void QLinReadByID::success() {
    emit message("QLinReadByID::success()");
    m_data.m_ui->pushButton->setEnabled(true);
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold; color:green; }");
    m_data.m_ui->successLabel->setEnabled(true);
    setStyleSheet("QLinReadByID { border:2px solid green; }");
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
}

void QLinReadByID::stateFailed() {
    emit message("QLinReadByID::stateFailed()");
    m_data.m_ui->pushButton->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold; color:red; }");
    m_data.m_ui->successLabel->setStyleSheet("");
    m_data.m_ui->progressLabel->setStyleSheet("");
    m_data.m_ui->failedLabel->setEnabled(true);
    m_data.m_ui->progressLabel->setEnabled(false);
    setStyleSheet("QLinReadByID { border:2px solid red; }");
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
}

void QLinReadByID::inprogress() {
    emit message("QLinReadByID::inprogress()");
    m_data.m_ui->pushButton->setEnabled(false);
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold; color:blue; }");
    m_data.m_ui->progressLabel->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->successLabel->setEnabled(false);
    setStyleSheet("QLinReadByID { border:2px solid blue; }");
}

void QLinReadByID::initial() {
    emit message("QLinReadByID::initial()");
    m_data.m_ui->result->setText("");
    m_data.m_ui->resultAscii->setText("");
    m_data.m_ui->pushButton->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold;}");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold;}");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold;}");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
    setStyleSheet("");
    m_data.m_result.clear();
    m_data.m_frames.clear();
    m_data.m_ui->lenLabel->setText("");
}

void QLinReadByID::startRead() {
    emit message("QLinReadByID::startRead()");
    auto set = settings<LinReadByIDSettings>();
    QByteArray data = QByteArray(1 + (set->frameData().size() - 2) / 2, 0);
    quint32 value;
    
    m_data.m_resched = 0;

    if (m_data.m_lin == nullptr) {
        emit message("QLinReadByID::startRead: LIN device is not set or does not exist");
        return;
    }

    inprogress();
    m_data.m_state = QLinReadByIDState::READ;
    m_data.m_result.clear();

    for (int i = 2; i < set->frameData().size(); i += 2) {
        QString tmp = set->frameData().mid(i, 2);
        value = tmp.toUInt(nullptr, 16);
        data[1 + i / 2 - 1] = value;
    }

    data[0] = QString("0x3c").toUInt(nullptr, 16);
    if (m_data.m_lin->write(data) == -1) {
        emit message("QLinReadByID::startRead: failed to send packet");
        m_data.m_state = QLinReadByIDState::ERR;
        stateFailed();
        return;
    }
    memcpy(&m_data.m_frame, data.constData()+1, sizeof(m_data.m_frame));
    QThread::msleep(set->interval());
    data = QByteArray(1, 0);
    data[0] = QString("0x3d").toUInt(nullptr, 16);
    if(m_data.m_lin->write(data) == -1) {
        emit message("QLinReadByID::startRead: failed to send packet");
        m_data.m_state = QLinReadByIDState::ERR;
        stateFailed();
        return;
    }
    
}

void QLinReadByID::linClosed() {
    emit message("QLinReadByID::linClosed()");
    m_data.m_state = QLinReadByIDState::INITIAL;
    initial();
}

void QLinReadByID::linOpened() {
    emit message("QLinReadByID::linOpened()");
    m_data.m_state = QLinReadByIDState::INITIAL;
    initial();
}

void QLinReadByID::readById(bool checked) {
    emit message("QLinReadByID::readById(bool checked)");
    const auto set = settings<LinReadByIDSettings>();
    m_data.m_try = 0;
    m_data.m_startTime = QDateTime::currentMSecsSinceEpoch();
    startRead();
}