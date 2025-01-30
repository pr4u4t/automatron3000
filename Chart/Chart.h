#ifndef CHART_H
#define CHART_H

#include "chart_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QChartView>

QT_BEGIN_NAMESPACE

namespace Ui {
    class ChartUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class CHART_EXPORT Chart : public Widget {

    Q_OBJECT

public:
    Chart(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, ChartSettings* set, const QString& uuid);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~Chart();

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("Chart::reset(Reset type)");
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:

    void settingsChanged();

    QVariant exec() override {
        return QVariant();
    }

private:
    Ui::ChartUI* m_ui = nullptr;
    QLineSeries* m_series = nullptr;
    QChart* m_chart = nullptr;
    QValueAxis* m_axisX = nullptr;
    QDateTimeAxis* m_axisY = nullptr;
    QChartView* m_chartView = nullptr;
};

#endif