#include "QLinCommand.h"

#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_lincommand.h"

#include <QtGlobal>
#include <QtGui>
#include <QMenu>
#include <QStyle>

struct QLinCommandMenu {
    QLinCommandMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinCommand", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_QLinCommandMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinCommand_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinCommandMenu* ctx, Logger* log) {
    log->message("QLinCommand_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QLinCommand_register(): application is non gui not registering");
        return false;
    }

    auto linbusMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));

    if (linbusMenu == nullptr) {
        log->message("QLinCommand_register(): linbus menu not found");
        return false;
    }

    windowAddInstanceSettings(linbusMenu, &ctx->m_settings, &ctx->m_newInstance, "QLinCommand", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QLinCommand, QLinCommandMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLinCommand"), gtx, ctx, log);
    });

    return true;
}

static bool QLinCommand_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinCommandMenu* ctx, Logger* log) {
    log->message("QLinCommand_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QLinCommand,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinCommand_register,
    QLinCommand_unregister,
    QLinCommandMenu,
    { "QLin" },
    true,
    900,
    LinCommandSettings
)

QLinCommand::QLinCommand(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, LinCommandSettings* set, const QString& uuid)
    : Widget(ld, plugins, parent, settingsPath, set, uuid)
    , m_data(new Ui::QLinCommandUI) {
    m_data.m_ui->setupUi(this);
    //settingsChanged();
    QObject::connect(m_data.m_ui->pushButton, &QPushButton::clicked, this, &QLinCommand::sendCommand);
    //QTimer::singleShot(0, this, &QLinCommand::init);
}

SettingsMdi* QLinCommand::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinCommand::settingsChanged);
    return ret;
}

