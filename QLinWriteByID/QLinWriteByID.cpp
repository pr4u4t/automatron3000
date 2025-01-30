#include "QLinWriteByID.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_linwritebyid.h"

#include <QtGlobal>
#include <QtGui>
#include <QStyle>

struct QLinWriteByIDMenu {
    QLinWriteByIDMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinWriteByID", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_QLinWriteByIDMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinWriteByID_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinWriteByIDMenu* ctx, Logger* log) {
    log->message("QLinWriteByID_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QLinWritedByID_register(): application is non gui not registering");
        return false;
    }

    auto linbusMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));

    if (linbusMenu == nullptr) {
        log->message("QLinWriteByID_register(): linbus menu not found");
        return false;
    }

    windowAddInstanceSettings(linbusMenu, &ctx->m_settings, &ctx->m_newInstance, "QLinWriteByID", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QLinWriteByID, QLinWriteByIDMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLinWriteByID"), gtx, ctx, log);
    });

    return true;
}

static bool QLinWriteByID_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinWriteByIDMenu* ctx, Logger* log) {
    log->message("QLinWriteByID_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QLinWriteByID,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinWriteByID_register,
    QLinWriteByID_unregister,
    QLinWriteByIDMenu,
    { "QLin" },
    true,
    1100,
    LinWriteByIDSettings
)


QLinWriteByID::QLinWriteByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, LinWriteByIDSettings* set, const QString& uuid)
    : Widget(ld, plugins, parent, settingsPath, set, uuid)
    , m_data(new Ui::QLinWriteByIDUI) {
    m_data.m_ui->setupUi(this);
    QObject::connect(m_data.m_ui->pushButton, &QPushButton::clicked, this, &QLinWriteByID::writeById);
}

QLinWriteByID::~QLinWriteByID() {
}

bool QLinWriteByID::initialize() {
    emit message("QLinWriteByID::init()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<LinWriteByIDSettings>();

    m_data.m_ui->title->setText(set->title());

    if (set->linDevice().isEmpty()) {
        emit message("QLinWriteByID::init: lin device name empty");
        emit message("QLinWriteByID::init: !!! please select lin device in settings !!!");
        return false;
    }

    auto plugin = plugins()->instance(set->linDevice(), 0);
    m_data.m_lin = plugin.dynamicCast<IODevice>();

    if (m_data.m_lin.isNull()) {
        emit message("QLinWriteByID::init: lin device == null");
        emit message("QLinWriteByID::init: !!! please select lin device in settings !!!");
        return false;
    }

    connect(m_data.m_lin.data(), &IODevice::dataReady, this, &QLinWriteByID::dataReady);
    if (m_data.m_lin->isOpen() == false) {
        if (m_data.m_lin->open() == false) {
            emit message(QString("QLinWriteByID::init: failed to open device: %1").arg(set->linDevice()));
        }
    }

    connect(m_data.m_lin.data(), &IODevice::closed, this, &QLinWriteByID::linClosed);

    //

    
    if (set->dataSource().isEmpty() == false) {
        emit message(QString("QLinWriteByID::settingsChanged: connecting data source %1").arg(set->dataSource()));
        auto source = plugins()->findByObjectName(set->dataSource());
        if (source.isNull() == false) {
            connect(dynamic_cast<QObject*>(source.data()), SIGNAL(success(const QByteArray&)), this, SLOT(fillInput(const QByteArray&)));
        }
    }

    return true;
}

bool QLinWriteByID::deinitialize() {
    emit message("QLinWriteByID::deinitialize()");
    return true;
}

void QLinWriteByID::settingsChanged() {
    emit message("QLinWriteByID::settingsChanged()", LoggerSeverity::LOG_DEBUG);

    const auto set = settings<LinWriteByIDSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<LinWriteByIDSettings>();
    *set = *nset;

    disconnect(this, SLOT(dataReady(const QByteArray&)));
    disconnect(this, SLOT(linClosed()));
    disconnect(this, SLOT(fillInput(const QByteArray&)));

    initialize();

    emit settingsApplied();
}

SettingsMdi* QLinWriteByID::settingsWindow() const {
   auto ret = new SettingsDialog(nullptr, this);
   QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinWriteByID::settingsChanged);
   return ret;
}

