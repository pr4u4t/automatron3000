#include "QLinKonsole.h"
#include <QApplication>
#include <QTranslator>
#include <QTimer>
#include <QThread>

#include "settingsdialog.h"

#include "../core/core.h"

struct QLinKonsoleMenu {
    QLinKonsoleMenu(QCoreApplication* app) 
    : m_app(app){
        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinKonsole", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QAction* m_newInstance = nullptr;
    QMenu* m_settings = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinKonsole_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinKonsoleMenu* ctx, Logger* log) {
    log->message("QLinKonsole_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QLinKonsole_register(): application is non gui not registering");
        return false;
    }

    auto linbusMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));

    if (linbusMenu == nullptr) {
        log->message("QLinKonsole_register(): linbus menu not found");
        return false;
    }

    windowAddInstanceSettings(linbusMenu, &ctx->m_settings, &ctx->m_newInstance, "QLinKonsole", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QLinKonsole, QLinKonsoleMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QLinKonsole"), gtx, ctx, log);
    });

    return true;
}

static bool QLinKonsole_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinKonsoleMenu* ctx, Logger* log) {
    log->message("QLinKonsole_unregister()");
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
    false,
    800,
    KonsoleSettings
)

QLinKonsole::QLinKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path, KonsoleSettings* set, const QString& uuid)
    : Widget(ld, plugins, parent, path, set, uuid)
    , m_data(new QTerminal(this, tr("<b>Welcome to LIN terminal</b>"))) {
    //settingsChanged();
    //m_settings = SettingsDialog::KonsoleSettings(Settings::get(), settingsPath());
    //m_terminal->setLocalEchoEnabled(m_settings.localEcho);
    QBoxLayout* l = new QVBoxLayout();
    l->addWidget(m_data.m_terminal);
    setLayout(l);

    QObject::connect(m_data.m_terminal, &QTerminal::execCommand, this, &QLinKonsole::enterPressed);
    //QTimer::singleShot(0, this, &QLinKonsole::init);
}

SettingsMdi* QLinKonsole::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinKonsole::settingsChanged);
    return ret;
}

bool QLinKonsole::initialize() {

    emit message("QLinKonsole::init()");
    const auto set = settings<KonsoleSettings>();
    *(set) = KonsoleSettings(Settings::get(), settingsPath());

    if (set->linDevice().isEmpty() == true) {
        emit message("QLinKonsole::init: lin device name == nullptr");
        return false;
    }

    auto io = plugins()->instance(set->linDevice(), nullptr);

    if (io.isNull() == true) {
        emit message(QString("QLinKonsole::init: failed to open lin device %1").arg(set->linDevice()));
    }

    connect(dynamic_cast<IODevice*>(io.data()), SIGNAL(dataReady(const QByteArray&)), this, SLOT(putData(const QByteArray&)));
    connect(dynamic_cast<IODevice*>(io.data()), SIGNAL(message(const QString&, LoggerSeverity)), this, SLOT(putData(const QString&, LoggerSeverity)));
    m_data.m_lin = io.dynamicCast<IODevice>();

    if (m_data.m_lin->isOpen() == false) {
        if (m_data.m_lin->open() == false) {
            emit message("QLinKonsole::init(): failed to open QLin");
            return false;
        }
    }

    return true;
}

bool QLinKonsole::deinitialize() {
    return true;
}

void QLinKonsole::settingsChanged() {
    emit message("QLinKonsole::settingsChanged()");
    const auto set = settings<KonsoleSettings>();
    //*(set) = KonsoleSettings(Settings::get(), settingsPath());
    *set = *(Settings::fetch<KonsoleSettings>(settingsPath()));
    m_data.m_terminal->setPrompt(set->prompt()); //setLocalEchoEnabled(m_settings.localEcho);
    disconnect(this, SLOT(putData(const QByteArray&)));
    disconnect(this, SLOT(putData(const QString&, LoggerSeverity)));
    m_data.m_lin = nullptr;

    if (set->linDevice().isEmpty() == true) {
        emit message("QLinKonsole::init: lin device name == nullptr");
        return;
    }

    auto io = plugins()->instance(set->linDevice(), nullptr);

    if (io.isNull() == true) {
        emit message(QString("QLinKonsole::init: failed to open lin device %1").arg(set->linDevice()));
        return;
    }

    connect(dynamic_cast<IODevice*>(io.data()), SIGNAL(dataReady(const QByteArray&)), this, SLOT(putData(const QByteArray&)));
    connect(dynamic_cast<IODevice*>(io.data()), SIGNAL(message(const QString&, LoggerSeverity)), this, SLOT(putData(const QString&, LoggerSeverity)));
    m_data.m_lin = io.dynamicCast<IODevice>();

    emit settingsApplied();

    if (m_data.m_lin->isOpen() == false) {
        if (m_data.m_lin->open() == false) {
            emit message("QLinKonsole::init(): failed to open QLin");
            return;
        }
    }
}

void QLinKonsole::putData(const QByteArray& data, LoggerSeverity severity) {
    emit message("QLinKonsole::putData()");
   
    m_data.m_terminal->printCommandExecutionResults(data);
}

void QLinKonsole::putData(const QString& data, LoggerSeverity severity) {
    if(severity == LoggerSeverity::LOG_ERROR){ 
        putData(data.toLocal8Bit(), severity);
    }
}

void QLinKonsole::enterPressed(const QString& command) {
    emit message("QLinKonsole::enterPressed()");

    if (m_data.m_lin->isOpen() != true) {
        emit message("QLinKonsole::enterPressed(): LIN not open", LoggerSeverity::LOG_ERROR);
        if (m_data.m_lin->open() != true) {
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
        QThread::msleep(settings<KonsoleSettings>()->commandDelay());
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
            //id:60 data: 101 3 34 241 140
            data = QByteArray(1 + (comm[1].size() - 2) / 2, ' ');

            for (int i = 2; i < comm[1].size(); i += 2) {
                QString tmp = comm[1].mid(i, 2);
                value = tmp.toUInt(&ok, 16);
                data[1 + i / 2-1] = value;
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

    m_data.m_lin->write(data);
    return;
FAIL:
    putData(("Invalid input: " + command).toLocal8Bit());
}

