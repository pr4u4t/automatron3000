#include "QExternalAction.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qexternalaction.h"

#include <QtGlobal>
#include <QtGui>

struct QExternalActionMenu {
    QExternalActionMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QExternalAction", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_actionsMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;
    
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QExternalAction_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QExternalActionMenu* ctx, Logger* log) {
    log->message("QExternalAction_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QCustomAction_register(): application is non gui not registering");
        return false;
    }

    if (gtx->m_win == nullptr) {
        log->message("QCustomAction_register(): window pointer == nullptr");
        return false;
    }

    auto actionsMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "Actions"));
    ctx->m_actionsMenu = actionsMenu->addMenu(ctx->m_app->translate("MainWindow", "ExternalAction"));

    windowAddInstanceSettings(ctx->m_actionsMenu, &ctx->m_settings, &ctx->m_newInstance, "QExternalAction", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QExternalAction, QExternalActionMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QExternalAction"), gtx, ctx, log);
    });

    return true;
}

static bool QExternalAction_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QExternalActionMenu* ctx, Logger* log) {
    log->message("QExternalAction_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QExternalAction,
    Plugin::Type::WIDGET,
    "0.0.2",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QExternalAction_register,
    QExternalAction_unregister,
    QExternalActionMenu,
    {},
    true,
    400,
    QExternalActionSettings
)

QExternalAction::QExternalAction(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, QExternalActionSettings* set, const QString& uuid)
    : Widget(
        ld, 
        plugins, 
        parent, 
        settingsPath, 
        set,
        uuid
    )
    , m_data(new Ui::QExternalActionUI) {
    m_data.m_ui->setupUi(this);
    QObject::connect(m_data.m_ui->execButton, &QPushButton::clicked, this, &QExternalAction::command);
    QObject::connect(&m_data.m_process, &QProcess::errorOccurred, this, &QExternalAction::errorOccurred);
    QObject::connect(&m_data.m_process, &QProcess::finished, this, &QExternalAction::finished);
    QObject::connect(&m_data.m_process, &QProcess::readyReadStandardError, this, &QExternalAction::readyReadStandardError);
    QObject::connect(&m_data.m_process, &QProcess::readyReadStandardOutput, this, &QExternalAction::readyReadStandardOutput);
    QObject::connect(&m_data.m_process, &QProcess::started, this, &QExternalAction::started);
    QObject::connect(&m_data.m_process, &QProcess::stateChanged, this, &QExternalAction::stateChanged);
}

QExternalAction::~QExternalAction() {
}

bool QExternalAction::initialize() {
    const auto set = settings<QExternalActionSettings>();
    m_data.m_ui->execButton->setText(set->buttonLabel());
    m_data.m_ui->title->setText(set->title());

    m_data.m_arguments = set->processArguments();
    disconnect(this, SLOT(previousSuccess(const QByteArray&)));
    if (set->previous().isEmpty() == false) {
        connect(plugins()->findByObjectName(set->previous()).dynamicCast<QObject>().data(), SIGNAL(success(const QByteArray&)), this, SLOT(previousSuccess(const QByteArray&)));
    }
    
    return true;
}

bool QExternalAction::deinitialize() {
    return true;
}

bool QExternalAction::saveSettings() {
    return true;
}

void QExternalAction::settingsChanged() {
    emit message("QExternalAction::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<QExternalActionSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<QExternalActionSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

void QExternalAction::previousSuccess(const QByteArray& data) {
    command();
}

SettingsMdi* QExternalAction::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QExternalAction::settingsChanged);
    return ret;
}

void QExternalAction::command(bool checked) {
    m_data.m_try = 0;
    execute();
}

QVariant QExternalAction::execute() {
    
    const auto set = settings<QExternalActionSettings>();
    if (set->programPath().isEmpty()) {
        emit message("QExternalAction::exec: JTAG program path not set");
        failed();
        return QVariant();
    }

    if (QFile::exists(set->programPath()) == false) {
        emit message("specified executable does not exist");
        failed();
        return QVariant();
    }

    m_data.m_ui->execButton->setEnabled(false);
    inprogress();
    m_data.m_process.start(set->programPath(), set->processArguments());
    
    return QVariant();
}

void QExternalAction::errorOccurred(QProcess::ProcessError error) {
    emit message("QExternalAction::errorOccurred(QProcess::ProcessError error)");
    emit message("QExternalAction::errorOccurred: "+errorString(error));

    const auto set = settings<QExternalActionSettings>();

    emit message(QString("QExternalAction::errorOccurred: try %1 of %2").arg(m_data.m_try).arg(set->tries()));

    if (error == QProcess::FailedToStart) {
        ++m_data.m_try;
        if (m_data.m_try < set->tries()) {
            emit message("QExternalAction::errorOccurred: attempting next try");
            execute();
            return;
        }
        emit message("QExternalAction::errorOccurred: enetring failed state");
        failed();
    }
}

void QExternalAction::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    const auto set = settings<QExternalActionSettings>();

    emit message("QExternalAction::finished(int exitCode, QProcess::ExitStatus exitStatus)");
    emit message(QString("QExternalAction::finished: %1 %2").arg(exitCode).arg(exitStatusString(exitStatus)));
    m_data.m_ui->execButton->setEnabled(true);
    emit message(QString("QExternalAction::finished: try %1 of %2").arg(m_data.m_try).arg(set->tries()));

    switch (exitCode) {
    case 0:
        success();
        break;
    default:
        ++m_data.m_try;
        if (m_data.m_try < set->tries()) {
            emit message("QExternalAction::finished: attempting next try");
            execute();
            return;
        }
        emit message("QExternalAction::finished: enetring failed state");
        failed();
        
    }
}

