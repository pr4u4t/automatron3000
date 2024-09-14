#include "QLinReadByID.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_linreadbyid.h"

#include <QtGlobal>
#include <QtGui>

struct QLinReadByIDMenu {
    QLinReadByIDMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QLinReadByID", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }


        m_QLinReadByIDMenu = new QMenu(m_app->translate("MainWindow", "ReadByID"));

        m_settings = new QMenu(m_app->translate("MainWindow", "Settings")); //new QAction(m_app->translate("MainWindow", "Settings"), m_badgesMenu);
        //m_QLinReadByIDMenu->addMenu(m_settings);

        m_newInstance = new QAction(m_app->translate("MainWindow", "New instance"), m_QLinReadByIDMenu);
        m_newInstance->setData(QVariant("QLinReadByID"));
        //m_QLinReadByIDMenu->addAction(m_newInstance);
    }

    QMenu* m_QLinReadByIDMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinReadByID_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinReadByIDMenu* ctx, Logger* log) {
    log->message("QLinReadByID_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    ctx->m_QLinReadByIDMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    //win->menuBar()->insertMenu(menu->menuAction(), ctx->m_linbusMenu);
    ctx->m_QLinReadByIDMenu->addSection(ctx->m_app->translate("MainWindow", "Read By ID"));
    ctx->m_QLinReadByIDMenu->addMenu(ctx->m_settings);
    ctx->m_QLinReadByIDMenu->insertAction(nullptr, ctx->m_newInstance);


    QMenu* windowMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        gtx->m_win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_QLinReadByIDMenu);
    }

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        if (plugin->name() != "QLinReadByID") {
            return;
        }

        QAction* settings = new QAction(plugin->settingsPath(), ctx->m_QLinReadByIDMenu);
        settings->setData(QVariant(plugin->settingsPath()));
        ctx->m_settings->addAction(settings);

        QObject::connect(settings, &QAction::triggered, [gtx, plugin, ctx] {
            if (gtx->m_win->toggleWindow(dynamic_cast<const QLinReadByID*>(plugin)->objectName() + "/Settings")) {
                return;
            }

            SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, plugin->settingsPath());
            QObject::connect(dialog, &SettingsDialog::settingsUpdated, dynamic_cast<const QLinReadByID*>(plugin), &QLinReadByID::settingsChanged);
            gtx->m_win->addSubWindow(dialog, dynamic_cast<const QLinReadByID*>(plugin)->objectName() + "/Settings"); //ctx->m_app->translate("MainWindow", "QLinReadByID/Settings"));
            });
        });
        

    return true;
}

static bool QLinReadByID_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinReadByIDMenu* ctx, Logger* log) {
    log->message("QLinReadByID_unregister()");
    return true;
}

REGISTER_PLUGIN(
    QLinReadByID,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinReadByID_register,
    QLinReadByID_unregister,
    QLinReadByIDMenu,
    {"QLin"},
    true,
    900
)

QLinReadByID::QLinReadByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath, new SettingsDialog::LinReadByIDSettings(Settings::get(), settingsPath))
    , m_ui(new Ui::QLinReadByIDUI) {
    m_ui->setupUi(this);
    settingsChanged();
    QObject::connect(m_ui->pushButton, &QPushButton::clicked, this, &QLinReadByID::readById);
}

SettingsMdi* QLinReadByID::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, nullptr, settingsPath());
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QLinReadByID::settingsChanged);
    return ret;
}

QLinReadByID::~QLinReadByID() {
}

void QLinReadByID::dataReady(const QByteArray& data) {

}

void QLinReadByID::startRead() {

}

void QLinReadByID::init() {

}

void QLinReadByID::settingsChanged() {
    emit message("QBadge::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    *(settings<SettingsDialog::LinReadByIDSettings>()) = SettingsDialog::LinReadByIDSettings(Settings::get(), settingsPath());
    m_ui->title->setText(settings<SettingsDialog::LinReadByIDSettings>()->title);
}

void QLinReadByID::readById(bool checked) {
    m_ui->result->setText("0xf18c6547");
}