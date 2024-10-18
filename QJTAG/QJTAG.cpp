#include "QJTAG.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qjtag.h"

#include <QtGlobal>
#include <QtGui>

struct QJTAGMenu {
    QJTAGMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QJTAG", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_QJTAGMenu = new QMenu(m_app->translate("MainWindow", "JTAG"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        m_QJTAGMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_QJTAGMenu);
        m_newInstance->setData(QVariant("QJTAG"));
        m_QJTAGMenu->addAction(m_newInstance);
    }

    QMenu* m_QJTAGMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;
    
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QJTAG_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QJTAGMenu* ctx, Logger* log) {
    log->message("QJTAG_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_QJTAGMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin){
        if (plugin->name() != "QJTAG") {
            return;
        }
        
        QAction* settings = new QAction(dynamic_cast<const QJTAG*>(plugin)->objectName(), ctx->m_QJTAGMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QJTAG*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QJTAG*>(plugin), &QJTAG::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QJTAG*>(plugin)->objectName() + "/Settings"); //ctx->m_app->translate("MainWindow", "QJTAG-Settings"));
        });
    });

    return true;
}

static bool QJTAG_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QJTAGMenu* ctx, Logger* log) {
    log->message("QJTAG_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QJTAG,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QJTAG_register,
    QJTAG_unregister,
    QJTAGMenu,
    {},
    true,
    400
)

QJTAG::QJTAG(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath) 
    : Widget(
        ld, 
        plugins, 
        parent, 
        settingsPath, 
        new SettingsDialog::QJTAGSettings(Settings::get(), settingsPath)
    )
    , m_data(new Ui::QJTAGUI) {
    m_data.m_ui->setupUi(this);
}

QJTAG::~QJTAG() {
}

bool QJTAG::initialize() {
    settingsChanged();
    QObject::connect(m_data.m_ui->execButton, &QPushButton::clicked, this, &QJTAG::command);
    QObject::connect(&m_data.m_process, &QProcess::errorOccurred, this, &QJTAG::errorOccurred);
    QObject::connect(&m_data.m_process, &QProcess::finished, this, &QJTAG::finished);
    QObject::connect(&m_data.m_process, &QProcess::readyReadStandardError, this, &QJTAG::readyReadStandardError);
    QObject::connect(&m_data.m_process, &QProcess::readyReadStandardOutput, this, &QJTAG::readyReadStandardOutput);
    QObject::connect(&m_data.m_process, &QProcess::started, this, &QJTAG::started);
    QObject::connect(&m_data.m_process, &QProcess::stateChanged, this, &QJTAG::stateChanged);
    return true;
}

bool QJTAG::deinitialize() {
    return true;
}

bool QJTAG::saveSettings() {
    return true;
}

void QJTAG::settingsChanged() {
    emit message("QJTAG::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<SettingsDialog::QJTAGSettings>();
    *set = SettingsDialog::QJTAGSettings(Settings::get(), settingsPath());

    m_data.m_ui->execButton->setText(set->buttonLabel);
    m_data.m_ui->title->setText(set->title);

    m_data.m_arguments = set->processArguments();
    disconnect(this, SLOT(previousSuccess(const QByteArray&)));
    if (set->previous.isEmpty() == false) {
        connect(plugins()->findByObjectName(set->previous).dynamicCast<QObject>().data(), SIGNAL(success(const QByteArray&)), this, SLOT(previousSuccess(const QByteArray&)));
    }
}

void QJTAG::previousSuccess(const QByteArray& data) {
    command();
}

SettingsMdi* QJTAG::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QJTAG::settingsChanged);
    return ret;
}

void QJTAG::command(bool checked) {
    m_data.m_try = 0;
    exec();
}

QVariant QJTAG::exec() {
    
    const auto set = settings<SettingsDialog::QJTAGSettings>();
    if (set->programPath.isEmpty()) {
        emit message("QJTAG::exec: JTAG program path not set");
        failed();
        return QVariant();
    }

    if (QFile::exists(set->programPath) == false) {
        emit message("specified executable does not exist");
        failed();
        return QVariant();
    }

    m_data.m_ui->execButton->setEnabled(false);
    inprogress();
    m_data.m_process.start(set->programPath, set->processArguments());

    return QVariant();
}

