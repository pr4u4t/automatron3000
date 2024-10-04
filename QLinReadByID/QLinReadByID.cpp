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


        m_QLinReadByIDMenu = new QMenu(m_app->translate("MainWindow", "ReadByID"));
        
        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        //m_QLinReadByIDMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_QLinReadByIDMenu);
        m_newInstance->setData(QVariant("QLinReadByID"));
        //m_QLinReadByIDMenu->addAction(m_newInstance);
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
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    ctx->m_QLinReadByIDMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    //win->menuBar()->insertMenu(menu->menuAction(), ctx->m_linbusMenu);
    ctx->m_QLinReadByIDMenu->addSection(ctx->m_app->translate("MainWindow", "Read By ID"));
    if (ctx->m_QLinReadByIDMenu->style()->styleHint(QStyle::SH_Menu_SupportsSections) == 0) {
        ctx->m_QLinReadByIDMenu->addAction(ctx->m_app->translate("MainWindow", "Read By ID"));
    }
    ctx->m_QLinReadByIDMenu->addMenu(ctx->m_settings);
    ctx->m_QLinReadByIDMenu->insertAction(nullptr, ctx->m_newInstance);


    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_QLinReadByIDMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QLinReadByID") {
            return;
        }

        QAction* settings = new QAction(dynamic_cast<const QObject*>(plugin)->objectName(), ctx->m_QLinReadByIDMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QLinReadByID*>(plugin)->objectName() + "/Settings")) {
                return;
            }

            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QLinReadByID*>(plugin), &QLinReadByID::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QLinReadByID*>(plugin)->objectName() + "/Settings"); //ctx->m_app->translate("MainWindow", "QLinReadByID/Settings"));
            });
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
    900
)

QLinReadByID::QLinReadByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath, new SettingsDialog::LinReadByIDSettings(Settings::get(), settingsPath))
    , m_data(new Ui::QLinReadByIDUI) {
    m_data.m_ui->setupUi(this);
    settingsChanged();
    QObject::connect(m_data.m_ui->pushButton, &QPushButton::clicked, this, &QLinReadByID::readById);
    QTimer::singleShot(0, this, &QLinReadByID::init);
}

SettingsMdi* QLinReadByID::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinReadByID::settingsChanged);
    return ret;
}

QLinReadByID::~QLinReadByID() {
}

void QLinReadByID::dataReady(const QByteArray& data) {
    if (m_data.m_state != QLinReadByIDState::READ) {
        return;
    }

    auto response = dataFromResponse(data);
    const UDSFrame* frame;

    if (response.has_value() == false) {
        m_data.m_state = QLinReadByIDState::ERR;
    }

    switch (m_data.m_state) {
    case QLinReadByIDState::READ:
        switch (response.value().ID) {
        case 0x3c:
            return;

        case 0x3d:
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
                    m_data.m_state = QLinReadByIDState::SUCCESS;
                    return;
                }
                break;
            }
        }
        
        [[fallthrough]];

    case QLinReadByIDState::ERR:
        if (m_data.m_try > 0) {
            --m_data.m_try;
            startRead();
        } else {
            m_data.m_state = QLinReadByIDState::ERR;
            stateFailed();
        }
    }
}

bool QLinReadByID::processSingleFrame(const UDSsingleFrame* frame) {
    if (frame->SID == 0x7F) {
        return false;
    }

    if (frame->SID - m_data.m_frame.data[0] != 0x40) {
        return false;
    }

    if (frame->DIDH != m_data.m_frame.data[1] || frame->DIDL != m_data.m_frame.data[2]) {
        return false;
    }

    for (int i = 0; i < frame->PCI.len - 3; ++i) {
        const QString tmp = QString::number(frame->data[i], 16);
        m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
    }

    m_data.m_ui->result->setText(m_data.m_result.trimmed());
    emit success(m_data.m_result.trimmed().toLocal8Bit());
    success();
    return true;
}

bool QLinReadByID::processFirstFrame(const UDSfirstFrame* frame) {
    if (frame->SID == 0x7F) {
        return false;
    }

    if (frame->SID - m_data.m_frame.data[0] != 0x40) {
        return false;
    }

    if (frame->DIDH != m_data.m_frame.data[1] || frame->DIDL != m_data.m_frame.data[2]) {
        return false;
    }

    auto set = settings<SettingsDialog::LinReadByIDSettings>();
    m_data.m_remaining = static_cast<int>(frame->LEN) - 5;

    QString tmp = QString::number(frame->data[0], 16);
    m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';

    tmp = QString::number(frame->data[1], 16);
    m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
    m_data.m_lin->write(QByteArray(1, 0x3d));
    QThread::msleep(set->interval);

    return true;
}

bool QLinReadByID::processConsecutiveFrame(const UDSconsecutiveFrame* frame) {
    if (frame->NAD != m_data.m_frame.NAD) {
        return false;
    }

    auto set = settings<SettingsDialog::LinReadByIDSettings>();

    if (m_data.m_remaining > 6) {
        for (int i = 0; i < 6; ++i) {
            const QString tmp = QString::number(frame->data[i], 16);
            m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
        }
        QThread::msleep(set->interval);
        m_data.m_lin->write(QByteArray(1, 0x3d));
        m_data.m_remaining -= 6;
    } else {
        for (int i = 0; i < m_data.m_remaining; ++i) {
            const QString tmp = QString::number(frame->data[i], 16);
            m_data.m_result += (tmp.size() == 2) ? tmp + ' ' : '0' + tmp + ' ';
        }

        m_data.m_ui->result->setText(m_data.m_result);
        emit success(m_data.m_result.trimmed().toLocal8Bit());
        success();
        m_data.m_state = QLinReadByIDState::SUCCESS;
    }

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
    m_data.m_ui->pushButton->setEnabled(true);
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold; color:green; }");
    m_data.m_ui->successLabel->setEnabled(true);
    setStyleSheet("QLinReadByID { border:2px solid green; }");
}

void QLinReadByID::stateFailed() {
    m_data.m_ui->pushButton->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold; color:red; }");
    m_data.m_ui->successLabel->setStyleSheet("");
    m_data.m_ui->progressLabel->setStyleSheet("");
    m_data.m_ui->failedLabel->setEnabled(true);
    m_data.m_ui->progressLabel->setEnabled(false);
    setStyleSheet("QLinReadByID { border:2px solid red; }");
}

void QLinReadByID::inprogress() {
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
    m_data.m_ui->result->setText("");
    m_data.m_ui->pushButton->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold;}");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold;}");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold;}");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
}

void QLinReadByID::startRead() {
    auto set = settings<SettingsDialog::LinReadByIDSettings>();
    QByteArray data = QByteArray(1 + (set->frameData.size() - 2) / 2, 0);
    quint32 value;
    
    inprogress();

    QCoreApplication::processEvents();
    for (int i = 2; i < set->frameData.size(); i += 2) {
        QString tmp = set->frameData.mid(i, 2);
        value = tmp.toUInt(nullptr, 16);
        data[1 + i / 2 - 1] = value;
    }

    data[0] = QString("0x3c").toUInt(nullptr, 16);
    if (m_data.m_lin->write(data) == -1) {
        emit message("QLinReadByID::startRead: failed to send packet");
        stateFailed();
        return;
    }
    memcpy(&m_data.m_frame, data.constData()+1, sizeof(m_data.m_frame));
    QThread::msleep(set->interval);
    data = QByteArray(1, 0);
    data[0] = QString("0x3d").toUInt(nullptr, 16);
    if(m_data.m_lin->write(data) == -1) {
        emit message("QLinReadByID::startRead: failed to send packet");
        stateFailed();
        return;
    }
    m_data.m_state = QLinReadByIDState::READ;
    m_data.m_result.clear();
}

void QLinReadByID::linClosed() {
    m_data.m_state = QLinReadByIDState::INITIAL;
    initial();
}

void QLinReadByID::linOpened() {
    m_data.m_state = QLinReadByIDState::INITIAL;
    initial();
}

void QLinReadByID::init() {
    emit message("QLinTester::init()", LoggerSeverity::LOG_DEBUG);
    auto plugin = plugins()->instance("QLin", 0);
    auto lin = plugin.dynamicCast<IODevice>();
    m_data.m_lin = lin;
    connect(lin.data(), &IODevice::dataReady, this, &QLinReadByID::dataReady);
    if (lin->isOpen() == false) {
        lin->open();
    }

    QObject::connect(lin.data(), &IODevice::closed, this, &QLinReadByID::linClosed);
}

void QLinReadByID::settingsChanged() {
    emit message("QBadge::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<SettingsDialog::LinReadByIDSettings>();
    *(set) = SettingsDialog::LinReadByIDSettings(Settings::get(), settingsPath());
    m_data.m_ui->title->setText(set->title);
    QObject::disconnect(this, SLOT(previousSuccess(const QByteArray&)));
    //if(set->)
}

void QLinReadByID::readById(bool checked) {
    const auto set = settings<SettingsDialog::LinReadByIDSettings>();
    m_data.m_try = set->tries;
    startRead();
}