#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>

#include "../api/api.h"
#include <Windows.h>
#include "../include/vxlapi.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

//class QIntValidator;
QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    static constexpr const char* masterIDKey = "lin/masterID";
    static constexpr char masterIDValue = 0;
    static constexpr const char* slaveIDKey = "lin/slaveID";
    static constexpr char slaveIDValue = 0;
    static constexpr const char* linVersionKey = "lin/version";
    static constexpr const unsigned int linVersionValue = XL_LIN_VERSION_1_3;
    static constexpr const char* masterKey = "lin/master";
    static constexpr bool masterValue = true;
    static constexpr const char* slaveKey = "lin/slave";
    static constexpr bool slaveValue = false;
    static constexpr const char* initialDataKey = "lin/initialSlaveData";
    static constexpr const char* initialDataValue = "0 0 0 0 0 0 0 0";

    struct LinSettings : public PluginSettings {
        LinSettings()
            : linVersion(linVersionValue)
            , masterID(masterIDValue) 
            , slaveID(slaveIDValue)
            , slave(slaveValue)
            , master(masterValue){
            //setInitialData();
        }

        LinSettings(const QSettings& settings, const QString& settingsPath)
            : linVersion(settings.value(settingsPath + "/" + linVersionKey, linVersionValue).toUInt())
            , masterID(settings.value(settingsPath + "/" + masterIDKey, masterIDValue).toInt())
            , slaveID(settings.value(settingsPath + "/" + slaveIDKey, slaveIDValue).toInt())
            , slave(settings.value(settingsPath + "/" + slaveKey, slaveValue).toBool())
            , master(settings.value(settingsPath + "/" + masterKey, masterValue).toBool()){
            //setInitialData();
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + linVersionKey, linVersion);
            settings.setValue(settingsPath + "/" + masterIDKey, masterID);
            settings.setValue(settingsPath + "/" + slaveIDKey, slaveID);
            settings.setValue(settingsPath + "/" + slaveKey, slave);
            settings.setValue(settingsPath + "/" + masterKey, master);
        }

        //void setInitialData(const char* data) {
        //    QString data = settings.value(settingsPath + "/" + masterKey, masterValue).toString();
        //    QStringList
        //}

        unsigned int linVersion;
        char masterID;
        char slaveID;
        bool slave;
        bool master;
        QByteArray initialData;

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
