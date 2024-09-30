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


        m_QLinWriteByIDMenu = new QMenu(m_app->translate("MainWindow", "WriteByID"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        //m_QLinReadByIDMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_QLinWriteByIDMenu);
        m_newInstance->setData(QVariant("QLinWriteByID"));
        //m_QLinReadByIDMenu->addAction(m_newInstance);
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
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    ctx->m_QLinWriteByIDMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    //win->menuBar()->insertMenu(menu->menuAction(), ctx->m_linbusMenu);
    ctx->m_QLinWriteByIDMenu->addSection(ctx->m_app->translate("MainWindow", "Write By ID"));
    if (ctx->m_QLinWriteByIDMenu->style()->styleHint(QStyle::SH_Menu_SupportsSections) == 0) {
        ctx->m_QLinWriteByIDMenu->addAction(ctx->m_app->translate("MainWindow", "Write By ID"));
    }
    ctx->m_QLinWriteByIDMenu->addMenu(ctx->m_settings);
    ctx->m_QLinWriteByIDMenu->insertAction(nullptr, ctx->m_newInstance);


    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_QLinWriteByIDMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QLinWriteByID") {
            return;
        }

        QAction* settings = new QAction(dynamic_cast<const QLinWriteByID*>(plugin)->objectName(), ctx->m_QLinWriteByIDMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QLinWriteByID*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QLinWriteByID*>(plugin), &QLinWriteByID::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QLinWriteByID*>(plugin)->objectName() + "/Settings");  //ctx->m_app->translate("MainWindow", "QLinWriteByID/Settings"));
            });
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
    1100
)


QLinWriteByID::QLinWriteByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath, new SettingsDialog::LinWriteByIDSettings(Settings::get(), settingsPath))
    , m_data(new Ui::QLinWriteByIDUI) {
    m_data.m_ui->setupUi(this);
    settingsChanged();
    QObject::connect(m_data.m_ui->pushButton, &QPushButton::clicked, this, &QLinWriteByID::writeById);
    QTimer::singleShot(0, this, &QLinWriteByID::init);
}

QLinWriteByID::~QLinWriteByID() {
}

SettingsMdi* QLinWriteByID::settingsWindow() const {
   auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
   QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinWriteByID::settingsChanged);
   return ret;
}

void QLinWriteByID::dataReady(const QByteArray& data) {
    if (m_data.m_state != QLinWriteByIDState::WRITE) {
        return;
    }

    const auto response = dataFromResponse(data);

    const UDSFrame* frame;

    if (response.has_value() == false) {
        m_data.m_state = QLinWriteByIDState::ERR;
    }

    switch (m_data.m_state) {
    case QLinWriteByIDState::WRITE:
        switch (response.value().ID) {
        case 0x3c:
            return;

        case 0x3d:
            frame = reinterpret_cast<const UDSFrame*>(response.value().payload);
            if (processFrame(frame) == true) {
                m_data.m_ui->successLabel->setStyleSheet("QLabel { color : green; font-weight:bold; }");
                m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
                m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
                m_data.m_ui->failedLabel->setEnabled(false);
                m_data.m_ui->successLabel->setEnabled(true);
                m_data.m_ui->progressLabel->setEnabled(false);
            }
            
            /*
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
            */
        }
    
        [[fallthrough]];
    
    case QLinWriteByIDState::ERR:
        if (m_data.m_try > 0) {
            --m_data.m_try;
            startWrite();
        } else {
            m_data.m_state = QLinWriteByIDState::ERR;
            m_data.m_ui->failedLabel->setStyleSheet("QLabel { color : red; font-weight:bold; }");
            m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
            m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
            m_data.m_ui->successLabel->setEnabled(false);
            m_data.m_ui->failedLabel->setEnabled(true);
            m_data.m_ui->progressLabel->setEnabled(false);
        }
    }
}

bool QLinWriteByID::processFrame(const UDSFrame* frame) {
    
    if (frame->service.SID == 0x7F) {
        emit message("QLinWriteByID::processFrame: error response "+errorString(frame->negative.SFID));
        return false;
    }

    if (frame->service.SID - m_data.m_frame.data[0] != 0x40) {
        emit message("QLinWriteByID::processFrame: invalid response service id");
        return false;
    }

    return false;
}

std::optional<LinFrame> QLinWriteByID::dataFromResponse(const QByteArray& data) const {

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
        memcpy(frame.payload, ret.constData() + 1, ret.size() - 1);

        return frame;
    }

    return std::nullopt;
}

