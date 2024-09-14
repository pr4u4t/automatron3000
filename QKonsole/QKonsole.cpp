#include "QKonsole.h"
#include <QApplication>
#include <QTranslator>
#include "settingsdialog.h"

#include "../core/core.h"

struct QKonsoleMenu {
    QKonsoleMenu(QCoreApplication* app) 
    : m_app(app){
        m_actionConfigure = new QAction(m_app->translate("MainWindow", "Settings"), m_serialMenu);
        m_actionConfigure->setData(QVariant("QKonsole/Settings"));
        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QKonsole", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_console = new QAction(m_app->translate("MainWindow", "Konsole"), nullptr);
        m_console->setData(QVariant("QKonsole"));
    }

    QAction* m_console = nullptr;
    QMenu* m_serialMenu = nullptr;
    QAction* m_actionConfigure = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QKonsole_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QKonsoleMenu* ctx, Logger* log) {

    log->message("PluginList_register");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

	QObject::connect(ctx->m_console, &QAction::triggered, gtx->m_win, &Window::createOrActivate);
	QMenu* menu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Serial"));
    
    menu->addSeparator();
    menu->insertAction(nullptr, ctx->m_console);
    menu->insertAction(nullptr, ctx->m_actionConfigure);
    
    QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, gtx, ctx] {
        QSharedPointer<Plugin> konsole = ld->instance("QKonsole", gtx->m_win);
        if (gtx->m_win->toggleWindow(dynamic_cast<const QKonsole*>(konsole.data())->objectName() + "/Settings")) {
            return;
        }
        SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, konsole->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, konsole.dynamicCast<QKonsole>().data(), &QKonsole::settingsChanged);
        gtx->m_win->addSubWindow(dialog, dynamic_cast<const QKonsole*>(konsole.data())->objectName() + "/Settings");//ctx->m_app->translate("MainWindow", "Konsole-Settings"));
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
    1200
)

QKonsole::QKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
    : Widget(ld, plugins, parent, path, new SettingsDialog::KonsoleSettings(Settings::get(), path))
    , m_terminal(new QTerminal(this, tr("<b>Welcome to serial (rs-232) terminal</b>"))) {
    settingsChanged();
    //m_settings = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    //m_terminal->setLocalEchoEnabled(m_settings.localEcho);
    QBoxLayout* l = new QVBoxLayout();
    l->addWidget(m_terminal);
    setLayout(l);

    QObject::connect(m_terminal, &QTerminal::execCommand, this, &QKonsole::enterPressed);
    auto io = plugins->instance("QSerial", nullptr);
    QObject::connect(dynamic_cast<IODevice*>(io.data()), &IODevice::dataReady, this, &QKonsole::putData);
    m_serial = io.dynamicCast<IODevice>();
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
    *(settings<SettingsDialog::KonsoleSettings>()) = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    m_terminal->setPrompt(settings<SettingsDialog::KonsoleSettings>()->prompt); //setLocalEchoEnabled(m_settings.localEcho);
}