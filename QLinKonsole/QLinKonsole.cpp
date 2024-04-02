#include "QLinKonsole.h"
#include <QApplication>
#include <QTranslator>
#include "settingsdialog.h"

#include "../core/core.h"

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

static bool QLinKonsole_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinKonsoleMenu* ctx, Logger* log) {

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

	QObject::connect(ctx->m_console, &QAction::triggered, gtx->m_win, &Window::createOrActivate);
	QMenu* menu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    
    menu->addSeparator();
    menu->insertAction(nullptr, ctx->m_console);
    menu->insertAction(nullptr, ctx->m_actionConfigure);
    
    QObject::connect(ctx->m_actionConfigure, &QAction::triggered, [ld, gtx, ctx] {
        QSharedPointer<Plugin> konsole = ld->instance("QLinKonsole", gtx->m_win);
        SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, konsole->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, konsole.dynamicCast<QLinKonsole>().data(), &QLinKonsole::settingsChanged);
        gtx->m_win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "LinKonsole/Settings"));
    });

	return true;
}

static bool QLinKonsole_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinKonsoleMenu* ctx, Logger* log) {
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
    {"QLin"},
    false
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

    QStringList commands = command.split(',');
    QRegularExpression rx("[\\s]+");
    for (auto it = commands.begin(), end = commands.end(); it != end; ++it) {
        processCommand((*it).trimmed().replace(rx, " "));
    }
}

void QLinKonsole::processCommand(const QString& command) {
    QStringList comm = command.split(' ');
    QByteArray data(1,0);
    bool ok = false;
    quint32 value = 0;

    if (comm.size() > 2 || comm[0].startsWith("0x") != true || (comm.size() == 2 && comm[1].startsWith("0x") != true)) {
        goto FAIL;
    }

    switch (comm.size()) {
        case 2:
            if (comm[1].size() % 2 != 0 || comm[1].size() > 18) {
                goto FAIL;
            }

            data = QByteArray(2 + (comm[1].size() - 2) / 2, ' ');

            for (int i = 2; i < comm[1].size(); i += 2) {
                QString tmp = comm[1].mid(i, 2);
                value = tmp.toUInt(&ok, 16);
                data[2 + i / 2-1] = value;
            }

        case 1:
            value = comm[0].toUInt(&ok, 16);
            if (ok == false || value > 63) {
                goto FAIL;
            }
            data[0] = value;

            break;

        default:
            goto FAIL;
            return;
    }

    m_lin->write(data);
    return;
FAIL:
    putData(("Invalid input: " + command).toLocal8Bit());
}

void QLinKonsole::settingsChanged() {
    emit message("QLinKonsole::settingsChanged()");
    m_settings = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    m_terminal->setPrompt(m_settings.prompt); //setLocalEchoEnabled(m_settings.localEcho);
}