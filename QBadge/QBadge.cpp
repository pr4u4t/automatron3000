#include "QBadge.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qbadge.h"

#include <QtGlobal>
#include <QtGui>

struct QBadgeMenu {
    QBadgeMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QBadge", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_badgesMenu = new QMenu(m_app->translate("MainWindow", "Badges"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        m_badgesMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_badgesMenu);
        m_newInstance->setData(QVariant("QBadge"));
        m_badgesMenu->addAction(m_newInstance);
    }

    QMenu* m_badgesMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;
    
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QBadge_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QBadgeMenu* ctx, Logger* log) {
    log->message("QBadge_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_badgesMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin){
        if (plugin->name() != "QBadge") {
            return;
        }
        
        QAction* settings = new QAction(plugin->settingsPath(), ctx->m_badgesMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QBadge*>(plugin), &QBadge::settingsChanged);
            gtx->m_win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Database-Settings"));
        });
    });

    return true;
}

static bool QBadge_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QBadgeMenu* ctx, Logger* log) {
    log->message("QBadge_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QBadge,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QBadge_register,
    QBadge_unregister,
    QBadgeMenu,
    {},
    true
)

QBadge::QBadge(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath)
    , m_ui(new Ui::QBadgeUI) {
    m_ui->setupUi(this);
    settingsChanged();
}

bool QBadge::saveSettings() {
	return true;
}

/*!
  \brief Destructor
  \param none
*/
QBadge::~QBadge() {
}

//void QBadge::resizeEvent(QResizeEvent* event) {
//    
//    m_pixmap = m_pixmap.scaled(event->size(), Qt::KeepAspectRatio);
//    m_ui->image->setPixmap(m_pixmap);
//
//    Widget::resizeEvent(event);
//}

void QBadge::settingsChanged() {
    emit message("QBadge::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    m_settings = SettingsDialog::BadgeSettings(Settings::get(), settingsPath());
    
    //m_ui->label->clear();
    //m_ui->image->clear();

    m_ui->label->setText(m_settings.text);
    m_ui->title->setText(m_settings.title);

    QPixmap pix;
    if (m_settings.imagePath.isEmpty() == false && pix.load(m_settings.imagePath)) {
        m_ui->image->setPixmap(pix);
    }
}
