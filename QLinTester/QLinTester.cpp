#include "QLinTester.h"
#include <QTranslator>
#include <QMessageBox>

#include "../core/core.h"
#include "settingsdialog.h"

struct QLinTesterMenu {
    QLinTesterMenu(QCoreApplication* app)
        : m_app(app) {
        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinTester", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QAction* m_newInstance = nullptr;
    QMenu* m_settings = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinTester_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinTesterMenu* ctx, Logger* log) {
    log->message("QLinTester_register()");
    
    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QLinTester_register(): application is non gui not registering");
        return false;
    }

    auto linbusMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));

    if (linbusMenu == nullptr) {
        log->message("QLinTester_register(): linbus menu not found");
        return false;
    }

    windowAddInstanceSettings(linbusMenu, &ctx->m_settings, &ctx->m_newInstance, "QLinTester", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QLinTester, QLinTesterMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLinTester"), gtx, ctx, log);
    });

    return true;
}

static bool QLinTester_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinTesterMenu* ctx, Logger* log) {
    log->message("QLinTester_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QLinTester,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinTester_register,
    QLinTester_unregister,
    QLinTesterMenu,
    { "QLin" },
    true,
    1000,
    LinTesterSettings
)

QLinTester::QLinTester(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, LinTesterSettings* set, const QString& uuid)
	: Widget(ld, plugins, parent, settingsPath, set, uuid)
    , m_data(new Ui::QLinTesterUI) {
    m_data.m_ui->setupUi(this);
    connect(m_data.m_ui->testButton, &QPushButton::clicked, this, &QLinTester::startTest);
    connect(m_data.m_ui->pushButton, &QPushButton::clicked, this, &QLinTester::testStop);
    connect(&m_data.m_timer, &QTimer::timeout, this, &QLinTester::testStep);
    m_data.m_ui->passedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
}

bool QLinTester::initialize() {
    emit message("QLinTester::initialize()", LoggerSeverity::LOG_DEBUG);
    
    const auto set = settings<LinTesterSettings>();

    if (set->linDevice().isEmpty()) {
        emit message("QLinTester::init: lin device name empty");
        emit message("QLinTester::init: !!! please select lin device in settings !!!");
        return false;
    }

    auto plugin = plugins()->instance(set->linDevice(), 0);
    m_data.m_lin = plugin.dynamicCast<IODevice>();

    if (m_data.m_lin.isNull()) {
        emit message("QLinTester::init: lin device == null");
        emit message("QLinTester::init: !!! please select lin device in settings !!!");
        return false;
    }

    connect(m_data.m_lin.data(), &IODevice::dataReady, this, &QLinTester::dataReady);
    
    if (m_data.m_lin->isOpen() == false) {
        if (m_data.m_lin->open() == false) {
            emit message(QString("QLinTester::init: failed to open lin device %1").arg(m_data.m_lin->objectName()));
        }
    }

    QObject::connect(m_data.m_lin.data(), &IODevice::closed, this, &QLinTester::linClosed);

    return true;
}

bool QLinTester::deinitialize() {
    return true;
}

void QLinTester::settingsChanged() {
    emit message("QLinTester::settingsChanged()");
    const auto set = settings<LinTesterSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<LinTesterSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

SettingsMdi* QLinTester::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinTester::settingsChanged);
    return ret;
}

void QLinTester::dataReady(const QByteArray& data) {
    emit message("QLinTester::dataReady()");

    if (m_data.m_state != QLinTesterState::SCAN) {
        emit message("QLinTester::dataReady(): not in scan state");
        return;
    }

    emit message("QLinTester::dataReady(): " + data, LoggerSeverity::LOG_DEBUG);

    if (data.startsWith("LIN NOANS")) {
        emit message("QLinTester::dataReady(): LIN NOANS", LoggerSeverity::LOG_DEBUG);
        return;
    }

    if (data.startsWith("ID")) {
        ++m_data.m_responses;
    }
}

