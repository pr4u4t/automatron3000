#include "QKonsole.h"
#include <QApplication>
#include <QTranslator>
#include "settingsdialog.h"

struct QKonsoleMenu {
    QKonsoleMenu(QCoreApplication* app) 
    : m_app(app){
        //m_serialMenu = new QMenu(m_app->translate("MainWindow", /*"&Serial"*/"DUPA"));

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

static bool QKonsole_register(Window* win, PluginsLoader* ld, QKonsoleMenu* ctx, Logger* log) {

	QObject::connect(ctx->m_console, &QAction::triggered, win, &Window::createOrActivate);
	QMenu* menu = win->findMenu(ctx->m_app->translate("MainWindow", "&Serial"));
    
    menu->addSeparator();
    menu->insertAction(nullptr, ctx->m_console);
    menu->insertAction(nullptr, ctx->m_actionConfigure);
    
    QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, win, ctx] {
        QSharedPointer<Plugin> konsole = ld->instance("QKonsole", win);
        SettingsDialog* dialog = new SettingsDialog(win, nullptr, konsole->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, konsole.dynamicCast<QKonsole>().data(), &QKonsole::settingsChanged);
        win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Konsole-Settings"));
    });

	return true;
}

static bool QKonsole_unregister(Window* win, PluginsLoader* ld, QKonsoleMenu* ctx, Logger* log) {
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
    {"QSerial"}
)

QKonsole::QKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
    : Widget(ld, plugins, parent, path)
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
    m_settings = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    m_terminal->setPrompt(m_settings.prompt); //setLocalEchoEnabled(m_settings.localEcho);
}