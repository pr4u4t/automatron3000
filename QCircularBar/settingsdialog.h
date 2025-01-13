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

struct CircularBarSettings : public PluginSettings {
    Q_GADGET

    Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)
    Q_PROPERTY(double threshold READ threshold WRITE setThreshold)
    Q_PROPERTY(int precision READ precision WRITE setPrecision)
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(QString units READ units WRITE setUnits)
    Q_PROPERTY(int steps READ steps WRITE setSteps)

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

public:
    CircularBarSettings()
        : m_minValue(minValueValue)
        , m_maxValue(maxValueValue)
        , m_threshold(thresholdValue)
        , m_precision(precisionValue)
        , m_label(labelValue)
        , m_units(unitsValue)
        , m_steps(stepsValue) {
        registerMetaObject(&staticMetaObject);
    }

    CircularBarSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_minValue(settings.value(settingsPath + "/" + minValueKey, minValueValue).toDouble())
        , m_maxValue(settings.value(settingsPath + "/" + maxValueKey, maxValueValue).toDouble())
        , m_threshold(settings.value(settingsPath + "/" + thresholdKey, thresholdValue).toDouble())
        , m_precision(settings.value(settingsPath + "/" + precisionKey, precisionValue).toInt())
        , m_label(settings.value(settingsPath + "/" + labelKey, labelValue).toString())
        , m_units(settings.value(settingsPath + "/" + unitsKey, unitsValue).toString())
        , m_steps(settings.value(settingsPath + "/" + stepsKey, stepsValue).toInt()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + minValueKey, m_minValue);
        settings.setValue(settingsPath + "/" + maxValueKey, m_maxValue);
        settings.setValue(settingsPath + "/" + thresholdKey, m_threshold);
        settings.setValue(settingsPath + "/" + precisionKey, m_precision);
        settings.setValue(settingsPath + "/" + labelKey, m_label);
        settings.setValue(settingsPath + "/" + unitsKey, m_units);
        settings.setValue(settingsPath + "/" + stepsKey, m_steps);

        PluginSettings::save(settings, settingsPath);
    }

    double minValue() const {
        return m_minValue;
    }

    void setMinValue(double minValue) {
        m_minValue = minValue;
    }

    double maxValue() const {
        return m_maxValue;
    }

    void setMaxValue(double maxValue) {
        m_maxValue = maxValue;
    }

    double threshold() {
        return m_threshold;
    }

    void setThreshold(double threshold) {
        m_threshold = threshold;
    }

    int precision() const {
        return m_precision;
    }

    void setPrecision(int precision) {
        m_precision = precision;
    }

    QString label() const {
        return m_label;
    }

    void setLabel(const QString& label) {
        m_label = label;
    }

    QString units() const {
        return m_units;
    }

    void setUnits(const QString& units) {
        m_units = units;
    }

    int steps() const {
        return m_steps;
    }

    void setSteps(int steps) {
        m_steps = steps;
    }

    bool operator==(const CircularBarSettings& other) const {
        return m_minValue == other.m_minValue &&
            m_maxValue == other.m_maxValue &&
            m_threshold == other.m_threshold &&
            m_precision == other.m_precision &&
            m_label == other.m_label &&
            m_units == other.m_units &&
            m_steps == other.m_steps;
    }

    bool operator!=(const CircularBarSettings& other) const {
        return !(*this == other);
    }

private:
    double m_minValue;
    double m_maxValue;
    double m_threshold;
    int m_precision;
    QString m_label;
    QString m_units;
    int m_steps;
};

class QCircularBar;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QCircularBar* bar);

    ~SettingsDialog();

    operator CircularBarSettings() const;

private slots:
    void ok();
    void apply();
    void cancel();

private:
    void updateSettings();
    void fillFromSettings();
    void setup();

private:
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
};

#endif
