#include "QMLVisual.h"
#include "../core/core.h"
#include "settingsdialog.h"

#include <QtGlobal>
#include <QtGui>
#include <QVBoxLayout>

struct QMLVisualMenu {
    QMLVisualMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QMLVisual", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_qmlvisualMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QMLVisual_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QMLVisualMenu* ctx, Logger* log) {
    log->message("QMLVisual_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QMLVisual_register(): application is non gui not registering");
        return false;
    }

    auto visualMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));

    if (visualMenu == nullptr) {
        log->message("QMLVisual_register(): visualizations menu not found");
        return false;
    }

    if ((ctx->m_qmlvisualMenu = visualMenu->addMenu(ctx->m_app->translate("MainWindow", "QML"))) == nullptr) {
        log->message("QMLVisual_register(): failed to create qml menu");
        return false;
    }

    windowAddInstanceSettings(ctx->m_qmlvisualMenu, &ctx->m_settings, &ctx->m_newInstance, "QMLVisual", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QMLVisual, QMLVisualMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QMLVisual"), gtx, ctx, log);
    });

    return true;
}

static bool QMLVisual_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QMLVisualMenu* ctx, Logger* log) {
    log->message("QMLViewer_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QMLVisual,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QMLVisual_register,
    QMLVisual_unregister,
    QMLVisualMenu,
    {},
    true,
    100,
    MLVisualSettings
)

QMLVisual::QMLVisual(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath, MLVisualSettings* set, const QString& uuid)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        set,
        uuid
    ),
    m_viewer(new QQuickWidget(this)){
    setLayout(new QVBoxLayout());
    layout()->addWidget(m_viewer);
}

bool QMLVisual::saveSettings() {
    return true;
}

/*!
  \brief Destructor
  \param none
*/
QMLVisual::~QMLVisual() {
}

//void QBadge::resizeEvent(QResizeEvent* event) {
//    
//    m_pixmap = m_pixmap.scaled(event->size(), Qt::KeepAspectRatio);
//    m_ui->image->setPixmap(m_pixmap);
//
//    Widget::resizeEvent(event);
//}

bool QMLVisual::initialize() {
    settingsChanged();
    return true;
}

bool QMLVisual::deinitialize() {
    return true;
}

void QMLVisual::settingsChanged() {
    emit message("QMLVisual::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<MLVisualSettings>();
    //*set = MLVisualSettings(Settings::get(), settingsPath());
    *set = *(Settings::fetch<MLVisualSettings>(settingsPath()));

    m_viewer->setSource(QUrl::fromLocalFile(set->viewerPath()));
    m_viewer->setResizeMode(QQuickWidget::SizeRootObjectToView);

    const auto err = m_viewer->errors();
    emit message(QString("QMLVisual::settingsChanged: errors %1").arg(err.size()), LoggerSeverity::LOG_DEBUG);
    emit settingsApplied();
}

SettingsMdi* QMLVisual::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QMLVisual::settingsChanged);
    return ret;
}

