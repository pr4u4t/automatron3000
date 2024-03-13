#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>

#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

//class QIntValidator;
QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    static constexpr const char* defaultSlaveIDKey = "lin/defaultSlaveID";
    static constexpr QChar defaultSlaveIDValue = static_cast<char>(0);

    struct LinSettings {
        LinSettings()
            : defaultSlaveID(defaultSlaveIDValue){}

        LinSettings(const QSettings& settings, const QString& settingsPath)
            : defaultSlaveID(settings.value(settingsPath + "/" + defaultSlaveIDKey, defaultSlaveIDValue).toChar()){}

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + defaultSlaveIDKey, defaultSlaveID);
        }

        QChar defaultSlaveID;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    LinSettings linSettings() const;

    bool saveSettings() {
        return true;
    }

    QString settingsPath() const {
        return m_settingsPath;
    }

private slots:
    void apply();
    void cancel();
    void ok();

private:
    void updateSettings();
    void fillFromSettings();

private:
    LinSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif
