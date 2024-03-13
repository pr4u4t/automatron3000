#include <QCoreApplication>
#include <QTranslator>

#include "QLinBus.h"

struct QLinBusMenu {
    QLinBusMenu(QCoreApplication* app)
        : m_app(app) {
        m_translator = new QTranslator();
        if (m_translator->load(QLocale::system(), "QLinBus", "_", "translations")) { //set directory of ts
            m_app->installTranslator(m_translator);
        }

        //m_linbusMenu = new QMenu(m_app->translate("MainWindow", "&LinBus"));
        m_linbusAction = new QAction(m_app->translate("MainWindow", "Test/Scan"), m_linbusMenu);
        m_linbusAction->setData(QVariant("QLinBus"));
        //m_linbusMenu->addAction(m_linbusAction);
        //m_linbusMenu->addSeparator();
        m_linbusSettings = new QAction(m_app->translate("MainWindow", "Settings"), m_linbusMenu);
        //m_linbusMenu->addAction(m_linbusSettings);
    }

    QMenu* m_linbusMenu = nullptr;
    QAction* m_linbusAction = nullptr;
    QAction* m_linbusSettings = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QLinBus_register(Window* win, PluginsLoader* ld, QLinBusMenu* ctx, Logger* log) {

    QObject::connect(ctx->m_linbusAction, &QAction::triggered, win, &Window::createOrActivate);
    QMenu* menu = win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    //win->menuBar()->insertMenu(menu->menuAction(), ctx->m_linbusMenu);
    menu->addSeparator();
    menu->insertAction(nullptr, ctx->m_linbusAction);
    menu->insertAction(nullptr, ctx->m_linbusSettings);

    QObject::connect(ctx->m_linbusSettings, &QAction::triggered, [ld, win, ctx] {
        QSharedPointer<Plugin> linbus = ld->instance("QLinBus", win);
        SettingsDialog* dialog = new SettingsDialog(win, nullptr, linbus->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, linbus.dynamicCast<QLinBus>().data(), &QLinBus::settingsChanged);
        win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Linbus-Settings"));
    });

    return true;
}

static bool QLinBus_unregister(Window* win, PluginsLoader* ld, QLinBusMenu* ctx, Logger* log) {
    return true;
}

REGISTER_PLUGIN(
    QLinBus,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    QLinBus_register,
    QLinBus_unregister,
    QLinBusMenu,
    {"QLin"}
)

QLinBus::QLinBus(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
	: Widget(ld, plugins, parent, path)
    , m_ui(new Ui::QLinBusUI){
    m_ui->setupUi(this);

}

void QLinBus::settingsChanged() {

}