bool QLinCommand::initialize() {
    emit message("QLinCommand::init()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<LinCommandSettings>();
    
    m_data.m_ui->title->setText(set->title());
    m_data.m_ui->pushButton->setText(set->buttonText());

    if (set->linDevice().isEmpty()) {
        emit message("QLinReadByID::initialize: !!! lin device is empty please updated settings !!!");
        return false;
    }

    auto plugin = plugins()->instance(set->linDevice(), 0);
    auto lin = plugin.dynamicCast<IODevice>();
    m_data.m_lin = lin;
    connect(lin.data(), &IODevice::dataReady, this, &QLinCommand::dataReady);
    connect(lin.data(), &IODevice::error, this, &QLinCommand::errorReady);
    
    if (lin->isOpen() == false) {
        emit message(QString("QLinCommand::settingsChanged: %1 is closed").arg(set->linDevice()));
        if (lin->open() == false) {
            emit message(QString("QLinCommand::settingsChanged: failed to open %1 device").arg(set->linDevice()));
        }
    }

    QObject::connect(lin.data(), &IODevice::closed, this, &QLinCommand::linClosed);

    if (set->previous().isEmpty() == false) {
        connect(plugins()->findByObjectName(set->previous()).dynamicCast<QObject>().data(), SIGNAL(success(const QByteArray&)), this, SLOT(previousSuccess(const QByteArray&)));
    }

    return true;
}

bool QLinCommand::deinitialize() {
    return true;
}

void QLinCommand::settingsChanged() {
    emit message("QLinCommand::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<LinCommandSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<LinCommandSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

QLinCommand::~QLinCommand() {
}

void QLinCommand::dataReady(const QByteArray& data) {
    emit message("QLinCommand::dataReady(const QByteArray& data)");
    emit message(QString("QLinCommand::dataReady: recv %1").arg(data));

    if (m_data.m_state != QLinCommandState::READ) {
        emit message(QString("QLinCommand::dataReady(): not in read state, state: %1").arg(static_cast<int>(m_data.m_state)));
        return;
    }

    const auto set = settings<LinCommandSettings>();
    auto response = dataFromResponse(data);
    
    const UDSframe* frame = nullptr;

    if (response.has_value() == false) {
        emit message("QLinCommand::dataReady: no answer response request sent to early looks like device needs more time to process");

        emit message(QString("QLinCommand::dataReady: try %1 reschedule %2 of %3").arg(m_data.m_try).arg(m_data.m_resched).arg(set->maxReschedules()));
        if (m_data.m_resched < set->maxReschedules()) {
            ++m_data.m_resched;
            QByteArray data(1, 0);
            data[0] = QString("0x3d").toUInt(nullptr, 16);

            emit message(QString("QLinCommand::dataReady: waiting %1 before next response request").arg(set->rescheduleInterval()));
            QThread::msleep(set->rescheduleInterval());

            if (m_data.m_lin->write(data) != -1) {
                emit message("QLinCommand::dataReady: rescheduling read event successful");
                return;
            }
        }

        emit message(QString("QLinCommand::dataReady: failed to read response after max: %1 reschedules").arg(m_data.m_resched));
        m_data.m_state = QLinCommandState::ERR;
    }

    switch (m_data.m_state) {
    case QLinCommandState::READ:
        switch (response.value().ID) {
        case 0x3c:
            emit message("QLinCommand::dataReady: got 0x3c");
            return;

        case 0x3d:
            emit message("QLinCommand::dataReady: got 0x3d");
            frame = reinterpret_cast<const UDSframe*>(response.value().payload);
            if (processFrame(frame) == true) {
                m_data.m_state = QLinCommandState::SUCCESS;
                success();
                return;
            }
        }

        [[fallthrough]];

    case QLinCommandState::ERR:
        emit message("QLinCommand::dataReady: error state");
        emit message(QString("QLinCommand::dataReady: try %1 of %2").arg(m_data.m_try).arg(set->tries()));
        ++m_data.m_try;
        if (m_data.m_try < set->tries()) {
            emit message("QLinCommand::dataReady: sending next try");
            send();
        } else {
            emit message("QLinCommand::dataReady: entering failed state");
            m_data.m_state = QLinCommandState::ERR;
            failed();
        } 
    }
}

std::optional<LinFrame> QLinCommand::dataFromResponse(const QByteArray& data) const {
    emit message("QLinCommand::dataFromResponse(const QByteArray& data) const");

    if (data.startsWith("LIN NOANS")) {
        return std::nullopt;
    }

    if (data.startsWith("ID")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }

        if (list[5].trimmed().toLongLong() < m_data.m_startTime) {
            emit message(QString("QLinCommand::dataFromResponse: discarding packet older than start time %1 %2").arg(list[5]).arg(m_data.m_startTime));
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
        memcpy(frame.payload, ret.constData() + 1, ret.size() - 1);

        return frame;
    }

    return std::nullopt;
}

bool QLinCommand::processFrame(const UDSframe* frame) {
    emit message("QLinCommand::processFrame()");

    if (frame->generic.SID == 0x7F) {
        emit message("QLinCommand::processFrame: got negative response: "+errorString(frame->negative.SFID));
        return false;
    }

    if (frame->generic.SID - m_data.m_frame.SID != 0x40) {
        emit message("QLinCommand::processFrame: invalid response sid received");
        return false;
    }

    switch (frame->generic.SID-0x40) {
    case 0x10: //session control
        emit message("QLinCommand::processFrame: received session response");
        return true;

    case 0x11: //ecu reset
        emit message("QLinCommand::processFrame: received ecu reset response");
        return true;
    }

    return false;
}

void QLinCommand::success() {
    m_data.m_ui->successLabel->setStyleSheet("QLabel { color : green; font-weight:bold; }");
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(true);
    m_data.m_ui->pushButton->setEnabled(true);
    setStyleSheet("QLinCommand { border:2px solid green; }");
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
}

void QLinCommand::failed() {
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { color : red; font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight: bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight: bold; }");
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setEnabled(true);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->pushButton->setEnabled(true);
    setStyleSheet("QLinCommand { border:2px solid red; }");
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
}

void QLinCommand::inprogress() {
    m_data.m_ui->pushButton->setEnabled(false);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight: bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ color:blue; font-weight: bold; }");
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(true);
    setStyleSheet("QLinCommand { border:2px solid blue; }");
}

void QLinCommand::initial() {
    m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->pushButton->setEnabled(true);
    setStyleSheet("");
}

void QLinCommand::send() {
    emit message("QLinCommand::send()");

    if (m_data.m_lin.isNull()) {
        emit message("QLinCommand::send; lin device is nullptr");
        failed();
        return;
    }

    m_data.m_state = QLinCommandState::READ;
    m_data.m_resched = 0;
    inprogress();

    const auto set = settings<LinCommandSettings>();
    QByteArray data = QByteArray(1 + (set->frameData().size() - 2) / 2, 0);
    quint32 value;

    for (int i = 2; i < set->frameData().size(); i += 2) {
        QString tmp = set->frameData().mid(i, 2);
        value = tmp.toUInt(nullptr, 16);
        data[1 + i / 2 - 1] = value;
    }
    
    data[0] = QString("0x3c").toUInt(nullptr, 16);
    if (m_data.m_lin->write(data) != data.size()) {
        emit message("QLinCommand::send: Command request write failed");
        failed();
        return;
    }
    memcpy(&m_data.m_frame, data.constData() + 1, sizeof(m_data.m_frame));
    QThread::msleep(set->interval());
    //QCoreApplication::processEvents(QEventLoop::AllEvents, set->interval);
    data = QByteArray(1, 0);
    data[0] = QString("0x3d").toUInt(nullptr, 16);
    if (m_data.m_lin->write(data) != 1) {
        emit message("QLinCommand::send: Command response write failed");
        failed();
        return;
    }
    
    
}

void QLinCommand::linClosed() {
    emit message("QLinCommand::linClosed()");
    //m_data.m_ui->result->setText("");
}


void QLinCommand::previousSuccess(const QByteArray& data) {

}

void QLinCommand::sendCommand(bool checked) {
    emit message("QLinCommand::sendCommand()");
    const auto set = settings<LinCommandSettings>();
    m_data.m_try = 0;
    m_data.m_startTime = QDateTime::currentMSecsSinceEpoch();
    send();
}

QString QLinCommand::errorString(quint8 error) const {
    switch (error) {
    case 0x10:	return "General reject";
    case 0x11:	return "Service not supported";
    case 0x12:	return "Subfunction not supported";
    case 0x13:	return "Incorrect message length or invalid format";
    case 0x14:	return "Response too long";
    case 0x21:	return "Busy, repeat request";
    case 0x22:	return "Conditions not correct";
    case 0x24:	return "Request sequence error";
    case 0x25:	return "No response from subnet component";
    case 0x26:	return "Failure prevents execution of requested action";
    case 0x31:	return "Request out of range";
    case 0x33:	return "Security access denied";
    case 0x34:	return "Authentication failed";
    case 0x35:	return "Invalid key";
    case 0x36:	return "Exceeded number of attempts";
    case 0x37:	return "Required time delay not expired";
    case 0x38:	return "Secure data transmission required";
    case 0x39:	return "Secure data transmission not allowed";
    case 0x3A:	return "Secure data verification failed";
    case 0x50:	return "Certificate validation failed, invalid time period";
    case 0x51:	return "Certificate validation failed, invalid signature";
    case 0x52:	return "Certificate validation failed, invalid chain of trust";
    case 0x53:	return "Certificate validation failed, invalid type";
    case 0x54:	return "Certificate validation failed, invalid format";
    case 0x55:	return "Certificate validation failed, invalid content";
    case 0x56:	return "Certificate validation failed, invalid scope";
    case 0x57:	return "Certificate validation failed, invalid certificate";
    case 0x58:	return "Ownership verification failed";
    case 0x59:	return "Challenge calculation failed";
    case 0x5A:	return "Setting access right failed";
    case 0x5B:	return "Session key creation / derivation failed";
    case 0x5C:	return "Configuration data usage failed";
    case 0x5D:	return "Deauthentication failed";
    case 0x70:	return "Upload download not accepted";
    case 0x71:	return "Transfer data suspended";
    case 0x72:	return "General programming failure";
    case 0x73:	return "Wrong block sequence number";
    case 0x78:	return "Request correctly received, response pending";
    case 0x7E:	return "Subfunction not supported in active session";
    case 0x7F:	return "Service not supported in active session";
    case 0x81:	return "RPM too high";
    case 0x82:	return "RPM too low";
    case 0x83:	return "Engine is running";
    case 0x84:	return "Engine is not running";
    case 0x85:	return "Engine run time too low";
    case 0x86:	return "Temperature too high";
    case 0x87:	return "Temperature too low";
    case 0x88:	return "Vehicle speed too high";
    case 0x89:	return "Vehicle speed too low";
    case 0x8A:	return "Throttle / pedal too high";
    case 0x8B:	return "Throttle / pedal too low";
    case 0x8C:	return "Transmission range not in neutral";
    case 0x8D:	return "Transmission range not in gear";
    case 0x8F:	return "Brake switch not closed";
    case 0x90:	return "Shifter lever not in park";
    case 0x91:	return "Torque converter clutch locked";
    case 0x92:	return "Voltage too high";
    case 0x93:	return "Voltage too low";
    case 0x94:	return "Resource temporary unavailable";
    }
    
    return QString("Unknown error: %1").arg(QString::number(error, 16));
}

void QLinCommand::commandTimeout() {
    emit message("QLinCommand::commandTimeout()");

    if (m_data.m_state != QLinCommandState::READ) {
        emit message("QLinCommand::commandTimeout: not in read state");
        return;
    }
}

void QLinCommand::errorReady(const QByteArray& err) {
    emit message("QLinCommand::errorReady()");

    if (m_data.m_state != QLinCommandState::READ) {
        emit message("QLinCommand::errorReady: not in read state");
        return;
    }

    dataReady(QByteArray());
}