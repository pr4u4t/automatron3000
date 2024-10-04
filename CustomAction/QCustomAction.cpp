#include "QCustomAction.h"

#include "QCustomAction.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qcustomaction.h"

#include <QtGlobal>
#include <QtGui>

struct QCustomActionMenu {
    QCustomActionMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QCustomAction", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_actionsMenu = new QMenu(m_app->translate("MainWindow", "Actions"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        m_actionsMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_actionsMenu);
        m_newInstance->setData(QVariant("QCustomAction"));
        m_actionsMenu->addAction(m_newInstance);
    }

    QMenu* m_actionsMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QCustomAction_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCustomActionMenu* ctx, Logger* log) {
    log->message("QBadge_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_actionsMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QCustomAction") {
            return;
        }

        QAction* settings = new QAction(dynamic_cast<const QObject*>(plugin)->objectName(), ctx->m_actionsMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QCustomAction*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QCustomAction*>(plugin), &QCustomAction::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QCustomAction*>(plugin)->objectName() + "/Settings"); // ctx->m_app->translate("MainWindow", dynamic_cast<const QBadge*>(plugin)->objectName() + "/Settings"));
            });
        });

    return true;
}

static bool QCustomAction_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QCustomActionMenu* ctx, Logger* log) {
    log->message("QBadge_unregister()");
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
    100
)

QCustomAction::QCustomAction(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        new SettingsDialog::CustomActionSettings(Settings::get(), sPath)
    )
    , m_ui(new Ui::QCustomActionUI) {
    m_ui->setupUi(this);
    settingsChanged();
    QObject::connect(m_ui->pushButton, &QPushButton::clicked, this, &QCustomAction::execClicked);
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
    emit message("QBadge::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<SettingsDialog::CustomActionSettings>();
    *set = SettingsDialog::CustomActionSettings(Settings::get(), settingsPath());

    m_ui->pushButton->setText(set->buttonText);
    m_ui->title->setText(set->title);
}

SettingsMdi* QCustomAction::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QCustomAction::settingsChanged);
    return ret;
}

void QCustomAction::execClicked(bool checked) {
    const auto set = settings<SettingsDialog::CustomActionSettings>();
    QStringList jobs = set->jobList();

    for (int i = 0, end = jobs.size(); i < end; i += 2) {
        auto plugin = plugins()->findByObjectName(jobs[i]);
        auto object = plugin.dynamicCast<QObject>();
        bool retVal = false;
        
        QMetaObject::invokeMethod(object.data(), jobs[i + 1].trimmed().toLocal8Bit().data(),
            Qt::DirectConnection, Q_RETURN_ARG(bool, retVal));
        
        emit message(QString("QCustomAction::execClicked: %1::%2: status: %3").arg(jobs[i]).arg(jobs[i+1]).arg(retVal));
    }
}
