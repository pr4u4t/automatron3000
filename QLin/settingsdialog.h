#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QStandardItemModel>

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

    struct LinSettings : public PluginSettings {
        enum Mode {
            MASTER,
            SLAVE
        };

        static constexpr const char* masterIDKey = "lin/masterID";
        static constexpr char masterIDValue = 0;
        static constexpr const char* slaveIDKey = "lin/slaveID";
        static constexpr char slaveIDValue = 0;
        static constexpr const char* linVersionKey = "lin/version";
        static constexpr const unsigned int linVersionValue = XL_LIN_VERSION_1_3;
        static constexpr const char* modeKey = "lin/mode";
        static constexpr Mode modeValue = Mode::MASTER;
        static constexpr const char* initialDataKey = "lin/initialSlaveData";
        static constexpr const unsigned char initialDataValue[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
        static constexpr const char* autoConnectKey = "lin/autoConnect";
        static constexpr const bool autoConnectValue = false;
        static constexpr const char* baudrateKey = "lin/baudrate";
        static constexpr const int baudrateValue = 16500;
        static constexpr const char* slaveDlcKey = "lin/slaveDLC";
        static constexpr const char slaveDlcValue = 8;
        static constexpr const char* dlcKey = "lin/dlc";
        static constexpr const char dlcValue[64] = { 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8 };

        LinSettings()
            : linVersion(linVersionValue)
            , masterID(masterIDValue) 
            , slaveID(slaveIDValue)
            , mode(modeValue)
            , initialData((const char*)&initialDataValue[0], sizeof(initialDataValue))
            , autoConnect(autoConnectValue)
            , baudrate(baudrateValue)
            , slaveDLC(slaveDlcValue)
            , dlc(dlcValue, 64){
        }

        LinSettings(const QSettings& settings, const QString& settingsPath)
            : linVersion(settings.value(settingsPath + "/" + linVersionKey, linVersionValue).toUInt())
            , masterID(settings.value(settingsPath + "/" + masterIDKey, masterIDValue).toInt())
            , slaveID(settings.value(settingsPath + "/" + slaveIDKey, slaveIDValue).toInt())
            , mode(static_cast<Mode>(settings.value(settingsPath + "/" + modeKey, modeValue).toInt()))
            , initialData(settings.value(settingsPath + "/" + initialDataKey, QByteArray((const char*) &initialDataValue[0], sizeof(initialDataValue))).toByteArray())
            , autoConnect(settings.value(settingsPath + "/" + autoConnectKey, autoConnectValue).toBool())
            , baudrate(settings.value(settingsPath + "/" + baudrateKey, baudrateValue).toInt())
            , slaveDLC(settings.value(settingsPath + "/" + slaveDlcKey, slaveDlcValue).toInt())
            , dlc(settings.value(settingsPath + "/" + dlcKey, QByteArray(dlcValue, 64)).toByteArray()){
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + linVersionKey, linVersion);
            settings.setValue(settingsPath + "/" + masterIDKey, masterID);
            settings.setValue(settingsPath + "/" + slaveIDKey, slaveID);
            settings.setValue(settingsPath + "/" + modeKey, mode);
            settings.setValue(settingsPath + "/" + initialDataKey, initialData);
            settings.setValue(settingsPath + "/" + autoConnectKey, autoConnect);
            settings.setValue(settingsPath + "/" + baudrateKey, baudrate);
            settings.setValue(settingsPath + "/" + slaveDlcKey, slaveDLC);
            settings.setValue(settingsPath + "/" + dlcKey, dlc);
        }

        unsigned int linVersion;
        char masterID;
        char slaveID;
        Mode mode;
        QByteArray initialData;
        bool autoConnect;
        int baudrate;
        char slaveDLC;
        QByteArray dlc;
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
    QStandardItemModel* m_model = nullptr;
    QStandardItemModel* m_sdlcModel = nullptr;
};

#endif
