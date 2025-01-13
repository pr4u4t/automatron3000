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
        log->message("QBadge_register: application is non gui not registering");
        return false;
    }

    if (gtx->m_win == nullptr) {
        log->message("QBadge_register(): window pointer == nullptr");
        return false;
    }

    auto visualMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));
    ctx->m_badgesMenu = visualMenu->addMenu(ctx->m_app->translate("MainWindow", "Badges"));

    windowAddInstanceSettings(ctx->m_badgesMenu, &ctx->m_settings, &ctx->m_newInstance, "QBadge", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QBadge, QBadgeMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QBadge"), gtx, ctx, log);
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
    100,
    BadgeSettings
)

QBadge::QBadge(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath, BadgeSettings* set, const QString& uuid)
    : Widget(
        ld, 
        plugins, 
        parent, 
        sPath, 
        set,
        uuid
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
    const auto set = settings<BadgeSettings>();
    *set = *(Settings::fetch<BadgeSettings>(settingsPath()));
    //*set = BadgeSettings(Settings::get(), settingsPath());

    m_ui->label->setText(set->text());
    m_ui->title->setText(set->title());

    QPixmap pix;
    if (set->imagePath().isEmpty() == false && pix.load(set->imagePath())) {
        m_ui->image->setPixmap(pix);
    }

    emit settingsApplied();
}

SettingsMdi* QBadge::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QBadge::settingsChanged);
    return ret;
}
