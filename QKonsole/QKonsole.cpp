#include "QKonsole.h"
#include <QApplication>
#include <QTranslator>
#include "settingsdialog.h"

#include "../core/core.h"

struct QKonsoleMenu {
    QKonsoleMenu(QCoreApplication* app) 
    : m_app(app){
        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QKonsole", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QAction* m_newInstance = nullptr;
    QMenu* m_settings = nullptr;
    QMenu* m_konsoleMenu = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QKonsole_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QKonsoleMenu* ctx, Logger* log) {
    log->message("QKonsole_register");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QKonsole_register(): application is non gui not registering");
        return false;
    }

    if (gtx->m_win == nullptr) {
        log->message("QKonsole_register(): window pointer == nullptr");
        return false;
    }

    auto ioMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Serial"));

    if (ioMenu == nullptr) {
        log->message("QKonsole_register(): serial menu not found");
        return false;
    }

    //if ((ctx->m_konsoleMenu = ioMenu->addMenu(ctx->m_app->translate("MainWindow", "&Konsole"))) == nullptr) {
    //    log->message("QKonsole_register(): failed to create konsole menu");
    //    return false;
    //}

    windowAddInstanceSettings(ioMenu, &ctx->m_settings, &ctx->m_newInstance, "QKonsole", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QKonsole, QKonsoleMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLin"), gtx, ctx, log);
    });

    return true;
}

static bool QKonsole_unregister(ModuleLoaderContext* win, PluginsLoader* ld, QKonsoleMenu* ctx, Logger* log) {
    return true;
}

REGISTER_PLUGIN(
	QKonsole, 
	Plugin::Type::WIDGET, 
	"0.0.1", 
	"pawel.ciejka@gmail.com", 
	"example plugin",
	QKonsole_register,
	QKonsole_unregister,
    QKonsoleMenu,
    {"QSerial"},
    true,
    1200,
    KonsoleSettings
)

QKonsole::QKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path, KonsoleSettings* set, const QString& uuid)
    : Widget(
        ld, 
        plugins, 
        parent, 
        path, 
        set,
        uuid
    )
    , m_terminal(new QTerminal(this, tr("<b>Welcome to serial (rs-232) terminal</b>"))) {
    //m_settings = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    //m_terminal->setLocalEchoEnabled(m_settings.localEcho);
    QBoxLayout* l = new QVBoxLayout();
    l->addWidget(m_terminal);
    setLayout(l);
}

bool QKonsole::initialize() {
    settingsChanged();
    QObject::connect(m_terminal, &QTerminal::execCommand, this, &QKonsole::enterPressed);
    auto io = plugins()->instance("QSerial", nullptr);
    QObject::connect(dynamic_cast<IODevice*>(io.data()), &IODevice::dataReady, this, &QKonsole::putData);
    m_serial = io.dynamicCast<IODevice>();
    return true;
}

bool QKonsole::deinitialize() {
    return true;
}

SettingsMdi* QKonsole::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QKonsole::settingsChanged);
    return ret;
}

void QKonsole::putData(const QByteArray& data) {
    emit message("QKonsole::putData()");
   
    m_terminal->printCommandExecutionResults(data);
}

void QKonsole::enterPressed(const QString& command) {
    emit message("QKonsole::enterPressed()");

    if (m_serial->isOpen() != true) {
        m_serial->open();
    }
    m_serial->write(command + "\n");
}

void QKonsole::settingsChanged() {
    const auto set = settings<KonsoleSettings>();
    //*(settings<KonsoleSettings>()) = KonsoleSettings(Settings::get(), settingsPath());
    *set = *(Settings::fetch<KonsoleSettings>(settingsPath()));
    m_terminal->setPrompt(set->prompt()); //setLocalEchoEnabled(m_settings.localEcho);
    emit settingsApplied();
}