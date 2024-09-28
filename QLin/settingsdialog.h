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

struct ChannelConfig;

QDataStream& operator<<(QDataStream& stream, const ChannelConfig& object);

QDataStream& operator>>(QDataStream& stream, ChannelConfig& object);

bool operator==(const ChannelConfig& lhs, const ChannelConfig& rhs);

bool operator==(const XLchannelConfig& lhs, const ChannelConfig& rhs);

struct ChannelConfig {
    decltype(static_cast<XL_CHANNEL_CONFIG*>(nullptr)->name) name;
    int serialNumber;
    int articleNumber;
    unsigned int hwType;
    unsigned int hwIndex;
    unsigned int hwChannel;
    unsigned int appChannel;
    int channelIndex;
    unsigned int busType = XL_BUS_TYPE_LIN;
    
    operator QByteArray() const {
        return toByteArray();
    }

    QByteArray toByteArray() const {
        QByteArray ret;
        QDataStream out(&ret, QIODeviceBase::WriteOnly);
        out << *this;
        return ret;
    }

    ChannelConfig(const QByteArray& arr) 
        : name{ 0 }
        , serialNumber(0)
        , articleNumber(0)
        , hwType(0)
        , hwIndex(0)
        , hwChannel(0)
        , appChannel(0)
        , channelIndex(0)
        , busType(XL_BUS_TYPE_LIN) {
        QDataStream in(arr);
        in >> *this;
    }

    ChannelConfig()
        : name{ 0 }
        , serialNumber(0)
        , articleNumber(0)
        , hwType(0)
        , hwIndex(0)
        , hwChannel(0)
        , appChannel(0)
        , channelIndex(0)
        , busType(XL_BUS_TYPE_LIN){
    }    
    
    ChannelConfig(const XLchannelConfig& other)
        : name{0} {
        *this = other;
    }

    constexpr ChannelConfig(int serial, int article = 0) 
        : serialNumber(serial)
        , articleNumber(article)
        , name{0}
        , hwType(0)
        , hwIndex(0)
        , hwChannel(0)
        , appChannel(0)
        , channelIndex(0){}

    void operator=(const XLchannelConfig& conf) {
        articleNumber = conf.articleNumber;
        serialNumber = conf.serialNumber;
        strncpy(name, conf.name, strlen(conf.name));
        hwType = conf.hwType;
        hwIndex = conf.hwIndex;
        hwChannel = conf.hwChannel;
        appChannel = 1;
        channelIndex = conf.channelIndex;
    }
};

//Q_DECLARE_METATYPE(ChannelConfig)


class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    struct LinSettings : public PluginSettings {
        enum Mode {
            MASTER,
            SLAVE,
            MASTER_WITH_SLAVE
        };

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
        static constexpr const char* appNameKey = "appName";
        static constexpr const char* appNameValue = "QLin";
        static constexpr const char* channelKey = "hwChannel";
        static constexpr const ChannelConfig channelValue = {0};
        static constexpr const char* queueSizeKey = "QueueSize";
        static constexpr const int queueSizeValue = 256;
        static constexpr const char* checksumMethodKey = "lin/checksum";
        static constexpr const int checksumMethodValue = XL_LIN_CALC_CHECKSUM;

        LinSettings()
            : PluginSettings()
            , linVersion(linVersionValue)
            , slaveID(slaveIDValue)
            , mode(modeValue)
            , initialData((const char*)&initialDataValue[0], sizeof(initialDataValue))
            , autoConnect(autoConnectValue)
            , baudrate(baudrateValue)
            , slaveDLC(slaveDlcValue)
            , dlc(dlcValue, 64)
            , appName(appNameValue)
            , hwChannel(channelValue)
            , queueSize(queueSizeValue)
            , checksumMethod(checksumMethodValue){
        }

        LinSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , linVersion(settings.value(settingsPath + "/" + linVersionKey, linVersionValue).toUInt())
            , slaveID(settings.value(settingsPath + "/" + slaveIDKey, slaveIDValue).toInt())
            , mode(static_cast<Mode>(settings.value(settingsPath + "/" + modeKey, modeValue).toInt()))
            , initialData(settings.value(settingsPath + "/" + initialDataKey, QByteArray((const char*)&initialDataValue[0], sizeof(initialDataValue))).toByteArray())
            , autoConnect(settings.value(settingsPath + "/" + autoConnectKey, autoConnectValue).toBool())
            , baudrate(settings.value(settingsPath + "/" + baudrateKey, baudrateValue).toInt())
            , slaveDLC(settings.value(settingsPath + "/" + slaveDlcKey, slaveDlcValue).toInt())
            , dlc(settings.value(settingsPath + "/" + dlcKey, QByteArray(dlcValue, 64)).toByteArray())
            , appName(settings.value(settingsPath + "/" + appNameKey, appNameValue).toString())
            , hwChannel(settings.value(settingsPath + "/" + channelKey, channelValue.toByteArray()).toByteArray())
            , queueSize(settings.value(settingsPath + "/" + queueSizeKey, queueSizeValue).toInt())
            , checksumMethod(settings.value(settingsPath + "/" + checksumMethodKey, checksumMethodValue).toInt()){
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + linVersionKey, linVersion);
            settings.setValue(settingsPath + "/" + slaveIDKey, slaveID);
            settings.setValue(settingsPath + "/" + modeKey, mode);
            settings.setValue(settingsPath + "/" + initialDataKey, initialData);
            settings.setValue(settingsPath + "/" + autoConnectKey, autoConnect);
            settings.setValue(settingsPath + "/" + baudrateKey, baudrate);
            settings.setValue(settingsPath + "/" + slaveDlcKey, slaveDLC);
            settings.setValue(settingsPath + "/" + dlcKey, dlc);
            settings.setValue(settingsPath + "/" + appNameKey, appName);
            settings.setValue(settingsPath + "/" + channelKey, hwChannel.toByteArray());
            settings.setValue(settingsPath + "/" + queueSizeKey, queueSize);
            settings.setValue(settingsPath + "/" + checksumMethodKey, checksumMethod);

            PluginSettings::save(settings, settingsPath);
        }

        unsigned int linVersion;
        unsigned char slaveID;
        Mode mode;
        QByteArray initialData;
        bool autoConnect;
        int baudrate;
        char slaveDLC;
        QByteArray dlc;
        QString appName;
        ChannelConfig hwChannel;
        int queueSize;
        int checksumMethod;
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
    void vectorConfig() {
        xlPopupHwConfig(nullptr, 0);
    }

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
