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
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QBadge") {
            return;
        }

        QAction* settings = new QAction(dynamic_cast<const QObject*>(plugin)->objectName(), ctx->m_badgesMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QBadge*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QBadge*>(plugin), &QBadge::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QBadge*>(plugin)->objectName() + "/Settings"); // ctx->m_app->translate("MainWindow", dynamic_cast<const QBadge*>(plugin)->objectName() + "/Settings"));
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
    true,
    100
)

QBadge::QBadge(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath)
    : Widget(
        ld, 
        plugins, 
        parent, 
        sPath, 
        new SettingsDialog::BadgeSettings() // Settings::get(), sPath)
    )
    , m_ui(new Ui::QBadgeUI) {
    m_ui->setupUi(this);
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

bool QBadge::initialize() {
    settingsChanged();
    return true;
}

bool QBadge::deinitialize() {

    return true;
}

void QBadge::settingsChanged() {
    emit message("QBadge::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<SettingsDialog::BadgeSettings>();
    *set = SettingsDialog::BadgeSettings(Settings::get(), settingsPath());

    m_ui->label->setText(set->text);
    m_ui->title->setText(set->title);

    QPixmap pix;
    if (set->imagePath.isEmpty() == false && pix.load(set->imagePath)) {
        m_ui->image->setPixmap(pix);
    }
}

SettingsMdi* QBadge::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QBadge::settingsChanged);
    return ret;
}
