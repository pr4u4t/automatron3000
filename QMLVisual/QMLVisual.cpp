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

        m_mlvisualMenu = new QMenu(m_app->translate("MainWindow", "QMLVisual"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        m_mlvisualMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_mlvisualMenu);
        m_newInstance->setData(QVariant("QMLVisual"));
        m_mlvisualMenu->addAction(m_newInstance);
    }

    QMenu* m_mlvisualMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QMLVisual_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QMLVisualMenu* ctx, Logger* log) {
    log->message("QMLVisual_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QMLVisual_register: application is non gui not registering");
        return false;
    }

    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));
    if (windowMenu != nullptr) {
        //gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_mlvisualMenu);
        windowMenu->addMenu(ctx->m_mlvisualMenu);
    } else {
        log->message("QMLVisual_register: failed to find menu");
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QMLVisual") {
            return;
        }

        QAction* settings = new QAction(dynamic_cast<const QObject*>(plugin)->objectName(), ctx->m_mlvisualMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QMLVisual*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QMLVisual*>(plugin), &QMLVisual::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QMLVisual*>(plugin)->objectName() + "/Settings"); // ctx->m_app->translate("MainWindow", dynamic_cast<const QBadge*>(plugin)->objectName() + "/Settings"));
            });
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
    100
)

QMLVisual::QMLVisual(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        new MLVisualSettings() // Settings::get(), sPath)
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
    *set = MLVisualSettings(Settings::get(), settingsPath());

    m_viewer->setSource(QUrl::fromLocalFile(set->viewerPath()));
    m_viewer->setResizeMode(QQuickWidget::SizeRootObjectToView);

    const auto err = m_viewer->errors();
    emit message(QString("QMLVisual::settingsChanged: errorrs %1").arg(err.size()), LoggerSeverity::LOG_DEBUG);
}

SettingsMdi* QMLVisual::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QMLVisual::settingsChanged);
    return ret;
}

