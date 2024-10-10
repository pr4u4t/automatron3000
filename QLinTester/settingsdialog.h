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

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:


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

    struct LinTesterSettings : public PluginSettings {
        LinTesterSettings()
            : testStartID(testStartIDValue)
            , testStopID(testStopIDValue)
            , testInterval(testIntervalValue)
            , tries(triesValue)
            , linDevice(linDeviceValue){
        }

        LinTesterSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , testStartID(settings.value(settingsPath + "/" + testStartIDKey, testStartIDValue).toUInt())
            , testStopID(settings.value(settingsPath + "/" + testStopIDKey, testStopIDValue).toUInt())
            , testInterval(settings.value(settingsPath + "/" + testIntervalKey, testIntervalValue).toInt())
            , tries(settings.value(settingsPath + "/" + triesKey, triesValue).toInt())
            , linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString()) {
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + testStartIDKey, testStartID);
            settings.setValue(settingsPath + "/" + testStopIDKey, testStopID);
            settings.setValue(settingsPath + "/" + testIntervalKey, testInterval);
            settings.setValue(settingsPath + "/" + triesKey, tries);
            settings.setValue(settingsPath + "/" + linDeviceKey, linDevice);

            PluginSettings::save(settings, settingsPath);
        }

        uint8_t testStartID = 0;
        uint8_t testStopID = 0;
        qint64 testInterval = 0;
        uint8_t tries = 0;
        QString linDevice;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    LinTesterSettings lintesterSettings() const;

    QString settingsPath() const {
        return m_settingsPath;
    }

private slots:
    void ok();
    void apply();
    void cancel();

private:
    void updateSettings();
    void fillFromSettings();

private:
    LinTesterSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif
