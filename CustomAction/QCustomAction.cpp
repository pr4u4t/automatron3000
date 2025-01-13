#include "QCustomAction.h"

#include "QCustomAction.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qcustomaction.h"

#include <QtGlobal>
#include <QtGui>
#include <QScrollBar>

struct QCustomActionMenu {
    QCustomActionMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QCustomAction", "_", "translations")) { //set directory of ts
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

static bool QCustomAction_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCustomActionMenu* ctx, Logger* log) {
    log->message("QCustomAction_register()");

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
    ctx->m_actionsMenu = actionsMenu->addMenu(ctx->m_app->translate("MainWindow", "CustomAction"));

    windowAddInstanceSettings(ctx->m_actionsMenu, &ctx->m_settings, &ctx->m_newInstance, "QCustomAction", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QCustomAction, QCustomActionMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QCustomAction"), gtx, ctx, log);
    });

    return true;
}

static bool QCustomAction_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCustomActionMenu* ctx, Logger* log) {
    log->message("QCustomAction_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QCustomAction,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QCustomAction_register,
    QCustomAction_unregister,
    QCustomActionMenu,
    {},
    true,
    100,
    CustomActionSettings
)

QCustomAction::QCustomAction(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath, CustomActionSettings* set, const QString& uuid)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        set,
        uuid
    )
    , m_ui(new Ui::QCustomActionUI) {
    m_ui->setupUi(this);
}

bool QCustomAction::initialize() {
    settingsChanged();
    QObject::connect(m_ui->pushButton, &QPushButton::clicked, this, &QCustomAction::execClicked);
    return true;
}

bool QCustomAction::deinitialize() {
    return true;
}

bool QCustomAction::saveSettings() {
    return true;
}

/*!
  \brief Destructor
  \param none
*/
QCustomAction::~QCustomAction() {
}

void QCustomAction::settingsChanged() {
    emit message("QCustomAction::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<CustomActionSettings>();
    *set = *(Settings::fetch<CustomActionSettings>(settingsPath()));
    //*set = CustomActionSettings(Settings::get(), settingsPath());

    m_ui->pushButton->setText(set->buttonText());
    m_ui->title->setText(set->title());
    m_ui->progressBar->setHidden(!set->progress());
    m_ui->textEdit->setHidden(!set->verbose());
    emit settingsApplied();
}

SettingsMdi* QCustomAction::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QCustomAction::settingsChanged);
    return ret;
}

void QCustomAction::execClicked(bool checked) {
    const auto set = settings<CustomActionSettings>();
    QStringList jobs = set->jobList();

    m_ui->progressBar->setMaximum(jobs.size());

    for (int i = 0, end = jobs.size(); i < end; i += 2) {
        auto plugin = plugins()->findByObjectName(jobs[i]);
        auto object = plugin.dynamicCast<QObject>();
        bool retVal = false;
        
        connect(object.data(), SIGNAL(message(const QString&, LoggerSeverity)), this, SLOT(jobMessage(const QString&, LoggerSeverity)));

        QMetaObject::invokeMethod(object.data(), jobs[i + 1].trimmed().toLocal8Bit().data(),
            Qt::DirectConnection, Q_RETURN_ARG(bool, retVal));
        
        disconnect(this, SLOT(jobMessage(const QString&, LoggerSeverity)));
        emit message(QString("QCustomAction::execClicked: %1::%2: status: %3").arg(jobs[i]).arg(jobs[i+1]).arg(retVal));
        m_ui->progressBar->setValue(i+2);
        jobMessage(QString("Progress.............%1").arg(m_ui->progressBar->text()));
        QCoreApplication::processEvents(QEventLoop::AllEvents, set->interval());
    }

    //m_ui->progressBar->setValue(m_ui->progressBar->maximum());
}

void QCustomAction::jobMessage(const QString& msg, LoggerSeverity severity) {
    QColor color = m_ui->textEdit->textColor();

    switch (severity) {
    case LoggerSeverity::LOG_INFO:
        m_ui->textEdit->setTextColor(QColor(56, 61, 65));
        break;

    case LoggerSeverity::LOG_NOTICE:
        m_ui->textEdit->setTextColor(QColor(12, 84, 96));
        break;

    case LoggerSeverity::LOG_WARNING:
        m_ui->textEdit->setTextColor(QColor(133, 100, 4));
        break;

    case LoggerSeverity::LOG_ERROR:
        m_ui->textEdit->setTextColor(QColor(114, 28, 36));
        break;

    case LoggerSeverity::LOG_DEBUG:
        break;
    }

    m_ui->textEdit->append(msg);
    m_ui->textEdit->setTextColor(color);
    m_ui->textEdit->verticalScrollBar()->setValue(m_ui->textEdit->verticalScrollBar()->maximum());
}

bool QCustomAction::reset(Reset type) {
    emit message("QCustomAction::reset(Reset type)");
    m_ui->progressBar->setValue(0);
    m_ui->textEdit->clear();
    return true;
}
