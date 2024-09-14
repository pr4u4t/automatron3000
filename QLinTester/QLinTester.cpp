#include "QLinTester.h"
#include <QTranslator>
#include <QMessageBox>

#include "../core/core.h"
#include "settingsdialog.h"

struct QLinTesterMenu {
    QLinTesterMenu(QCoreApplication* app)
        : m_app(app) {

        m_actionConfigure = new QAction(m_app->translate("MainWindow", "Settings"));
        m_actionConfigure->setData(QVariant("QLinTester/Settings"));
        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinTester", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_tester = new QAction(m_app->translate("MainWindow", "LinTester"), nullptr);
        m_tester->setData(QVariant("QLinTester"));
    }

    QAction* m_tester = nullptr;
    QAction* m_actionConfigure = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinTester_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinTesterMenu* ctx, Logger* log) {
    log->message("QLinTester_register()");
    
    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    QObject::connect(ctx->m_tester, &QAction::triggered, gtx->m_win, &Window::createOrActivate);
    QMenu* menu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));

    menu->addSeparator();
    menu->insertAction(nullptr, ctx->m_tester);
    menu->insertAction(nullptr, ctx->m_actionConfigure);

    QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, gtx, ctx] {
        QSharedPointer<Plugin> tester = ld->instance("QLinTester", gtx->m_win);

        if (gtx->m_win->toggleWindow(dynamic_cast<const QLinTester*>(tester.data())->objectName() + "/Settings")) {
            return;
        }

        SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, tester->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, tester.dynamicCast<QLinTester>().data(), &QLinTester::settingsChanged);
        gtx->m_win->addSubWindow(dialog, dynamic_cast<const QLinTester*>(tester.data())->objectName() + "/Settings"); //ctx->m_app->translate("MainWindow", "QLinTester/Settings"));
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
    false,
    1000
)

QLinTester::QLinTester(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
	: Widget(ld, plugins, parent, settingsPath, new SettingsDialog::LinTesterSettings(Settings::get(), settingsPath))
    , m_ui(new Ui::QLinTesterUI) {
    settingsChanged();
    m_ui->setupUi(this);
    connect(m_ui->testButton, &QPushButton::clicked, this, &QLinTester::startTest);
}

void QLinTester::settingsChanged() {
    emit message("QLinTester::settingsChanged()");
    *(settings<SettingsDialog::LinTesterSettings>()) = SettingsDialog::LinTesterSettings(Settings::get(), settingsPath());
}

SettingsMdi* QLinTester::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinTester::settingsChanged);
    return ret;
}

void QLinTester::dataReady(const QByteArray& data) {
    emit message("QLinTester::dataReady(): " + data, LoggerSeverity::LOG_DEBUG);

    if (data.startsWith("LIN NOANS")) {
        emit message("QLinTester::dataReady(): LIN NOANS", LoggerSeverity::LOG_DEBUG);
        return;
    }

    if (data.startsWith("ID")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }
        
        //
    }
}

void QLinTester::startTest() {
    emit message("QLinTester::startScan()", LoggerSeverity::LOG_DEBUG);

//TODO:
    m_ui->passedLabel->setEnabled(true);
    m_ui->passedLabel->setStyleSheet("QLabel { color : green; }");
    return;

    if (m_lin.isNull() || m_lin->isOpen() == false) {
        emit message("QLinTester::startTest(): failed LIN device not open");
        QMessageBox::critical(
            this,
            tr("QLinBus: error"),
            tr("LIN device not open"),
            QMessageBox::StandardButton::Ok
        );

        return;
    }


    switch (m_state) {
    case QLinTesterState::STOP:
    case QLinTesterState::INITIAL:
        m_responses = 0;
        break;
    case QLinTesterState::PAUSE:
        break;
    }
}

void QLinTester::testStep() {
    emit message("QLinTester::testStep()");

    if (m_test <= settings<SettingsDialog::LinTesterSettings>()->scanStopID) {
        QByteArray data(1, static_cast<char>(m_test));
        m_lin->write(data);

        m_ui->testProgress->setValue(m_test);
        ++m_test;
    } else {
        m_state = QLinTesterState::STOP;
        m_test = settings<SettingsDialog::LinTesterSettings>()->scanStartID;
        m_timer.stop();
    }
}

void QLinTester::testStop() {
    emit message("QLinTester::scanStop()", LoggerSeverity::LOG_DEBUG);
    m_state = QLinTesterState::STOP;
    m_test = settings<SettingsDialog::LinTesterSettings>()->scanStartID;
    m_timer.stop();
    //m_ui->startButton->setEnabled(true);
    //m_ui->stopButton->setEnabled(false);
    //m_ui->pauseButton->setEnabled(false);
}

void QLinTester::init() {
    emit message("QLinTester::init()", LoggerSeverity::LOG_DEBUG);
    auto plugin = plugins()->instance("QLin", 0);
    auto lin = plugin.dynamicCast<IODevice>();
    m_lin = lin;
    connect(lin.data(), &IODevice::dataReady, this, &QLinTester::dataReady);
    if (lin->isOpen() == false) {
        lin->open();
    }
}
