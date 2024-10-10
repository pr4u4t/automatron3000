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


    static constexpr const char* const frameDataKey = "frameData";
    static constexpr const char* const intervalKey = "Interval";
    static constexpr const char* const triesKey = "tries";
    static constexpr const char* const titleKey = "title";
    static constexpr const char* const buttonTextKey = "buttonText";
    static constexpr const char* const previousKey = "previous";
    static constexpr const char* const linDeviceKey = "linDevice";

    static constexpr const char* const frameDataValue = "0x6522f18c";
    static constexpr const qint64 intervalValue = 0;
    static constexpr const uint8_t triesValue = 3;
    static constexpr const char* const titleValue = "Command";
    static constexpr const char* const buttonTextValue = "Execute";
    static constexpr const char* const previousValue = nullptr;
    static constexpr const char* const linDeviceValue = nullptr;

    struct LinCommandSettings : public PluginSettings {
        LinCommandSettings()
            : frameData(frameDataValue)
            , interval(intervalValue)
            , tries(triesValue)
            , title(titleValue)
            , buttonText(buttonTextValue)
            , previous(previousValue)
            , linDevice(linDeviceValue){
        }

        LinCommandSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , frameData(settings.value(settingsPath + "/" + frameDataKey, frameDataValue).toByteArray())
            , interval(settings.value(settingsPath + "/" + intervalKey, intervalValue).toInt())
            , tries(settings.value(settingsPath + "/" + triesKey, triesValue).toUInt())
            , title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
            , buttonText(settings.value(settingsPath + "/" + buttonTextKey, buttonTextValue).toString())
            , previous(settings.value(settingsPath + "/" + previousKey, previousValue).toString())
            , linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString()){
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + frameDataKey, frameData);
            settings.setValue(settingsPath + "/" + intervalKey, interval);
            settings.setValue(settingsPath + "/" + triesKey, tries);
            settings.setValue(settingsPath + "/" + titleKey, title);
            settings.setValue(settingsPath + "/" + buttonTextKey, buttonText);
            settings.setValue(settingsPath + "/" + previousKey, previous);
            settings.setValue(settingsPath + "/" + previousKey, previous);

            PluginSettings::save(settings, settingsPath);
        }

        QByteArray frameData;
        qint64 interval;
        uint8_t tries;
        QString title;
        QString buttonText;
        QString previous;
        QString linDevice;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    LinCommandSettings lincommandSettings() const;

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
    LinCommandSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif
