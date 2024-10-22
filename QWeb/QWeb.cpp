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

        m_webMenu = new QMenu(m_app->translate("MainWindow", "Web"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        m_webMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_webMenu);
        m_newInstance->setData(QVariant("QWeb"));
        m_webMenu->addAction(m_newInstance);
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
        log->message("QWeb_register: application is non gui not registering");
        return false;
    }

    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));
    if (windowMenu != nullptr) {
        windowMenu->addMenu(ctx->m_webMenu);
        //gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_badgesMenu);
        log->message("QWeb_register: menu added");
    } else {
        log->message("QWeb_register: failed to find visualization menu");
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QWeb") {
            return;
        }

        QAction* settings = new QAction(dynamic_cast<const QObject*>(plugin)->objectName(), ctx->m_webMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QWeb*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QWeb*>(plugin), &QWeb::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QWeb*>(plugin)->objectName() + "/Settings"); // ctx->m_app->translate("MainWindow", dynamic_cast<const QBadge*>(plugin)->objectName() + "/Settings"));
            });
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
    100
)

QWeb::QWeb(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        new SettingsDialog::WebSettings() // Settings::get(), sPath)
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
    const auto set = settings<SettingsDialog::WebSettings>();
    *set = SettingsDialog::WebSettings(Settings::get(), settingsPath());
    m_ui->webView->setUrl(QUrl(set->url));
}

SettingsMdi* QWeb::settingsWindow() const {
    emit message("QWeb::settingsWindow()", LoggerSeverity::LOG_DEBUG);
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QWeb::settingsChanged);
    return ret;
}
