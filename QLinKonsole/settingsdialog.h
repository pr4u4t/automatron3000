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
    static constexpr const char* const commandDelayKey = "commandDelay";
    static constexpr const char* const linDeviceKey = "linDevice";

    static constexpr const char* const promptValue = "-> ";
    static constexpr const bool localEchoValue = true;
    static constexpr const int commandDelayValue = 0;
    static constexpr const char* const linDeviceValue = nullptr;

    Q_GADGET
    Q_PROPERTY(QString prompt READ prompt WRITE setPrompt)
    Q_PROPERTY(bool localEcho READ localEcho WRITE setLocalEcho)
    Q_PROPERTY(int commandDelay READ commandDelay WRITE setCommandDelay)
    Q_PROPERTY(QString linDevice READ linDevice WRITE setLinDevice)

public:
    KonsoleSettings()
        : m_prompt(promptValue)
        , m_localEcho(localEchoValue)
        , m_commandDelay(commandDelayValue)
        , m_linDevice(linDeviceValue) {
        registerMetaObject(&staticMetaObject);
    }

    KonsoleSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_prompt(settings.value(settingsPath + "/" + promptKey, promptValue).toString())
        , m_localEcho(settings.value(settingsPath + "/" + localEchoKey, localEchoValue).toBool())
        , m_commandDelay(settings.value(settingsPath + "/" + commandDelayKey, commandDelayValue).toInt())
        , m_linDevice(settings.value(settingsPath + "/" + linDeviceKey, linDeviceValue).toString()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + promptKey, m_prompt);
        settings.setValue(settingsPath + "/" + localEchoKey, m_localEcho);
        settings.setValue(settingsPath + "/" + commandDelayKey, m_commandDelay);
        settings.setValue(settingsPath + "/" + linDeviceKey, m_linDevice);

        PluginSettings::save(settings, settingsPath);
    }

    QString prompt() const { return m_prompt; }
    void setPrompt(const QString& prompt) { m_prompt = prompt; }

    // Getter and Setter for localEcho
    bool localEcho() const { return m_localEcho; }
    void setLocalEcho(bool localEcho) { m_localEcho = localEcho; }

    // Getter and Setter for commandDelay
    int commandDelay() const { return m_commandDelay; }
    void setCommandDelay(int commandDelay) { m_commandDelay = commandDelay; }

    // Getter and Setter for linDevice
    QString linDevice() const { return m_linDevice; }
    void setLinDevice(const QString& linDevice) { m_linDevice = linDevice; }

    bool operator==(const KonsoleSettings& other) const {
        return m_prompt == other.m_prompt &&
            m_localEcho == other.m_localEcho &&
            m_commandDelay == other.m_commandDelay &&
            m_linDevice == other.m_linDevice;
    }

    bool operator!=(const KonsoleSettings& other) const {
        return !(*this == other);
    }

private:
    QString m_prompt;
    bool m_localEcho;
    int m_commandDelay;
    QString m_linDevice;
};

class QLinKonsole;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:
    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QLinKonsole* konsole);

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

