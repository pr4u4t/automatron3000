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

struct LinCommandSettings : public PluginSettings {
    static constexpr const char* const frameDataKey = "frameData";
    static constexpr const char* const intervalKey = "Interval";
    static constexpr const char* const triesKey = "tries";
    static constexpr const char* const titleKey = "title";
    static constexpr const char* const buttonTextKey = "buttonText";
    static constexpr const char* const previousKey = "previous";
    static constexpr const char* const linDeviceKey = "linDevice";
    static constexpr const char* const maxReschedulesKey = "maxReschedules";
    static constexpr const char* const rescheduleIntervalKey = "rescheduleInterval";

    static constexpr const char* const frameDataValue = "0x6522f18c";
    static constexpr const qint64 intervalValue = 0;
    static constexpr const uint8_t triesValue = 3;
    static constexpr const char* const titleValue = "Command";
    static constexpr const char* const buttonTextValue = "Execute";
    static constexpr const char* const previousValue = nullptr;
    static constexpr const char* const linDeviceValue = nullptr;
    static constexpr const int maxReschedulesValue = 3;
    static constexpr const int rescheduleIntervalValue = 200;

    Q_GADGET

    Q_PROPERTY(QByteArray frameData READ frameData WRITE setFrameData)
    Q_PROPERTY(qint64 interval READ interval WRITE setInterval)
    Q_PROPERTY(uint8_t tries READ tries WRITE setTries)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString buttonText READ buttonText WRITE setButtonText)
    Q_PROPERTY(QString previous READ previous WRITE setPrevious)
    Q_PROPERTY(QString linDevice READ linDevice WRITE setLinDevice)
    Q_PROPERTY(int maxReschedules READ maxReschedules WRITE setMaxReschedules)
    Q_PROPERTY(int rescheduleInterval READ rescheduleInterval WRITE setRescheduleInterval)

public:
    LinCommandSettings()
        : m_frameData(frameDataValue)
        , m_interval(intervalValue)
        , m_tries(triesValue)
        , m_title(titleValue)
        , m_buttonText(buttonTextValue)
        , m_previous(previousValue)
        , m_linDevice(linDeviceValue) 
        , m_maxReschedules(maxReschedulesValue)
        , m_rescheduleInterval(rescheduleIntervalValue){
        registerMetaObject(&staticMetaObject);
    }

    LinCommandSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_frameData(settings.value(settingsPath + "/" + frameDataKey, frameDataValue).toByteArray())
        , m_interval(settings.value(settingsPath + "/" + intervalKey, intervalValue).toInt())
        , m_tries(settings.value(settingsPath + "/" + triesKey, triesValue).toUInt())
        , m_title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
        , m_buttonText(settings.value(settingsPath + "/" + buttonTextKey, buttonTextValue).toString())
        , m_previous(settings.value(settingsPath + "/" + previousKey, previousValue).toString())
        , m_linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString())
        , m_maxReschedules(settings.value(settingsPath + "/" + maxReschedulesKey, maxReschedulesValue).toInt())
        , m_rescheduleInterval(settings.value(settingsPath + "/" + rescheduleIntervalKey, rescheduleIntervalValue).toInt()){
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + frameDataKey, m_frameData);
        settings.setValue(settingsPath + "/" + intervalKey, m_interval);
        settings.setValue(settingsPath + "/" + triesKey, m_tries);
        settings.setValue(settingsPath + "/" + titleKey, m_title);
        settings.setValue(settingsPath + "/" + buttonTextKey, m_buttonText);
        settings.setValue(settingsPath + "/" + previousKey, m_previous);
        settings.setValue(settingsPath + "/" + linDeviceKey, m_linDevice);
        settings.setValue(settingsPath + "/" + maxReschedulesKey, m_maxReschedules);
        settings.setValue(settingsPath + "/" + rescheduleIntervalKey, m_rescheduleInterval);

        PluginSettings::save(settings, settingsPath);
    }

    QByteArray frameData() const { return m_frameData; }
    void setFrameData(const QByteArray& frameData) { m_frameData = frameData; }

    // Getter and Setter for interval
    qint64 interval() const { return m_interval; }
    void setInterval(qint64 interval) { m_interval = interval; }

    // Getter and Setter for tries
    uint8_t tries() const { return m_tries; }
    void setTries(uint8_t tries) { m_tries = tries; }

    // Getter and Setter for title
    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; }

    // Getter and Setter for buttonText
    QString buttonText() const { return m_buttonText; }
    void setButtonText(const QString& buttonText) { m_buttonText = buttonText; }

    // Getter and Setter for previous
    QString previous() const { return m_previous; }
    void setPrevious(const QString& previous) { m_previous = previous; }

    // Getter and Setter for linDevice
    QString linDevice() const { return m_linDevice; }
    void setLinDevice(const QString& linDevice) { m_linDevice = linDevice; }

    // Getter and Setter for tries
    int maxReschedules() const { return m_maxReschedules; }
    void setMaxReschedules(int resched) { m_maxReschedules = resched; }

    int rescheduleInterval() const { return m_rescheduleInterval; }
    void setRescheduleInterval(int resched) { m_rescheduleInterval = resched; }

    bool operator==(const LinCommandSettings& other) const {
        return m_frameData == other.m_frameData &&
            m_interval == other.m_interval &&
            m_tries == other.m_tries &&
            m_title == other.m_title &&
            m_buttonText == other.m_buttonText &&
            m_previous == other.m_previous &&
            m_linDevice == other.m_linDevice &&
            m_maxReschedules == other.m_maxReschedules &&
            m_rescheduleInterval == other.m_rescheduleInterval;
    }

    bool operator!=(const LinCommandSettings& other) const {
        return !(*this == other);
    }

private:
    QByteArray m_frameData;
    qint64 m_interval;
    uint8_t m_tries;
    QString m_title;
    QString m_buttonText;
    QString m_previous;
    QString m_linDevice;
    int m_maxReschedules;
    int m_rescheduleInterval;
};

class QLinCommand;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QLinCommand* command);

    ~SettingsDialog();

    operator LinCommandSettings() const;

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
