#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>

#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:


    static constexpr const char* const minValueKey = "minValue";
    static constexpr double minValueValue = 0.0;
    static constexpr const char* const maxValueKey = "maxValue";
    static constexpr double maxValueValue = 1.0;
    static constexpr const char* const thresholdKey = "treshold";
    static constexpr double thresholdValue = 0.0;
    static constexpr const char* const precisionKey = "precision";
    static constexpr int precisionValue = 0;
    static constexpr const char* const labelKey = "label";
    static constexpr const char* const labelValue = "Gauge";
    static constexpr const char* const unitsKey = "units";
    static constexpr const char* const unitsValue = nullptr;
    static constexpr const char* const stepsKey = "setps";
    static constexpr const int stepsValue = 0;

    struct CircularBarSettings : public PluginSettings {
        CircularBarSettings()
            : minValue(minValueValue)
            , maxValue(maxValueValue)
            , threshold(thresholdValue)
            , precision(precisionValue)
            , label(labelValue)
            , units(unitsValue)
            , steps(stepsValue) {
        }

        CircularBarSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , minValue(settings.value(settingsPath + "/" + minValueKey, minValueValue).toDouble())
            , maxValue(settings.value(settingsPath + "/" + maxValueKey, maxValueValue).toDouble ())
            , threshold(settings.value(settingsPath + "/" + thresholdKey, thresholdValue).toDouble())
            , precision(settings.value(settingsPath + "/" + precisionKey, precisionValue).toInt())
            , label(settings.value(settingsPath + "/" + labelKey, labelValue).toString())
            , units(settings.value(settingsPath + "/" + unitsKey, unitsValue).toString())
            , steps(settings.value(settingsPath + "/" + stepsKey, stepsValue).toInt()) {
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + minValueKey, minValue);
            settings.setValue(settingsPath + "/" + maxValueKey, maxValue);
            settings.setValue(settingsPath + "/" + thresholdKey, threshold);
            settings.setValue(settingsPath + "/" + precisionKey, precision);
            settings.setValue(settingsPath + "/" + labelKey, label);
            settings.setValue(settingsPath + "/" + unitsKey, units);
            settings.setValue(settingsPath + "/" + stepsKey, steps);

            PluginSettings::save(settings, settingsPath);
        }

        double minValue;
        double maxValue;
        double threshold;
        int precision;
        QString label;
        QString units;
        int steps;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    CircularBarSettings circularbarSettings() const;

    QString settingsPath() const;

private slots:
    void ok();
    void apply();
    void cancel();

private:
    void updateSettings();
    void fillFromSettings();

private:
    CircularBarSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif
