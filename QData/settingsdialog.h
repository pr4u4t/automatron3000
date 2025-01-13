#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>

#include "../api/api.h"
#include "simplecrypt.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

struct DataSettings : public PluginSettings {

    static constexpr const char* dbDriverKey = "dbDriver";
    static constexpr const char* dbUriKey = "dbUri";
    static constexpr const char* dbNameKey = "dbName";
    static constexpr const char* dbTableKey = "dbTable";
    static constexpr const char* dbLockKey = "dbLock";
    static constexpr const char* dbLockPassKey = "dbLockPass";
    static constexpr const char* serialPrefixKey = "serialPrefix";
    static constexpr const char* barcodeRegexpKey = "barcodeRegex";
    static constexpr const char* serialIntervalKey = "serialInterval";
    static constexpr const char* cipherKey = "cipher";
    static constexpr const char* removeCharsKey = "removeChars";
    static constexpr const char* omitZerosKey = "omitZeros";
    static constexpr const char* clearCodeKey = "clearCode";
    static constexpr const char* keepClearKey = "keepClear";

    static constexpr const char* dbDriverValue = "QPSQL";
    static constexpr const char* dbUriValue = "szpuler.dat";
    static constexpr const char* dbNameValue = "szpuler";
    static constexpr const char* dbTableValue = "part_shelf";
    static constexpr const char* dbLockValue = "No";
    static constexpr const char* dbLockPassValue = "";
    static constexpr const char* serialPrefixValue = "9999";
    static constexpr const char* barcodeRegexpValue = "^[0-9]+[\\\\\\/]([0-9\\.]+).+$";
    static constexpr const int serialIntervalValue = -1;
    static constexpr const int cipherValue = 0;
    static constexpr const char* removeCharsValue = ",./\\";
    static constexpr const bool omitZerosValue = true;
    static constexpr const int clearCodeValue = -1;
    static constexpr const bool keepClearValue = false;

    Q_GADGET
        
    Q_PROPERTY(QString dbDriver READ dbDriver WRITE setDbDriver)
    Q_PROPERTY(QString dbUri READ dbUri WRITE setDbUri)
    Q_PROPERTY(QString dbName READ dbName WRITE setDbName)
    Q_PROPERTY(QString dbTable READ dbTable WRITE setDbTable)
    Q_PROPERTY(bool dbLock READ dbLock WRITE setDbLock)
    Q_PROPERTY(QByteArray dbLockPass READ dbLockPass WRITE setDbLockPass)
    Q_PROPERTY(QString serialPrefix READ serialPrefix WRITE setSerialPrefix)
    Q_PROPERTY(QString barcodeRegexp READ barcodeRegexp WRITE setBarcodeRegexp)
    Q_PROPERTY(int serialInterval READ serialInterval WRITE setSerialInterval)
    Q_PROPERTY(quint64 cipher READ cipher WRITE setCipher)
    Q_PROPERTY(QString removeChars READ removeChars WRITE setRemoveChars)
    Q_PROPERTY(bool omitZeros READ omitZeros WRITE setOmitZeros)
    Q_PROPERTY(int clearCode READ clearCode WRITE setClearCode)
    Q_PROPERTY(bool keepClear READ keepClear WRITE setKeepClear)


public:

    DataSettings()
        : m_dbDriver(dbDriverValue)
        , m_dbUri(dbUriValue)
        , m_dbName(dbNameValue)
        , m_dbTable(dbTableValue)
        , m_dbLock(dbLockValue)
        , m_dbLockPass(dbLockPassValue)
        , m_serialPrefix(serialPrefixValue)
        , m_barcodeRegexp(barcodeRegexpValue)
        , m_serialInterval(serialIntervalValue)
        , m_cipher(cipherValue)
        , m_removeChars(removeCharsValue)
        , m_omitZeros(omitZerosValue)
        , m_clearCode(clearCodeValue)
        , m_keepClear(keepClearValue) {
        registerMetaObject(&staticMetaObject);
    }

    DataSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_dbDriver(settings.value(settingsPath + "/" + dbDriverKey, dbDriverValue).toString())
        , m_dbUri(settings.value(settingsPath + "/" + dbUriKey, dbUriValue).toString())
        , m_dbName(settings.value(settingsPath + "/" + dbNameKey, dbNameValue).toString())
        , m_dbTable(settings.value(settingsPath + "/" + dbTableKey, dbTableValue).toString())
        , m_dbLock(settings.value(settingsPath + "/" + dbLockKey, dbLockValue).toBool())
        , m_dbLockPass(settings.value(settingsPath + "/" + dbLockPassKey, dbLockPassValue).toByteArray())
        , m_serialPrefix(settings.value(settingsPath + "/" + serialPrefixKey, serialPrefixValue).toString())
        , m_barcodeRegexp(settings.value(settingsPath + "/" + barcodeRegexpKey, barcodeRegexpValue).toString())
        , m_serialInterval(settings.value(settingsPath + "/" + serialIntervalKey, serialIntervalValue).toInt())
        , m_cipher(settings.value(settingsPath + "/" + cipherKey, cipherValue).toULongLong())
        , m_removeChars(settings.value(settingsPath + "/" + removeCharsKey, removeCharsValue).toString())
        , m_omitZeros(settings.value(settingsPath + "/" + omitZerosKey, omitZerosValue).toBool())
        , m_clearCode(settings.value(settingsPath + "/" + clearCodeKey, clearCodeValue).toInt())
        , m_keepClear(settings.value(settingsPath + "/" + keepClearKey, keepClearValue).toBool()) {
        registerMetaObject(&staticMetaObject);
        if (m_cipher == 0) {
            m_cipher = QRandomGenerator::global()->generate64();
            const_cast<QSettings&>(settings).setValue(settingsPath + "/" + cipherKey, m_cipher);
        }

        if (m_dbLockPass.isEmpty() == false) {
            SimpleCrypt crypto(m_cipher);
            m_dbLockPass = crypto.decryptToByteArray(m_dbLockPass);
        }
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        QByteArray dbLockPass_e;

        if (m_dbLockPass.isEmpty() == false) {
            SimpleCrypt crypto(m_cipher);
            dbLockPass_e = crypto.encryptToByteArray(m_dbLockPass);
        }

        settings.setValue(settingsPath + "/" + dbDriverKey, m_dbDriver);
        settings.setValue(settingsPath + "/" + dbUriKey, m_dbUri);
        settings.setValue(settingsPath + "/" + dbNameKey, m_dbName);
        settings.setValue(settingsPath + "/" + dbTableKey, m_dbTable);
        settings.setValue(settingsPath + "/" + dbLockKey, m_dbLock);
        settings.setValue(settingsPath + "/" + dbLockPassKey, m_dbLockPass);
        settings.setValue(settingsPath + "/" + serialPrefixKey, m_serialPrefix);
        settings.setValue(settingsPath + "/" + barcodeRegexpKey, m_barcodeRegexp);
        settings.setValue(settingsPath + "/" + serialIntervalKey, m_serialInterval);
        settings.setValue(settingsPath + "/" + cipherKey, m_cipher);
        settings.setValue(settingsPath + "/" + removeCharsKey, m_removeChars);
        settings.setValue(settingsPath + "/" + omitZerosKey, m_omitZeros);
        settings.setValue(settingsPath + "/" + clearCodeKey, m_clearCode);
        settings.setValue(settingsPath + "/" + keepClearKey, m_keepClear);

        PluginSettings::save(settings, settingsPath);
    }

    // Getter and Setter for m_dbDriver
    QString dbDriver() const { 
        return m_dbDriver; 
    }
    
    void setDbDriver(const QString& dbDriver) { 
        m_dbDriver = dbDriver; 
    }

    // Getter and Setter for m_dbUri
    QString dbUri() const { 
        return m_dbUri; 
    }
    
    void setDbUri(const QString& dbUri) { 
        m_dbUri = dbUri; 
    }

    // Getter and Setter for m_dbName
    QString dbName() const { 
        return m_dbName; 
    }
    
    void setDbName(const QString& dbName) { 
        m_dbName = dbName; 
    }

    // Getter and Setter for m_dbTable
    QString dbTable() const { 
        return m_dbTable; 
    }
    
    void setDbTable(const QString& dbTable) { 
        m_dbTable = dbTable; 
    }

    // Getter and Setter for m_dbLock
    bool dbLock() const { 
        return m_dbLock; 
    }
    
    void setDbLock(bool dbLock) { 
        m_dbLock = dbLock;
    }

    // Getter and Setter for m_dbLockPass
    QByteArray dbLockPass() const { 
        return m_dbLockPass;
    }
    
    void setDbLockPass(const QByteArray& dbLockPass) { 
        m_dbLockPass = dbLockPass; 
    }

    // Getter and Setter for m_serialPrefix
    QString serialPrefix() const { 
        return m_serialPrefix; 
    }
    
    void setSerialPrefix(const QString& serialPrefix) { 
        m_serialPrefix = serialPrefix; 
    }

    // Getter and Setter for m_barcodeRegexp
    QString barcodeRegexp() const { 
        return m_barcodeRegexp; 
    }
    
    void setBarcodeRegexp(const QString& barcodeRegexp) { 
        m_barcodeRegexp = barcodeRegexp; 
    }

    // Getter and Setter for m_serialInterval
    int serialInterval() const { 
        return m_serialInterval; 
    }

    void setSerialInterval(int serialInterval) { 
        m_serialInterval = serialInterval; 
    }

    // Getter and Setter for m_cipher
    quint64 cipher() const { 
        return m_cipher; 
    }
    
    void setCipher(quint64 cipher) { 
        m_cipher = cipher; 
    }

    // Getter and Setter for m_removeChars
    QString removeChars() const { 
        return m_removeChars;
    }
    
    void setRemoveChars(const QString& removeChars) { 
        m_removeChars = removeChars;
    }

    // Getter and Setter for m_omitZeros
    bool omitZeros() const { 
        return m_omitZeros; 
    }

    void setOmitZeros(bool omitZeros) { 
        m_omitZeros = omitZeros; 
    }

    // Getter and Setter for m_clearCode
    int clearCode() const { 
        return m_clearCode; 
    }
    void setClearCode(int clearCode) { 
        m_clearCode = clearCode; 
    }

    // Getter and Setter for m_keepClear
    bool keepClear() const { 
        return m_keepClear; 
    }
    void setKeepClear(bool keepClear) { 
        m_keepClear = keepClear; 
    }

    bool operator==(const DataSettings& other) const {
        return m_dbDriver == other.m_dbDriver &&
            m_dbUri == other.m_dbUri &&
            m_dbName == other.m_dbName &&
            m_dbTable == other.m_dbTable &&
            m_dbLock == other.m_dbLock &&
            m_dbLockPass == other.m_dbLockPass &&
            m_serialPrefix == other.m_serialPrefix &&
            m_barcodeRegexp == other.m_barcodeRegexp &&
            m_serialInterval == other.m_serialInterval &&
            m_cipher == other.m_cipher &&
            m_removeChars == other.m_removeChars &&
            m_omitZeros == other.m_omitZeros &&
            m_clearCode == other.m_clearCode &&
            m_keepClear == other.m_keepClear;
    }

    // Inequality operator (optional)
    bool operator!=(const DataSettings& other) const {
        return !(*this == other);
    }

private:
    QString m_dbDriver;
    QString m_dbUri;
    QString m_dbName;
    QString m_dbTable;
    bool m_dbLock;
    QByteArray m_dbLockPass;
    QString m_serialPrefix;
    QString m_barcodeRegexp;
    int m_serialInterval;
    quint64 m_cipher;
    QString m_removeChars;
    bool m_omitZeros;
    int m_clearCode;
    bool m_keepClear;
};

class QData;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QData* data);

    ~SettingsDialog();

    operator DataSettings() const;

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
