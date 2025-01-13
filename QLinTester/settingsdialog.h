#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

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

struct LinTesterSettings : public PluginSettings {
    static constexpr const char* const testStartIDKey = "testStartID";
    static constexpr const char* const testStopIDKey = "testStopID";
    static constexpr const char* const testIntervalKey = "testInterval";
    static constexpr const char* const triesKey = "tries";
    static constexpr const char* const linDeviceKey = "linDevice";

    static constexpr const uint8_t testStartIDValue = 0;
    static constexpr const uint8_t testStopIDValue = 63;
    static constexpr const qint64 testIntervalValue = 0;
    static constexpr const uint8_t triesValue = 3;
    static constexpr const char* const linDeviceValue = nullptr;
    
    Q_GADGET

    Q_PROPERTY(uint8_t testStartID READ testStartID WRITE setTestStartID)
    Q_PROPERTY(uint8_t testStopID READ testStopID WRITE setTestStopID)
    Q_PROPERTY(qint64 testInterval READ testInterval WRITE setTestInterval)
    Q_PROPERTY(uint8_t tries READ tries WRITE setTries)
    Q_PROPERTY(QString linDevice READ linDevice WRITE setLinDevice)

public:
    LinTesterSettings()
        : m_testStartID(testStartIDValue)
        , m_testStopID(testStopIDValue)
        , m_testInterval(testIntervalValue)
        , m_tries(triesValue)
        , m_linDevice(linDeviceValue) {
        registerMetaObject(&staticMetaObject);
    }

    LinTesterSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_testStartID(settings.value(settingsPath + "/" + testStartIDKey, testStartIDValue).toUInt())
        , m_testStopID(settings.value(settingsPath + "/" + testStopIDKey, testStopIDValue).toUInt())
        , m_testInterval(settings.value(settingsPath + "/" + testIntervalKey, testIntervalValue).toInt())
        , m_tries(settings.value(settingsPath + "/" + triesKey, triesValue).toInt())
        , m_linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + testStartIDKey, m_testStartID);
        settings.setValue(settingsPath + "/" + testStopIDKey, m_testStopID);
        settings.setValue(settingsPath + "/" + testIntervalKey, m_testInterval);
        settings.setValue(settingsPath + "/" + triesKey, m_tries);
        settings.setValue(settingsPath + "/" + linDeviceKey, m_linDevice);

        PluginSettings::save(settings, settingsPath);
    }

    // Getter and Setter for testStartID
    uint8_t testStartID() const { return m_testStartID; }
    void setTestStartID(uint8_t testStartID) { m_testStartID = testStartID; }

    // Getter and Setter for testStopID
    uint8_t testStopID() const { return m_testStopID; }
    void setTestStopID(uint8_t testStopID) { m_testStopID = testStopID; }

    // Getter and Setter for testInterval
    qint64 testInterval() const { return m_testInterval; }
    void setTestInterval(qint64 testInterval) { m_testInterval = testInterval; }

    // Getter and Setter for tries
    uint8_t tries() const { return m_tries; }
    void setTries(uint8_t tries) { m_tries = tries; }

    // Getter and Setter for linDevice
    QString linDevice() const { return m_linDevice; }
    void setLinDevice(const QString& linDevice) { m_linDevice = linDevice; }

    bool operator==(const LinTesterSettings& other) const {
        return m_testStartID == other.m_testStartID &&
            m_testStopID == other.m_testStopID &&
            m_testInterval == other.m_testInterval &&
            m_tries == other.m_tries &&
            m_linDevice == other.m_linDevice;
    }

    bool operator!=(const LinTesterSettings& other) const {
        return !(*this == other);
    }

private:
    uint8_t m_testStartID = 0;
    uint8_t m_testStopID = 0;
    qint64 m_testInterval = 0;
    uint8_t m_tries = 0;
    QString m_linDevice;
};

class QLinTester;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QLinTester* tester);

    ~SettingsDialog();

    operator LinTesterSettings() const;

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
