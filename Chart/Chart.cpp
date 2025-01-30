#include "Chart.h"
#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_chart.h"

#include <QtGlobal>
#include <QtGui>

struct QChartMenu {
    QChartMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QChart", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_chartMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool Chart_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QChartMenu* ctx, Logger* log) {
    log->message("Chart_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("Chart_register: application is non gui not registering");
        return false;
    }

    if (gtx->m_win == nullptr) {
        log->message("Chart_register(): window pointer == nullptr");
        return false;
    }

    auto visualMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));
    ctx->m_chartMenu = visualMenu->addMenu(ctx->m_app->translate("MainWindow", "Charts"));

    windowAddInstanceSettings(ctx->m_chartMenu, &ctx->m_settings, &ctx->m_newInstance, "Chart", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<Chart, QChartMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("Chart"), gtx, ctx, log);
    });

    return true;
}

static bool Chart_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, QChartMenu* ctx, Logger* log) {
    log->message("Chart_unregister()");
    return true;
}

REGISTER_PLUGIN(
    Chart,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    Chart_register,
    Chart_unregister,
    QChartMenu,
    {},
    true,
    100,
    ChartSettings
)

Chart::Chart(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath, ChartSettings* set, const QString& uuid)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        set,
        uuid
    )
    , m_ui(new Ui::ChartUI) {
    m_ui->setupUi(this);
}

bool Chart::saveSettings() {
    return true;
}

/*!
  \brief Destructor
  \param none
*/
Chart::~Chart() {
}

//void QBadge::resizeEvent(QResizeEvent* event) {
//    
//    m_pixmap = m_pixmap.scaled(event->size(), Qt::KeepAspectRatio);
//    m_ui->image->setPixmap(m_pixmap);
//
//    Widget::resizeEvent(event);
//}

bool Chart::initialize() {
    //settingsChanged();
    const auto set = settings<ChartSettings>();
    //m_ui->label->setText(set->text());
    //m_ui->title->setText(set->title());

    m_series = new QLineSeries();
    m_chart = new QChart();

    m_chart->addSeries(m_series);
    m_chart->setTitle(set->title());
    m_chart->setAnimationOptions(QChart::SeriesAnimations);

    m_axisX = new QValueAxis();
    m_axisX->setTitleText(set->xAxisName());
    m_axisX->setRange(set->xBegin(), set->xEnd());

    m_axisY = new QDateTimeAxis();
    m_axisY->setTitleText(set->yAxisName());
    m_axisY->setMin(QDateTime::currentDateTime() - static_cast<std::chrono::milliseconds>(static_cast<int>(set->xBegin()) * 1000));
    m_axisY->setMax(QDateTime::currentDateTime() + static_cast<std::chrono::milliseconds>(static_cast<int>(set->xEnd()) * 1000));

    //m_chart->setAxisX(m_axisX, m_series);
    //m_chart->setAxisY(m_axisY, m_series);

    // Attach axes to the chart and the series
    m_chart->addAxis(m_axisY, Qt::AlignBottom);
    m_chart->addAxis(m_axisX, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    // Create a QChartView to display the chart
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    m_ui->verticalLayout->addWidget(m_chartView);

    return true;
}

bool Chart::deinitialize() {

    return true;
}

void Chart::settingsChanged() {
    emit message("Chart::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<ChartSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<ChartSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

SettingsMdi* Chart::settingsWindow() const {
    emit message("Chart::settingsWindow()", LoggerSeverity::LOG_DEBUG);
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &Chart::settingsChanged);
    return ret;
}

