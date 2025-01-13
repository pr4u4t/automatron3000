#include "QWeb.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qweb.h"

#include <QtGlobal>
#include <QtGui>

struct QWebMenu {
    QWebMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QWeb", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_webMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QWeb_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QWebMenu* ctx, Logger* log) {
    log->message("QWeb_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QWeb_register(): application is non gui not registering");
        return false;
    }

    auto visualMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));

    if (visualMenu == nullptr) {
        log->message("QWeb_register(): visualizations menu not found");
        return false;
    }

    if ((ctx->m_webMenu = visualMenu->addMenu(ctx->m_app->translate("MainWindow", "Web"))) == nullptr) {
        log->message("QWeb_register(): failed to create web menu");
        return false;
    }

    windowAddInstanceSettings(ctx->m_webMenu, &ctx->m_settings, &ctx->m_newInstance, "QWeb", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QWeb, QWebMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QWeb"), gtx, ctx, log);
    });

    return true;
}

static bool QWeb_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QWebMenu* ctx, Logger* log) {
    log->message("QWeb_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QWeb,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QWeb_register,
    QWeb_unregister,
    QWebMenu,
    {},
    true,
    100,
    WebSettings
)

QWeb::QWeb(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath, WebSettings* set, const QString& uuid)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        set,
        uuid
    )
    , m_ui(new Ui::QWebUI) {
    m_ui->setupUi(this);
}

bool QWeb::saveSettings() {
    return true;
}

/*!
  \brief Destructor
  \param none
*/
QWeb::~QWeb() {
}

//void QBadge::resizeEvent(QResizeEvent* event) {
//    
//    m_pixmap = m_pixmap.scaled(event->size(), Qt::KeepAspectRatio);
//    m_ui->image->setPixmap(m_pixmap);
//
//    Widget::resizeEvent(event);
//}

bool QWeb::initialize() {
    settingsChanged();
    return true;
}

bool QWeb::deinitialize() {

    return true;
}

void QWeb::settingsChanged() {
    emit message("QWeb::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<WebSettings>();
    *set = WebSettings(Settings::get(), settingsPath());
    m_ui->webView->setUrl(QUrl(set->url()));
    emit settingsApplied();
}

SettingsMdi* QWeb::settingsWindow() const {
    emit message("QWeb::settingsWindow()", LoggerSeverity::LOG_DEBUG);
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QWeb::settingsChanged);
    return ret;
}
