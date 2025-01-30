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
    static constexpr const char* const promptKey = "prompt";
    static constexpr const char* const localEchoKey = "localEcho";
    static constexpr const char* const portKey = "port";
    
    static constexpr const char* const promptValue = "-> ";
    static constexpr const bool localEchoValue = true;
    static constexpr const char* const portValue = nullptr;

    Q_GADGET
    Q_PROPERTY(QString prompt READ prompt WRITE setPrompt)
    Q_PROPERTY(bool localEcho READ localEcho WRITE setLocalEcho)
    Q_PROPERTY(QString port READ port WRITE setPort)

public:

    KonsoleSettings()
        : m_prompt(promptValue)
        , m_localEcho(localEchoValue)
        , m_port(portValue){
        registerMetaObject(&staticMetaObject);
    }

    KonsoleSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_prompt(settings.value(settingsPath + "/" + promptKey, promptValue).toString())
        , m_localEcho(settings.value(settingsPath + "/" + localEchoKey, localEchoValue).toBool())
        , m_port(settings.value(settingsPath + "/" + portKey, portValue).toString()){
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + promptKey, m_prompt);
        settings.setValue(settingsPath + "/" + localEchoKey, m_localEcho);
        settings.setValue(settingsPath + "/" + portKey, m_port);

        PluginSettings::save(settings, settingsPath);
    }

    QString prompt() const { return m_prompt; }
    void setPrompt(const QString& prompt) { m_prompt = prompt; }

    // Getter and Setter for m_localEcho
    bool localEcho() const { return m_localEcho; }
    void setLocalEcho(bool localEcho) { m_localEcho = localEcho; }

    QString port() const { return m_port; }
    void setPort(const QString& port) { m_port = port; }

    bool operator==(const KonsoleSettings& other) const {
        return m_prompt == other.m_prompt &&
            m_localEcho == other.m_localEcho &&
            m_port == other.m_port;
    }

    bool operator!=(const KonsoleSettings& other) const {
        return !(*this == other);
    }

private:
    QString m_prompt;
    bool m_localEcho;
    QString m_port;
};

class QKonsole;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:
 
    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QKonsole* konsole);

    ~SettingsDialog();

    operator KonsoleSettings() const;

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

