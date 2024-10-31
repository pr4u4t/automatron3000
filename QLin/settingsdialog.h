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
    static constexpr const ChannelConfig channelValue = { 0 };
    static constexpr const char* queueSizeKey = "QueueSize";
    static constexpr const int queueSizeValue = 256;
    static constexpr const char* checksumMethodKey = "lin/checksum";
    static constexpr const int checksumMethodValue = XL_LIN_CALC_CHECKSUM;

    Q_GADGET
    Q_PROPERTY(unsigned int linVersion READ linVersion WRITE setLinVersion)
    Q_PROPERTY(unsigned char slaveID READ slaveID WRITE setSlaveID)
    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(QByteArray initialData READ initialData WRITE setInitialData)
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect)
    Q_PROPERTY(int baudrate READ baudrate WRITE setBaudrate)
    Q_PROPERTY(char slaveDLC READ slaveDLC WRITE setSlaveDLC)
    Q_PROPERTY(QByteArray dlc READ dlc WRITE setDlc)
    Q_PROPERTY(QString appName READ appName WRITE setAppName)
    Q_PROPERTY(ChannelConfig hwChannel READ hwChannel WRITE setHwChannel)
    Q_PROPERTY(int queueSize READ queueSize WRITE setQueueSize)
    Q_PROPERTY(int checksumMethod READ checksumMethod WRITE setChecksumMethod)

public:

    LinSettings()
        : PluginSettings()
        , m_linVersion(linVersionValue)
        , m_slaveID(slaveIDValue)
        , m_mode(modeValue)
        , m_initialData((const char*)&initialDataValue[0], sizeof(initialDataValue))
        , m_autoConnect(autoConnectValue)
        , m_baudrate(baudrateValue)
        , m_slaveDLC(slaveDlcValue)
        , m_dlc(dlcValue, 64)
        , m_appName(appNameValue)
        , m_hwChannel(channelValue)
        , m_queueSize(queueSizeValue)
        , m_checksumMethod(checksumMethodValue) {
        registerMetaObject(&staticMetaObject);
    }

    LinSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_linVersion(settings.value(settingsPath + "/" + linVersionKey, linVersionValue).toUInt())
        , m_slaveID(settings.value(settingsPath + "/" + slaveIDKey, slaveIDValue).toInt())
        , m_mode(static_cast<Mode>(settings.value(settingsPath + "/" + modeKey, modeValue).toInt()))
        , m_initialData(settings.value(settingsPath + "/" + initialDataKey, QByteArray((const char*)&initialDataValue[0], sizeof(initialDataValue))).toByteArray())
        , m_autoConnect(settings.value(settingsPath + "/" + autoConnectKey, autoConnectValue).toBool())
        , m_baudrate(settings.value(settingsPath + "/" + baudrateKey, baudrateValue).toInt())
        , m_slaveDLC(settings.value(settingsPath + "/" + slaveDlcKey, slaveDlcValue).toInt())
        , m_dlc(settings.value(settingsPath + "/" + dlcKey, QByteArray(dlcValue, 64)).toByteArray())
        , m_appName(settings.value(settingsPath + "/" + appNameKey, appNameValue).toString())
        , m_hwChannel(settings.value(settingsPath + "/" + channelKey, channelValue.toByteArray()).toByteArray())
        , m_queueSize(settings.value(settingsPath + "/" + queueSizeKey, queueSizeValue).toInt())
        , m_checksumMethod(settings.value(settingsPath + "/" + checksumMethodKey, checksumMethodValue).toInt()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + linVersionKey, m_linVersion);
        settings.setValue(settingsPath + "/" + slaveIDKey, m_slaveID);
        settings.setValue(settingsPath + "/" + modeKey, m_mode);
        settings.setValue(settingsPath + "/" + initialDataKey, m_initialData);
        settings.setValue(settingsPath + "/" + autoConnectKey, m_autoConnect);
        settings.setValue(settingsPath + "/" + baudrateKey, m_baudrate);
        settings.setValue(settingsPath + "/" + slaveDlcKey, m_slaveDLC);
        settings.setValue(settingsPath + "/" + dlcKey, m_dlc);
        settings.setValue(settingsPath + "/" + appNameKey, m_appName);
        settings.setValue(settingsPath + "/" + channelKey, m_hwChannel.toByteArray());
        settings.setValue(settingsPath + "/" + queueSizeKey, m_queueSize);
        settings.setValue(settingsPath + "/" + checksumMethodKey, m_checksumMethod);

        PluginSettings::save(settings, settingsPath);
    }

    // Getter and Setter for linVersion
    unsigned int linVersion() const { return m_linVersion; }
    void setLinVersion(unsigned int linVersion) { m_linVersion = linVersion; }

    // Getter and Setter for slaveID
    unsigned char slaveID() const { return m_slaveID; }
    void setSlaveID(unsigned char slaveID) { m_slaveID = slaveID; }

    // Getter and Setter for mode
    Mode mode() const { return m_mode; }
    void setMode(Mode mode) { m_mode = mode; }

    // Getter and Setter for initialData
    QByteArray initialData() const { return m_initialData; }
    void setInitialData(const QByteArray& initialData) { m_initialData = initialData; }

    // Getter and Setter for autoConnect
    bool autoConnect() const { return m_autoConnect; }
    void setAutoConnect(bool autoConnect) { m_autoConnect = autoConnect; }

    // Getter and Setter for baudrate
    int baudrate() const { return m_baudrate; }
    void setBaudrate(int baudrate) { m_baudrate = baudrate; }

    // Getter and Setter for slaveDLC
    char slaveDLC() const { return m_slaveDLC; }
    void setSlaveDLC(char slaveDLC) { m_slaveDLC = slaveDLC; }

    // Getter and Setter for dlc
    QByteArray dlc() const { return m_dlc; }
    void setDlc(const QByteArray& dlc) { m_dlc = dlc; }

    // Getter and Setter for appName
    QString appName() const { return m_appName; }
    void setAppName(const QString& appName) { m_appName = appName; }

    // Getter and Setter for hwChannel
    ChannelConfig hwChannel() const { return m_hwChannel; }
    void setHwChannel(const ChannelConfig& hwChannel) { m_hwChannel = hwChannel; }

    // Getter and Setter for queueSize
    int queueSize() const { return m_queueSize; }
    void setQueueSize(int queueSize) { m_queueSize = queueSize; }

    // Getter and Setter for checksumMethod
    int checksumMethod() const { return m_checksumMethod; }
    void setChecksumMethod(int checksumMethod) { m_checksumMethod = checksumMethod; }


private:

    unsigned int m_linVersion;
    unsigned char m_slaveID;
    Mode m_mode;
    QByteArray m_initialData;
    bool m_autoConnect;
    int m_baudrate;
    char m_slaveDLC;
    QByteArray m_dlc;
    QString m_appName;
    ChannelConfig m_hwChannel;
    int m_queueSize;
    int m_checksumMethod;
};


class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

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
