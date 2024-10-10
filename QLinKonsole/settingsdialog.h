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

    static constexpr const char* const promptKey = "prompt";
    static constexpr const char* const localEchoKey = "localEcho";
    static constexpr const char* const commandDelayKey = "commandDelay";
    static constexpr const char* const linDeviceKey = "linDevice";

    static constexpr const char* const promptValue = "-> ";
    static constexpr const bool localEchoValue = true;
    static constexpr const int commandDelayValue = 0;
    static constexpr const char* const linDeviceValue = nullptr;

    struct KonsoleSettings : public PluginSettings {
        KonsoleSettings()
            : prompt(promptValue)
            , localEcho(localEchoValue)
            , commandDelay(commandDelayValue)
            , linDevice(linDeviceValue){}

        KonsoleSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , prompt(settings.value(settingsPath + "/" + promptKey, promptValue).toString())
            , localEcho(settings.value(settingsPath + "/" + localEchoKey, localEchoValue).toBool())
            , commandDelay(settings.value(settingsPath + "/" + commandDelayKey, commandDelayValue).toInt())
            , linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString()){}

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + promptKey, prompt);
            settings.setValue(settingsPath + "/" + localEchoKey, localEcho);
            settings.setValue(settingsPath + "/" + commandDelayKey, commandDelay);
            settings.setValue(settingsPath + "/" + linDeviceKey, linDevice);

            PluginSettings::save(settings, settingsPath);
        }

        QString prompt;
        bool localEcho;
        int commandDelay;
        QString linDevice;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    KonsoleSettings konsoleSettings() const;

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
    KonsoleSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif // SETTINGSDIALOG_H