void QLinWriteByID::startWrite() {
    QByteArray data = m_data.m_ui->valueEdit->text().toLocal8Bit();
    QByteArrayList lst = data.split(' ');
    const auto set = settings<SettingsDialog::LinWriteByIDSettings>();

    QByteArray fdata = QByteArray(1 + (set->frameData.size() - 2) / 2, 0);
    quint32 value;

    for (int i = 2; i < set->frameData.size(); i += 2) {
        QString tmp = set->frameData.mid(i, 2);
        value = tmp.toUInt(nullptr, 16);
        fdata[1 + i / 2 - 1] = value;
    }

    const UDSserviceFrame* service = reinterpret_cast<const UDSserviceFrame*>(fdata.constData() + 1);
    m_data.m_frame = *(reinterpret_cast<const UDSgenericFrame*>(service));
    auto frames = prepareFrames(lst, service);

    m_data.m_state = QLinWriteByIDState::WRITE;
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; color:blue; }");
    m_data.m_ui->progressLabel->setEnabled(true);

    while (frames.size()) {
        auto packet = frames.dequeue();
        QByteArray buffer = packet;
        m_data.m_lin->write(buffer);
        QThread::msleep(set->interval);
    }    
}  

QQueue<LinUdsFrame> QLinWriteByID::prepareFrames(const QByteArrayList& data, const UDSserviceFrame* service) {
    QQueue<LinUdsFrame> ret;
    LinUdsFrame frame;

    if (data.size() <= 3) {
        frame.ID = 0x3c;
        frame.uds.sf.SID = service->SID;
        frame.uds.sf.NAD = service->NAD;
        frame.uds.sf.PCI.type = static_cast<quint8>(PCITypes::SF);
        frame.uds.sf.PCI.len = data.size();
        frame.uds.sf.DIDH = service->data[0];
        frame.uds.sf.DIDL = service->data[1];

        for (int i = 0; i < data.size(); ++i) {
            frame.uds.sf.data[i] = data[i].toUInt(nullptr, 16);
        }

        frame.length = 6 + data.size();
        ret.enqueue(frame);

        frame.ID = 0x3d;
        frame.length = 1;
        ret.enqueue(frame);
    } else {
        int i = 0;
        int remaining = data.size();

        //FF
        frame.ID = 0x3c;
        frame.uds.ff.NAD = service->NAD;
        frame.uds.sf.SID = service->SID;
        frame.uds.ff.PCI.type = static_cast<quint8>(PCITypes::FF);
        frame.uds.ff.PCI.len = data.size()+3;
        frame.uds.ff.DIDH = service->data[0];
        frame.uds.ff.DIDL = service->data[1];
        frame.uds.ff.data[0] = data[i++].toUInt(nullptr, 16);
        frame.uds.ff.data[1] = data[i++].toUInt(nullptr, 16);
        frame.length = 9;
        remaining -= i;
        ret.enqueue(frame);

        //CF
        for (int j = 0, rem = ceil(remaining / 6); j < rem; ++j) {
            frame.ID = 0x3c;
            frame.uds.cf.NAD = service->NAD;
            frame.uds.cf.PCI.type = static_cast<quint8>(PCITypes::CF);
            frame.uds.cf.PCI.len = 0;
            int k = 0;
            for (; k < std::min(6, remaining); ++k) {
                frame.uds.cf.data[k] = data[i++].toUInt(nullptr, 16);
            }
            remaining -= k;
            frame.length = 3 + k;
            ret.enqueue(frame);
        }

        frame.ID = 0x3d;
        frame.length = 1;
        ret.enqueue(frame);
    }

    return ret;
}

void QLinWriteByID::init() {
    emit message("QLinTester::init()", LoggerSeverity::LOG_DEBUG);
    auto plugin = plugins()->instance("QLin", 0);
    auto lin = plugin.dynamicCast<IODevice>();
    m_data.m_lin = lin;
    connect(lin.data(), &IODevice::dataReady, this, &QLinWriteByID::dataReady);
    if (lin->isOpen() == false) {
        lin->open();
    }

    QObject::connect(lin.data(), &IODevice::closed, this, &QLinWriteByID::linClosed);
    
    const auto set = settings<SettingsDialog::LinWriteByIDSettings>();
    if (set->dataSource.isEmpty() == false) {
        auto source = plugins()->findByObjectName(set->dataSource);
        if (source.isNull() == false) {
            QObject::connect(dynamic_cast<QObject*>(source.data()), SIGNAL(success(const QByteArray&)), this, SLOT(fillInput(const QByteArray&)));
        }
    }
}

void QLinWriteByID::settingsChanged() {
    emit message("QBadge::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<SettingsDialog::LinWriteByIDSettings>();
    QObject::disconnect(this, SLOT(fillInput(const QByteArray&)));
    *set = SettingsDialog::LinWriteByIDSettings(Settings::get(), settingsPath());
    m_data.m_ui->title->setText(set->title);
    
    if (set->dataSource.isEmpty() == false) {
        auto source = plugins()->findByObjectName(set->dataSource);
        if (source.isNull() == false) {
            QObject::connect(dynamic_cast<QObject*>(source.data()), SIGNAL(success(const QByteArray&)), this, SLOT(fillInput(const QByteArray&)));
        }
    }
}

void QLinWriteByID::fillInput(const QByteArray& data) {
    m_data.m_ui->valueEdit->setText(data);
}

void QLinWriteByID::linClosed() {
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->valueEdit->setText("");
    
}

void QLinWriteByID::writeById() {
    emit message("QLinWriteByID::writeById()");

    if (m_data.m_ui->valueEdit->text().isEmpty() == true) {
        emit message("QLinWriteByID::writeById()");
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