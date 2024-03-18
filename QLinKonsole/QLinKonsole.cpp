#include "QLinKonsole.h"
#include <QApplication>
#include <QTranslator>
#include "settingsdialog.h"

struct QLinKonsoleMenu {
    QLinKonsoleMenu(QCoreApplication* app) 
    : m_app(app){
        //m_serialMenu = new QMenu(m_app->translate("MainWindow", /*"&Serial"*/"DUPA"));

        m_actionConfigure = new QAction(m_app->translate("MainWindow", "Settings"), m_serialMenu);
        m_actionConfigure->setData(QVariant("QLinKonsole/Settings"));
        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinKonsole", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_console = new QAction(m_app->translate("MainWindow", "LinKonsole"), nullptr);
        m_console->setData(QVariant("QLinKonsole"));
    }

    QAction* m_console = nullptr;
    QMenu* m_serialMenu = nullptr;
    QAction* m_actionConfigure = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinKonsole_register(Window* win, PluginsLoader* ld, QLinKonsoleMenu* ctx, Logger* log) {

	QObject::connect(ctx->m_console, &QAction::triggered, win, &Window::createOrActivate);
	QMenu* menu = win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    
    menu->addSeparator();
    menu->insertAction(nullptr, ctx->m_console);
    menu->insertAction(nullptr, ctx->m_actionConfigure);
    
    QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, win, ctx] {
        QSharedPointer<Plugin> konsole = ld->instance("QLinKonsole", win);
        SettingsDialog* dialog = new SettingsDialog(win, nullptr, konsole->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, konsole.dynamicCast<QLinKonsole>().data(), &QLinKonsole::settingsChanged);
        win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "LinKonsole/Settings"));
    });

	return true;
}

static bool QLinKonsole_unregister(Window* win, PluginsLoader* ld, QLinKonsoleMenu* ctx, Logger* log) {
    return true;
}

REGISTER_PLUGIN(
	QLinKonsole, 
	Plugin::Type::WIDGET, 
	"0.0.1", 
	"pawel.ciejka@gmail.com", 
	"example plugin",
	QLinKonsole_register,
	QLinKonsole_unregister,
    QLinKonsoleMenu,
    {"QLin"}
)

QLinKonsole::QLinKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
    : Widget(ld, plugins, parent, path)
    , m_terminal(new QTerminal(this, tr("<b>Welcome to LIN terminal</b>"))) {
    settingsChanged();
    //m_settings = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    //m_terminal->setLocalEchoEnabled(m_settings.localEcho);
    QBoxLayout* l = new QVBoxLayout();
    l->addWidget(m_terminal);
    setLayout(l);

    QObject::connect(m_terminal, &QTerminal::execCommand, this, &QLinKonsole::enterPressed);
    auto io = plugins->instance("QLin", nullptr);
    QObject::connect(dynamic_cast<IODevice*>(io.data()), &IODevice::dataReady, this, &QLinKonsole::putData);
    m_lin = io.dynamicCast<IODevice>();
}

void QLinKonsole::putData(const QByteArray& data) {
    emit message("QLinKonsole::putData()");
   
    m_terminal->printCommandExecutionResults(data);
}

void QLinKonsole::enterPressed(const QString& command) {
    emit message("QLinKonsole::enterPressed()");

    if (m_lin->isOpen() != true) {
        emit message("QLinKonsole::enterPressed(): LIN not open");
        if (m_lin->open() != true) {
            emit message("QLinKonsole::enterPressed(): failed to open LIN");
            return;
        } else {
            emit message("QLinKonsole::enterPressed(): LIN open success");
        }
    }
    
    
    //m_serial->write(command + "\n");
}

void QLinKonsole::settingsChanged() {
    emit message("QLinKonsole::settingsChanged()");
    m_settings = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    m_terminal->setPrompt(m_settings.prompt); //setLocalEchoEnabled(m_settings.localEcho);
}