void QLinTester::startTest() {
    emit message("QLinTester::startScan()", LoggerSeverity::LOG_DEBUG);

    if (m_data.m_lin.isNull() || m_data.m_lin->isOpen() == false) {
        emit message("QLinTester::startTest(): failed LIN device not open");
        QMessageBox::critical(
            this,
            tr("QLinBus: error"),
            tr("LIN device not open"),
            QMessageBox::StandardButton::Ok
        );
        failed();
        return;
    }

    if (m_data.m_try == 0) {
        QMetaObject::invokeMethod(m_data.m_lin.data(), "slaveID", Qt::DirectConnection,
            Q_RETURN_ARG(int, m_data.m_slaveID));
    }

    auto set = settings<LinTesterSettings>();

    switch (m_data.m_state) {
    case QLinTesterState::STOP:
    case QLinTesterState::INITIAL:
        m_data.m_responses = 0;
        m_data.m_ui->testProgress->setRange(0, set->tries()*(set->testStopID() - set->testStartID()));
        initial();
    case QLinTesterState::NEXT_TRY:
    case QLinTesterState::PAUSE:
        m_data.m_state = QLinTesterState::SCAN;
        m_data.m_timer.start();
        inprogress();
        break;
    }
}

void QLinTester::linClosed() {
    m_data.m_responses = 0;
    m_data.m_ui->testProgress->setValue(0);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->passedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->passedLabel->setEnabled(false);
    m_data.m_try = 0;
    m_data.m_state = QLinTesterState::INITIAL;
}

void QLinTester::testStep() {
    emit message("QLinTester::testStep()");

    auto set = settings<LinTesterSettings>();

    if (m_data.m_test <= set->testStopID()) {
        if (m_data.m_test != m_data.m_slaveID) {
            QByteArray data(1, static_cast<char>(m_data.m_test));
            m_data.m_lin->write(data);
            m_data.m_ui->testProgress->setValue(m_data.m_try * (set->testStopID() - set->testStartID()) + m_data.m_test);
        }
        ++m_data.m_test;
    } else {
        m_data.m_state = QLinTesterState::STOP;
        m_data.m_test = set->testStartID();
        m_data.m_timer.stop();
        ++m_data.m_try;
        if (m_data.m_try < set->tries()) {
            m_data.m_state = QLinTesterState::NEXT_TRY;
            startTest();
        } else {
            m_data.m_state = QLinTesterState::INITIAL;
            m_data.m_try = 0;
            if (m_data.m_responses > 0) {
                success();
            } else {
                failed();
            }
        }
    }
}

void QLinTester::testStop() {
    emit message("QLinTester::scanStop()", LoggerSeverity::LOG_DEBUG);
    m_data.m_state = QLinTesterState::STOP;
    m_data.m_test = settings<LinTesterSettings>()->testStartID();
    m_data.m_timer.stop();
    m_data.m_ui->testButton->setEnabled(true);
    m_data.m_ui->pushButton->setEnabled(false);
    
    //m_ui->pauseButton->setEnabled(false);
}

void QLinTester::success() {
    m_data.m_ui->passedLabel->setStyleSheet("QLabel { color : green; font-weight:bold; }");
    m_data.m_ui->passedLabel->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->testProgress->setValue(m_data.m_ui->testProgress->maximum());
    m_data.m_ui->testButton->setEnabled(true);
    m_data.m_ui->pushButton->setEnabled(false);
    setStyleSheet("QLinTester { border:2px solid blue; }");
}

void QLinTester::failed() {
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { color : red; font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(true);
    m_data.m_ui->passedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->passedLabel->setEnabled(false);
    m_data.m_ui->testProgress->setValue(m_data.m_ui->testProgress->maximum());
    m_data.m_ui->testButton->setEnabled(true);
    m_data.m_ui->pushButton->setEnabled(false);
    setStyleSheet("QLinTester { border:2px solid red; }");
}

void QLinTester::inprogress() {
    m_data.m_ui->testButton->setEnabled(false);
    m_data.m_ui->pushButton->setEnabled(true);
    setStyleSheet("QLinTester { border:2px solid blue; }");
}

void QLinTester::initial() {
    m_data.m_ui->testProgress->setValue(0);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->passedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->passedLabel->setEnabled(false);
    m_data.m_ui->testButton->setEnabled(true);
    m_data.m_ui->pushButton->setEnabled(false);
    setStyleSheet("");
}