void QExternalAction::success() {
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(true);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ color:green; font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    setStyleSheet("QExternalAction { border:2px solid green; }");
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
    emit success(objectName().toLocal8Bit());
}

void QExternalAction::initial() {
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel { font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel { font-weight:bold; }");
    setStyleSheet("");
    m_data.m_state = QExternalActionState::INITIAL;
}

void QExternalAction::failed() {
    m_data.m_ui->failedLabel->setEnabled(true);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(false);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ color:red; font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    setStyleSheet("QExternalAction { border:2px solid red; }");
    m_data.m_state = QExternalActionState::FAILED;
    if (m_data.m_loop != nullptr) {
        m_data.m_loop->exit();
    }
}

void QExternalAction::inprogress() {
    m_data.m_ui->failedLabel->setEnabled(false);
    m_data.m_ui->successLabel->setEnabled(false);
    m_data.m_ui->progressLabel->setEnabled(true);
    m_data.m_ui->failedLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->successLabel->setStyleSheet("QLabel{ font-weight:bold; }");
    m_data.m_ui->progressLabel->setStyleSheet("QLabel{ color:blue; font-weight:bold; }");
    setStyleSheet("QExternalAction { border:2px solid blue; }");
    m_data.m_state = QExternalActionState::INPROGRESS;
}

void QExternalAction::readyReadStandardError() {
    emit message("QExternalAction::readyReadStandardError()");
}

void QExternalAction::readyReadStandardOutput() {
    emit message("QExternalAction::readyReadStandardOutput()");
}

void QExternalAction::started() {
    emit message("QExternalAction::started()");
}

void QExternalAction::stateChanged(QProcess::ProcessState newState) {
    emit message("QExternalAction::stateChanged(QProcess::ProcessState newState)");
    emit message("QExternalAction::stateChanged: "+stateString(newState));
}

QString QExternalAction::exitStatusString(QProcess::ExitStatus exitStatus) const {
    switch (exitStatus) {
    case QProcess::NormalExit:
        return "Normal exit";
    case QProcess::CrashExit:
        return "Crash exit";
    }

    return QString("Unknown exit status %1").arg(exitStatus);
}

QString QExternalAction::stateString(QProcess::ProcessState state) const {
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

QString QExternalAction::errorString(QProcess::ProcessError err) const {
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