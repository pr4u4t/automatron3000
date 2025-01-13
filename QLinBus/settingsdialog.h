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

struct LinBusSettings : public PluginSettings {
    static constexpr const char* scanStartIDKey = "scanStartID";
    static constexpr const char* scanStopIDKey = "scanStopID";
    static constexpr const char* scanIntervalKey = "scanInterval";
    static constexpr const char* colorsKey = "enableColors";
    static constexpr const char* linDeviceKey = "linDevice";

    static constexpr const uint8_t scanStartIDValue = 0;
    static constexpr const uint8_t scanStopIDValue = 63;
    static constexpr const qint64 scanIntervalValue = 0;
    static constexpr const bool colorsValue = true;
    static constexpr const char* const linDeviceValue = nullptr;

    Q_GADGET
    Q_PROPERTY(uint8_t scanStartID READ scanStartID WRITE setScanStartID)
    Q_PROPERTY(uint8_t scanStopID READ scanStopID WRITE setScanStopID)
    Q_PROPERTY(qint64 scanInterval READ scanInterval WRITE setScanInterval)
    Q_PROPERTY(bool enableColors READ enableColors WRITE setEnableColors)
    Q_PROPERTY(QString linDevice READ linDevice WRITE setLinDevice)

public:
    LinBusSettings()
        : m_scanStartID(scanStartIDValue)
        , m_scanStopID(scanStopIDValue)
        , m_scanInterval(scanIntervalValue)
        , m_enableColors(colorsValue) {

    }

    LinBusSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_scanStartID(settings.value(settingsPath + "/" + scanStartIDKey, scanStartIDValue).toUInt())
        , m_scanStopID(settings.value(settingsPath + "/" + scanStopIDKey, scanStopIDValue).toUInt())
        , m_scanInterval(settings.value(settingsPath + "/" + scanIntervalKey, scanIntervalValue).toInt())
        , m_enableColors(settings.value(settingsPath + "/" + colorsKey, colorsValue).toBool())
        , m_linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString()) {
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + scanStartIDKey, m_scanStartID);
        settings.setValue(settingsPath + "/" + scanStopIDKey, m_scanStopID);
        settings.setValue(settingsPath + "/" + scanIntervalKey, m_scanInterval);
        settings.setValue(settingsPath + "/" + colorsKey, m_enableColors);
        settings.setValue(settingsPath + "/" + linDeviceKey, m_linDevice);

        PluginSettings::save(settings, settingsPath);
    }

    uint8_t scanStartID() const { return m_scanStartID; }
    void setScanStartID(uint8_t scanStartID) { m_scanStartID = scanStartID; }

    // Getter and Setter for scanStopID
    uint8_t scanStopID() const { return m_scanStopID; }
    void setScanStopID(uint8_t scanStopID) { m_scanStopID = scanStopID; }

    // Getter and Setter for scanInterval
    qint64 scanInterval() const { return m_scanInterval; }
    void setScanInterval(qint64 scanInterval) { m_scanInterval = scanInterval; }

    // Getter and Setter for enableColors
    bool enableColors() const { return m_enableColors; }
    void setEnableColors(bool enableColors) { m_enableColors = enableColors; }

    // Getter and Setter for linDevice
    QString linDevice() const { return m_linDevice; }
    void setLinDevice(const QString& linDevice) { m_linDevice = linDevice; }

    bool operator==(const LinBusSettings& other) const {
        return m_scanStartID == other.m_scanStartID &&
            m_scanStopID == other.m_scanStopID &&
            m_scanInterval == other.m_scanInterval &&
            m_enableColors == other.m_enableColors &&
            m_linDevice == other.m_linDevice;
    }

    bool operator!=(const LinBusSettings& other) const {
        return !(*this == other);
    }

private:
    uint8_t m_scanStartID;
    uint8_t m_scanStopID;
    qint64 m_scanInterval;
    bool m_enableColors;
    QString m_linDevice;
};

class QLinBus;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QLinBus* linbus);

    ~SettingsDialog();

    operator LinBusSettings() const;

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

#endif // SETTINGSDIALOG_H
