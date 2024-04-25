#include <QCoreApplication>
#include <QTranslator>

#include "QLinBus.h"
#include "../core/core.h"

struct QLinBusMenu {
    QLinBusMenu(QCoreApplication* app)
        : m_app(app) {
        m_translator = new QTranslator();
        if (m_translator->load(QLocale::system(), "QLinBus", "_", "translations")) { //set directory of ts
            m_app->installTranslator(m_translator);
        }

        //m_linbusMenu = new QMenu(m_app->translate("MainWindow", "&LinBus"));
        m_linbusAction = new QAction(m_app->translate("MainWindow", "Sniff/Scan"), m_linbusMenu);
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

static bool QLinBus_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinBusMenu* ctx, Logger* log) {

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("PluginList_register(): application is non gui not registering");
        return false;
    }

    QObject::connect(ctx->m_linbusAction, &QAction::triggered, gtx->m_win, &Window::createOrActivate);
    QMenu* menu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "&LinBus"));
    //win->menuBar()->insertMenu(menu->menuAction(), ctx->m_linbusMenu);
    menu->addSeparator();
    menu->insertAction(nullptr, ctx->m_linbusAction);
    menu->insertAction(nullptr, ctx->m_linbusSettings);

    QObject::connect(ctx->m_linbusSettings, &QAction::triggered, [ld, gtx, ctx] {
        QSharedPointer<Plugin> linbus = ld->instance("QLinBus", gtx->m_win);
        SettingsDialog* dialog = new SettingsDialog(gtx->m_win, nullptr, linbus->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, linbus.dynamicCast<QLinBus>().data(), &QLinBus::settingsChanged);
        gtx->m_win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Linbus-Settings"));
    });

    return true;
}

static bool QLinBus_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QLinBusMenu* ctx, Logger* log) {
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
    {"QLin"},
    true
)

QLinBus::QLinBus(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
	: Widget(ld, plugins, parent, path)
    , m_ui(new Ui::QLinBusUI)
    , m_model(new QStandardItemModel(0,5)){
    m_ui->setupUi(this);
    connect(m_ui->startButton, &QPushButton::clicked, this, &QLinBus::scanStep);
    connect(m_ui->stopButton, &QPushButton::clicked, this, &QLinBus::scanStop);
    connect(m_ui->clearButton, &QPushButton::clicked, this, &QLinBus::scanClear);
    m_ui->stopButton->setEnabled(false);

    QTimer::singleShot(0, this, &QLinBus::init);

    m_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Type"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("DLC"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Data"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Time"));

    m_ui->scanTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->scanTable->setModel(m_model);

    settingsChanged();
}

void QLinBus::settingsChanged() {
    m_settings = SettingsDialog::LinBusSettings(Settings::get(), settingsPath());
}

void QLinBus::init() {
    auto plugin = plugins()->instance("QLin", 0);
    auto lin = plugin.dynamicCast<IODevice>();
    m_lin = lin;
    connect(lin.data(), &IODevice::dataReady, this, &QLinBus::dataReady);
    if (lin->isOpen() == false) {
        lin->open();
    }
}

void QLinBus::scanStep() {
    if (m_scan == m_settings.scanStartID) {
        m_model->removeRows(0, m_model->rowCount());
        m_ui->startButton->setEnabled(false);
        m_ui->stopButton->setEnabled(true);
    }

    if (m_scan <= m_settings.scanStopID) {
        QByteArray data(1, static_cast<char>(m_scan));
        m_lin->write(data);

        QList<QStandardItem*> rows;
        rows << new QStandardItem("0x" + QString::number(m_scan, 16)) << new QStandardItem("REQUEST") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem("-");
        m_model->appendRow(rows);

        m_ui->scanProgress->setValue(m_scan);
        ++m_scan;
        QTimer::singleShot(0, this, &QLinBus::scanStep);
    }
    else {
        m_scan = m_settings.scanStartID;
        m_ui->startButton->setEnabled(true);
        m_ui->stopButton->setEnabled(false);
    }
}

void QLinBus::dataReady(const QByteArray& data) {
    emit message("QLinBus::dataReady(): " + data);

    if (data.startsWith("LIN NOANS")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }
        QList<QStandardItem*> rows;
        rows << new QStandardItem(list[0]) << new QStandardItem("NOANS") << new QStandardItem("-") << new QStandardItem("-") << new QStandardItem(list[2]);
        m_model->appendRow(rows);
    }
    else if (data.startsWith("ID")) {
        QByteArrayList list = data.split(',');
        for (auto& item : list) {
            item = item.mid(item.indexOf(':') + 1);
        }
        QList<QStandardItem*> rows;
        rows << new QStandardItem(list[0]) << new QStandardItem("ANS") << new QStandardItem(list[1].removeIf([](QChar ch) { return ch == '\'';  })) << new QStandardItem(list[2]) << new QStandardItem(list[3]);
        m_model->appendRow(rows);
    }

    m_ui->scanTable->verticalScrollBar()->setSliderPosition(m_ui->scanTable->verticalScrollBar()->maximum());
}

void QLinBus::scanStop() {
    m_scan = m_settings.scanStopID;
}

void QLinBus::scanClear() {
    m_model->removeRows(0, m_model->rowCount());
}