void QJTAG::errorOccurred(QProcess::ProcessError error) {
    emit message("QJTAG::errorOccurred(QProcess::ProcessError error)");
    emit message("QJTAG::errorOccurred: "+errorString(error));

    const auto set = settings<SettingsDialog::QJTAGSettings>();

    if (error == QProcess::FailedToStart) {
        ++m_data.m_try;
        if (m_data.m_try < set->tries) {
            exec();
            return;
        }
        failed();
    }
}

void QJTAG::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    const auto set = settings<SettingsDialog::QJTAGSettings>();

    emit message("QJTAG::finished(int exitCode, QProcess::ExitStatus exitStatus)");
    emit message(QString("QJTAG::finished: %1 %2").arg(exitCode).arg(exitStatusString(exitStatus)));
    m_data.m_ui->execButton->setEnabled(true);

    switch (exitCode) {
    case 0:
        success();
        break;
    default:
        ++m_data.m_try;
        if (m_data.m_try < set->tries) {
            exec();
            return;
        }
        failed();
    }
}

void QJTAG::success() {
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(true);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ color:green; font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    setStyleSheet("QJTAG { border:2px solid green; }");
    emit success(objectName().toLocal8Bit());
}

void QJTAG::initial() {
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    setStyleSheet("");
}

void QJTAG::failed() {
    m_data.m_ui->failedLabel->setEnabled(true);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ color:red; font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    setStyleSheet("QJTAG { border:2px solid red; }");
}

void QJTAG::inprogress() {
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ color:blue; font-weight:bold; }");
    setStyleSheet("QJTAG { border:2px solid blue; }");
}

void QJTAG::readyReadStandardError() {
    emit message("QJTAG::readyReadStandardError()");
}

void QJTAG::readyReadStandardOutput() {
    emit message("QJTAG::readyReadStandardOutput()");
}

void QJTAG::started() {
    emit message("QJTAG::started()");
}

void QJTAG::stateChanged(QProcess::ProcessState newState) {
    emit message("QJTAG::stateChanged(QProcess::ProcessState newState)");
    emit message("QJTAG::stateChanged: "+stateString(newState));
}

QString QJTAG::exitStatusString(QProcess::ExitStatus exitStatus) const {
    switch (exitStatus) {
    case QProcess::NormalExit:
        return "Normal exit";
    case QProcess::CrashExit:
        return "Crash exit";
    }

    return QString("Unknown exit status %1").arg(exitStatus);
}

QString QJTAG::stateString(QProcess::ProcessState state) const {
    switch (state) {
    case QProcess::NotRunning:
        return "The process is not running.";
    case QProcess::Starting:
        return "The process is starting, but the program has not yet been invoked.";
    case QProcess::Running:
        return "The process is running and is ready for reading and writing.";
    }

    return QString("Unknown process state %1").arg(state);
}

QString QJTAG::errorString(QProcess::ProcessError err) const {
    switch (err) {
    case QProcess::FailedToStart:	
        return "The process failed to start.Either the invoked program is missing, or you may have insufficient permissions or resources to invoke the program.";
    case QProcess::Crashed: 
        return "The process crashed some time after starting successfully.";   
    case QProcess::Timedout: 
        return "The last waitFor...() function timed out.The state of QProcess is unchanged, and you can try calling waitFor...() again.";
    case QProcess::WriteError: 
        return "An error occurred when attempting to write to the process.For example, the process may not be running, or it may have closed its input channel.";
    case QProcess::ReadError: 
        return "An error occurred when attempting to read from the process.For example, the process may not be running.";
    case QProcess::UnknownError: 
        return "An unknown error occurred.This is the default return value of error().";
    }

    return QString("Unknown process error %1").arg(err);
}