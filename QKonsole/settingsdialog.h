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

struct KonsoleSettings : public PluginSettings {
    static constexpr const char* promptKey = "prompt";
    static constexpr const char* localEchoKey = "localEcho";

    static constexpr const char* promptValue = "-> ";
    static constexpr const bool localEchoValue = true;

    Q_GADGET
    Q_PROPERTY(QString prompt READ prompt WRITE setPrompt)
    Q_PROPERTY(bool localEcho READ localEcho WRITE setLocalEcho)

public:

    KonsoleSettings()
        : m_prompt(promptValue)
        , m_localEcho(localEchoValue) {
        registerMetaObject(&staticMetaObject);
    }

    KonsoleSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_prompt(promptValue)
        , m_localEcho(localEchoValue) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + promptKey, m_prompt);
        settings.setValue(settingsPath + "/" + localEchoKey, m_localEcho);

        PluginSettings::save(settings, settingsPath);
    }

    QString prompt() const { return m_prompt; }
    void setPrompt(const QString& prompt) { m_prompt = prompt; }

    // Getter and Setter for m_localEcho
    bool localEcho() const { return m_localEcho; }
    void setLocalEcho(bool localEcho) { m_localEcho = localEcho; }

private:
    QString m_prompt;
    bool m_localEcho;
};

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:
 
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