void QLinWriteByID::dataReady(const QByteArray& data) {
    emit message("QLinWriteByID::dataReady(const QByteArray& data)");
    if (m_data.m_state != QLinWriteByIDState::WRITE) {
        emit message("QLinWriteByID::dataReady: not in write state");
        return;
    }

    const auto response = dataFromResponse(data);
    const auto set = settings<LinWriteByIDSettings>();
    const UDSFrame* frame;

    if (response.has_value() == false) {
        emit message("QLinWriteByID::dataReady: no answer response request sent to early looks like device needs more time to process");

        if (m_data.m_resched < set->maxReschedules()) {
            ++m_data.m_resched;
            QByteArray data(1, 0);
            data[0] = QString("0x3d").toUInt(nullptr, 16);

            QThread::msleep(set->rescheduleInterval());

            if (m_data.m_lin->write(data) != -1) {
                emit message("QLinWriteByID::dataReady: rescheduling read event successful");
                return;
            }
        }


        m_data.m_state = QLinWriteByIDState::ERR;
    } else {
        emit message(QString("QLinWriteByID::dataReady: response: %1").arg(response.value().dump()));
    }

    switch (m_data.m_state) {
    case QLinWriteByIDState::WRITE:
        emit message("QLinWriteByID::dataReady: write state");
        switch (response.value().ID) {
        case 0x3c:
            emit message("QLinWriteByID::dataReady: got 0x3c");
            return;

        case 0x3d:
            emit message("QLinWriteByID::dataReady: got 0x3d");
            frame = reinterpret_cast<const UDSFrame*>(response.value().payload);
            if (processFrame(frame) == true) {
                success();
                m_data.m_state = QLinWriteByIDState::SUCCESS;
                return;
            }
        }
    
        [[fallthrough]];
    
    case QLinWriteByIDState::ERR:
        emit message("QLinWriteByID::dataReady: error occured");
        if (m_data.m_try < set->tries()) {
            emit message(QString("QLinWriteByID::dataReady: attempting try %1 of %2").arg(m_data.m_try).arg(set->tries()));
            ++m_data.m_try;
            startWrite();
        } else {
            emit message("QLinWriteByID::dataReady(const QByteArray& data): entering failed state");
            m_data.m_state = QLinWriteByIDState::ERR;
            failed();
        }
    }
}

bool QLinWriteByID::processFrame(const UDSFrame* frame) {
    emit message("QLinWriteByID::processFrame(const UDSFrame* frame)");

    emit message(QString("QLinWriteByID::processFrame: service id: %1").arg(QString::number(frame->service.SID,16)));

    if (frame->service.SID == 0x7F) {
        emit message("QLinWriteByID::processFrame: error response "+errorString(frame->negative.SFID));
        return false;
    }

    if (frame->service.SID - 0x2e != 0x40) {
        emit message("QLinWriteByID::processFrame: invalid response service id");
        return false;
    }

    return true;
}

std::optional<LinFrame> QLinWriteByID::dataFromResponse(const QByteArray& data) const {
    emit message("QLinWriteByID::dataFromResponse(const QByteArray& data)");
    emit message(QString("QLinWriteByID::dataFromResponse: %1").arg(data));

    if (data.startsWith("LIN NOANS")) {
        emit message("QLinWriteByID::dataFromResponse: LIN NOANS - returning nullopt");
        return std::nullopt;
    }

    if (data.startsWith("ID")) {
        emit message(QString("QLinWriteByID::dataFromResponse: got normal frame with Frame: %1").arg(data));
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
        memcpy(frame.payload, ret.constData() + 1, ret.size() - 1);

        return frame;
    }

    emit message("QLinWriteByID::dataFromResponse: returning null");

    return std::nullopt;
}

void QLinWriteByID::startWrite() {
    emit message("QLinWriteByID::startWrite()");
    QByteArray data = m_data.m_ui->valueEdit->text().toLocal8Bit();
    QByteArrayList lst = data.split(' ');
    const auto set = settings<LinWriteByIDSettings>();

    
    auto frames = prepareFrames(lst); // , service);
    m_data.m_resched = 0;
    m_data.m_state = QLinWriteByIDState::WRITE;
    inprogress();

    if (m_data.m_lin == nullptr) {
        emit message("QLinWriteByID::startWrite: LIN device is nullptr");
        failed();
        return;
    }

    while (frames.size()) {
        auto packet = frames.dequeue();
        QByteArray buffer = packet;
        m_data.m_lin->write(buffer);
        QThread::msleep(set->interval());
    }    
}  

