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

    
    static constexpr const char* scanStartIDKey = "scanStartID";
    static constexpr const char* scanStopIDKey = "scanStopID";
    static constexpr const char* scanIntervalKey = "scanInterval";
    static constexpr const char* colorsKey = "enableColors";

    static constexpr const uint8_t scanStartIDValue = 0;
    static constexpr const uint8_t scanStopIDValue = 63;
    static constexpr const qint64 scanIntervalValue = 0;
    static constexpr const bool colorsValue = true;
   

    struct LinBusSettings : public PluginSettings {
        LinBusSettings()
            : scanStartID(scanStartIDValue)
            , scanStopID(scanStopIDValue)
            , scanInterval(scanIntervalValue)
            , enableColors(colorsValue){
        }

        LinBusSettings(const QSettings& settings, const QString& settingsPath)
            : scanStartID(settings.value(settingsPath + "/" + scanStartIDKey, scanStartIDValue).toUInt())
            , scanStopID(settings.value(settingsPath + "/" + scanStopIDKey, scanStopIDValue).toUInt())
            , scanInterval(settings.value(settingsPath + "/" + scanIntervalKey, scanIntervalValue).toInt())
            , enableColors(settings.value(settingsPath + "/" + colorsKey, colorsValue).toString() == "true" ? true : false) {
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + scanStartIDKey, scanStartID);
            settings.setValue(settingsPath + "/" + scanStopIDKey, scanStopID);
            settings.setValue(settingsPath + "/" + scanIntervalKey, scanInterval);
            settings.setValue(settingsPath + "/" + colorsKey, enableColors);
        }

        uint8_t scanStartID;
        uint8_t scanStopID;
        qint64 scanInterval;
        bool enableColors;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    LinBusSettings linbusSettings() const;

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
    LinBusSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif // SETTINGSDIALOG_H
