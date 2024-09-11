#include "QJTAG.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_qjtag.h"

#include <QtGlobal>
#include <QtGui>

struct QJTAGMenu {
    QJTAGMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QJTAG", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_QJTAGMenu = new QMenu(m_app->translate("MainWindow", "JTAG"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        m_QJTAGMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_QJTAGMenu);
        m_newInstance->setData(QVariant("QJTAG"));
        m_QJTAGMenu->addAction(m_newInstance);
    }

    QMenu* m_QJTAGMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;
    
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QJTAG_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QJTAGMenu* ctx, Logger* log) {
    log->message("QJTAG_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_QJTAGMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin){
        if (plugin->name() != "QJTAG") {
            return;
        }
        
        QAction* settings = new QAction(plugin->settingsPath(), ctx->m_QJTAGMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QJTAG*>(plugin), &QJTAG::settingsChanged);
            gtx->m_win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "QJTAG-Settings"));
        });
    });

    return true;
}

static bool QJTAG_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QJTAGMenu* ctx, Logger* log) {
    log->message("QJTAG_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QJTAG,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QJTAG_register,
    QJTAG_unregister,
    QJTAGMenu,
    {},
    true
)

QJTAG::QJTAG(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath) 
    : Widget(ld, plugins, parent, settingsPath, new SettingsDialog::QJTAGSettings(Settings::get(), settingsPath))
    , m_ui(new Ui::QJTAGUI) {
    m_ui->setupUi(this);
    settingsChanged();
}

QJTAG::~QJTAG() {
}

bool QJTAG::saveSettings() {
    return true;
}

void QJTAG::settingsChanged() {
    emit message("QJTAG::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    *(settings<SettingsDialog::QJTAGSettings>()) = SettingsDialog::QJTAGSettings(Settings::get(), settingsPath());

    m_ui->execButton->setText(settings<SettingsDialog::QJTAGSettings>()->buttonLabel);
    m_ui->title->setText(settings<SettingsDialog::QJTAGSettings>()->title);
}