QQueue<LinUdsFrame> QLinWriteByID::prepareFrames(const QByteArrayList& data) { //, const UDSserviceFrame* service) {
    emit message("QLinWriteByID::prepareFrames(const QByteArrayList& data)");
    QQueue<LinUdsFrame> ret;
    LinUdsFrame frame;
    const auto set = settings<LinWriteByIDSettings>();

    if (data.size() <= 3) {
        frame.ID = 0x3c;
        frame.uds.sf.NAD = set->nad().toUInt(nullptr, 16);//service->NAD;
        frame.uds.sf.SID = 0x2e; //this is service identifier see: https://en.wikipedia.org/wiki/Unified_Diagnostic_Services
        frame.uds.sf.PCI.type = static_cast<quint8>(PCITypes::SF);
        frame.uds.sf.PCI.len = data.size();
        frame.uds.sf.DIDH = set->didh().toUInt(nullptr, 16);
        frame.uds.sf.DIDL = set->didl().toUInt(nullptr, 16);

        for (int i = 0; i < data.size(); ++i) {
            frame.uds.sf.data[i] = data[i].toUInt(nullptr, 16);
        }

        frame.length = 6 + data.size();
        ret.enqueue(frame);

        emit message(QString("QLinWriteByID::prepareFrames: frame SF: %1").arg(frame.dump()));

        frame.ID = 0x3d;
        frame.length = 1;
        ret.enqueue(frame);
    } else {
        int i = 0;
        int remaining = data.size();

        //FF
        frame.ID = 0x3c;
        frame.uds.ff.NAD = set->nad().toUInt(nullptr, 16); //service->NAD;
        frame.uds.ff.pci.type = 1 << 4;
        frame.uds.ff.pci.len = static_cast<quint16>(data.size() + 3);
        frame.uds.ff.SID = 0x2e;  //service->SID;
        frame.uds.ff.DIDH = set->didh().toUInt(nullptr, 16);
        frame.uds.ff.DIDL = set->didl().toUInt(nullptr, 16);
        frame.uds.ff.data[0] = data[i++].toUInt(nullptr, 16);
        frame.uds.ff.data[1] = data[i++].toUInt(nullptr, 16);
        frame.length = 9;
        remaining -= i;
        ret.enqueue(frame);

        emit message(QString("QLinWriteByID::prepareFrames: frame 0: %1").arg(frame.dump()));

        //CF
        
        for (int j = 0, rem = ceil(remaining / 6); j < rem; ++j) {
            frame.ID = 0x3c;
            frame.uds.cf.NAD = set->nad().toUInt(nullptr, 16);
            frame.uds.cf.PCI.type = static_cast<quint8>(PCITypes::CF);
            frame.uds.cf.PCI.len = j+1;
            int k = 0;
            for (; k < std::min(6, remaining); ++k) {
                frame.uds.cf.data[k] = data[i++].toUInt(nullptr, 16);
            }
            remaining -= k;
            frame.length = 3 + k;
            ret.enqueue(frame);

            emit message(QString("QLinWriteByID::prepareFrames: frame %1: %2").arg(j+1).arg(frame.dump()));
        }
        

        frame.ID = 0x3d;
        frame.length = 1;
        ret.enqueue(frame);
    }

    return ret;
}

void QLinWriteByID::fillInput(const QByteArray& data) {
    emit message("QLinWriteByID::fillInput(const QByteArray& data)");
    emit message(QString("QLinWriteByID::fillInput: new value: %1").arg(data));
    m_data.m_ui->valueEdit->setText(data);
}

void QLinWriteByID::linClosed() {
    initial();
}

void QLinWriteByID::writeById() {
    emit message("QLinWriteByID::writeById()");
    m_data.m_try = 0;
    
    if (m_data.m_ui->valueEdit->text().isEmpty() == true) {
        emit message("QLinWriteByID::writeById: input is empty");
        failed();
        return;
    }

    startWrite();
}

QString QLinWriteByID::errorString(quint8 error) const {
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

void QLinWriteByID::success() {
    emit message("QLinWriteByID::success()");
    m_data.m_ui->successLabel->setStyleSheet("QLabel { color : green; font-weight:bold; }");
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(true);
    m_data.m_ui->progressLabel->setEnabled(false);
    setStyleSheet("QLinWriteByID { border:2px solid green; }");
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
}

void QLinWriteByID::failed() {
    emit message("QLinWriteByID::failed()");
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { color : red; font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setEnabled(true);
    m_data.m_ui->progressLabel->setEnabled(false);
    setStyleSheet("QLinWriteByID { border:2px solid red; }");
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
}

void QLinWriteByID::inprogress() {
    emit message("QLinWriteByID::inprogress()");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; color:blue; }");
    m_data.m_ui->progressLabel->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->successLabel->setEnabled(false);
    setStyleSheet("QLinWriteByID { border:2px solid blue; }");
}

void QLinWriteByID::initial() {
    emit message("QLinWriteByID::initial()");
    m_data.m_state = QLinWriteByIDState::INITIAL;
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setEnabled(false);
    //m_data.m_ui->valueEdit->setText("");
    fillInput("");
    setStyleSheet("");
}