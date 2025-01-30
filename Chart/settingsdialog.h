#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>

#include "../api/api.h"

class Chart;

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

struct ChartSettings : public PluginSettings {
    Q_GADGET
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString xAxisName READ xAxisName WRITE setXAxisName)
    Q_PROPERTY(QString yAxisName READ yAxisName WRITE setYAxisName)
    Q_PROPERTY(float xBegin READ xBegin WRITE setXBegin)
    Q_PROPERTY(float xEnd READ xEnd WRITE setXEnd)
    Q_PROPERTY(float yBegin READ yBegin WRITE setYBegin)
    Q_PROPERTY(float yEnd READ yEnd WRITE setYEnd)

    static constexpr const char* const titleKey = "title";
    static constexpr const char* const titleValue = nullptr;
    static constexpr const char* const xAxisNameKey = "xAxisName";
    static constexpr const char* const xAxisNameValue = "x";
    static constexpr const char* const yAxisNameKey = "yAxisName";
    static constexpr const char* const yAxisNameValue = "y";
    static constexpr const char* const xAxisBeginKey = "xBegin";
    static constexpr const float xAxisBeginValue = 0.0f;
    static constexpr const char* const xAxisEndKey = "xEnd";
    static constexpr const float xAxisEndValue = 10.0f;
    static constexpr const char* const yAxisBeginKey = "yBegin";
    static constexpr const float yAxisBeginValue = 0.0f;
    static constexpr const char* const yAxisEndKey = "yEnd";
    static constexpr const float yAxisEndValue = 10.0f;

public:
    ChartSettings()
        : m_title(titleValue) 
        , m_xaxisName(xAxisNameValue)
        , m_yaxisName(yAxisNameValue)
        , m_xbegin(xAxisBeginValue)
        , m_xend(xAxisEndValue)
        , m_ybegin(yAxisBeginValue)
        , m_yend(xAxisEndValue) {
        registerMetaObject(&staticMetaObject);
    }

    virtual ~ChartSettings() = default;

    ChartSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
        , m_xaxisName(settings.value(settingsPath + "/" + xAxisNameKey, xAxisNameValue).toString())
        , m_yaxisName(settings.value(settingsPath + "/" + yAxisNameKey, yAxisNameValue).toString())
        , m_xbegin(settings.value(settingsPath + "/" + xAxisBeginKey, xAxisBeginValue).toFloat())
        , m_xend(settings.value(settingsPath + "/" + xAxisEndKey, xAxisEndValue).toFloat())
        , m_ybegin(settings.value(settingsPath + "/" + yAxisBeginKey, yAxisBeginValue).toFloat())
        , m_yend(settings.value(settingsPath + "/" + yAxisEndKey, yAxisEndValue).toFloat()){
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + titleKey, m_title);
        settings.setValue(settingsPath + "/" + xAxisNameKey, m_xaxisName);
        settings.setValue(settingsPath + "/" + yAxisNameKey, m_yaxisName);
        settings.setValue(settingsPath + "/" + xAxisBeginKey, m_xbegin);
        settings.setValue(settingsPath + "/" + xAxisEndKey, m_xend);
        settings.setValue(settingsPath + "/" + yAxisBeginKey, m_ybegin);
        settings.setValue(settingsPath + "/" + yAxisEndKey, m_yend);
        PluginSettings::save(settings, settingsPath);
    }

    QString title() const { return m_title; }

    void setTitle(const QString& title) { m_title = title; }

    QString xAxisName() const { return m_xaxisName; }
    QString yAxisName() const { return m_yaxisName; }
    float xBegin() const { return m_xbegin; }
    float xEnd() const { return m_xend; }
    float yBegin() const { return m_ybegin; }
    float yEnd() const { return m_yend; }

    void setXAxisName(const QString& name) { m_xaxisName = name; }
    void setYAxisName(const QString& name) { m_yaxisName = name; }
    void setXBegin(float value) { m_xbegin = value; }
    void setXEnd(float value) { m_xend = value; }
    void setYBegin(float value) { m_ybegin = value; }
    void setYEnd(float value) { m_yend = value; }

    bool operator==(const ChartSettings& other) const {
        return m_title == other.m_title;
    }

    bool operator!=(const ChartSettings& other) const {
        return !(*this == other);
    }

    QString m_title;
    QString m_xaxisName;
    QString m_yaxisName;
    float m_xbegin;
    float m_xend;
    float m_ybegin;
    float m_yend;
};

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:
   
    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const Chart* chart);

    ~SettingsDialog();

    operator ChartSettings() const;

private slots:
    void ok();

    void apply();
    
    void cancel();

    void chooseImage();

private:
    void updateSettings();
    
    void fillFromSettings();

    bool verifySettings() const;

    void setup();

private:

    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
};

#endif