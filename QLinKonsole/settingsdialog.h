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

    static constexpr const char* promptKey = "prompt";
    static constexpr const char* localEchoKey = "localEcho";

    static constexpr const char* promptValue = "-> ";
    static constexpr const bool localEchoValue = true;

    struct KonsoleSettings : public PluginSettings {
        KonsoleSettings()
            : prompt(promptValue)
            , localEcho(localEchoValue){}

        KonsoleSettings(const QSettings& settings, const QString& settingsPath)
            : prompt(promptValue)
            , localEcho(localEchoValue) {}

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + promptKey, prompt);
            settings.setValue(settingsPath + "/" + localEchoKey, localEcho);
        }

        QString prompt;
        bool localEcho;
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

