#include "QLinWriteByID.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_linwritebyid.h"

#include <QtGlobal>
#include <QtGui>

struct QLinWriteByIDMenu {
    QLinWriteByIDMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinWriteByID", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }


        m_QLinWriteByIDMenu = new QMenu(m_app->translate("MainWindow", "WriteByID"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        //m_QLinReadByIDMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_QLinWriteByIDMenu);
        m_newInstance->setData(QVariant("QLinWriteByID"));
        //m_QLinReadByIDMenu->addAction(m_newInstance);
    }

    QMenu* m_QLinWriteByIDMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinWriteByID_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinWriteByIDMenu* ctx, Logger* log) {
    log->message("QLinWriteByID_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    ctx->m_QLinWriteByIDMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    //win->menuBar()->insertMenu(menu->menuAction(), ctx->m_linbusMenu);
    ctx->m_QLinWriteByIDMenu->addSection(ctx->m_app->translate("MainWindow", "Read By ID"));
    ctx->m_QLinWriteByIDMenu->addMenu(ctx->m_settings);
    ctx->m_QLinWriteByIDMenu->insertAction(nullptr, ctx->m_newInstance);


    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_QLinWriteByIDMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QLinWriteByID") {
            return;
        }

        QAction* settings = new QAction(plugin->settingsPath(), ctx->m_QLinWriteByIDMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QLinWriteByID*>(plugin)->objectName() + "/Settings")) {
                return;
            }
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QLinWriteByID*>(plugin), &QLinWriteByID::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QLinWriteByID*>(plugin)->objectName() + "/Settings");  //ctx->m_app->translate("MainWindow", "QLinWriteByID/Settings"));
            });
        });


    return true;
}

static bool QLinWriteByID_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinWriteByIDMenu* ctx, Logger* log) {
    log->message("QLinWriteByID_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QLinWriteByID,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinWriteByID_register,
    QLinWriteByID_unregister,
    QLinWriteByIDMenu,
    { "QLin" },
    true,
    1100
)


QLinWriteByID::QLinWriteByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath, new SettingsDialog::LinWriteByIDSettings(Settings::get(), settingsPath))
    , m_ui(new Ui::QLinWriteByIDUI) {
    m_ui->setupUi(this);
    settingsChanged();
}

QLinWriteByID::~QLinWriteByID() {
}

SettingsMdi* QLinWriteByID::settingsWindow() const {
   auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
   QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinWriteByID::settingsChanged);
   return ret;
}

void QLinWriteByID::dataReady(const QByteArray& data) {

}

void QLinWriteByID::startRead() {

}

void QLinWriteByID::init() {

}

void QLinWriteByID::settingsChanged() {
    emit message("QBadge::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    *(settings<SettingsDialog::LinWriteByIDSettings>()) = SettingsDialog::LinWriteByIDSettings(Settings::get(), settingsPath());
    m_ui->title->setText(settings<SettingsDialog::LinWriteByIDSettings>()->title);

}
