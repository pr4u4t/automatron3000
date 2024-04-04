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

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

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

    struct DataSettings : public PluginSettings {
        DataSettings() 
        : dbDriver(dbDriverValue)
        , dbUri(dbUriValue)
        , dbName(dbNameValue)
        , dbTable(dbTableValue)
        , dbLock(dbLockValue)
        , dbLockPass(dbLockPassValue)
        , serialPrefix(serialPrefixValue)
        , barcodeRegexp(barcodeRegexpValue)
        , serialInterval(serialIntervalValue)
        , cipher(cipherValue)
        , removeChars(removeCharsValue)
        , omitZeros(omitZerosValue)
        , clearCode(clearCodeValue)
        , keepClear(keepClearValue){}

        DataSettings(const QSettings& settings, const QString& settingsPath)
            : dbDriver(settings.value(settingsPath + "/" + dbDriverKey, dbDriverValue).toString())
            , dbUri(settings.value(settingsPath + "/" + dbUriKey, dbUriValue).toString())
            , dbName(settings.value(settingsPath + "/" + dbNameKey, dbNameValue).toString())
            , dbTable(settings.value(settingsPath + "/" + dbTableKey, dbTableValue).toString())
            , dbLock(settings.value(settingsPath + "/" + dbLockKey, dbLockValue).toBool())
            , dbLockPass(settings.value(settingsPath + "/" + dbLockPassKey, dbLockPassValue).toByteArray())
            , serialPrefix(settings.value(settingsPath + "/" + serialPrefixKey, serialPrefixValue).toString())
            , barcodeRegexp(settings.value(settingsPath + "/" + barcodeRegexpKey, barcodeRegexpValue).toString())
            , serialInterval(settings.value(settingsPath + "/" + serialIntervalKey, serialIntervalValue).toInt())
            , cipher(settings.value(settingsPath + "/" + cipherKey, cipherValue).toULongLong())
            , removeChars(settings.value(settingsPath + "/" + removeCharsKey, removeCharsValue).toString())
            , omitZeros(settings.value(settingsPath + "/" + omitZerosKey, omitZerosValue).toBool())
            , clearCode(settings.value(settingsPath + "/" + clearCodeKey, clearCodeValue).toInt())
            , keepClear(settings.value(settingsPath + "/" + keepClearKey, keepClearValue).toBool()){

            if (cipher == 0) {
                cipher = QRandomGenerator::global()->generate64();
                const_cast<QSettings&>(settings).setValue(settingsPath + "/" + cipherKey, cipher);
            }

            if (dbLockPass.isEmpty() == false) {    
                SimpleCrypt crypto(cipher);
                dbLockPass = crypto.decryptToByteArray(dbLockPass);
            }
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            QByteArray dbLockPass_e;

            if (dbLockPass.isEmpty() == false) {
                SimpleCrypt crypto(cipher);
                dbLockPass_e = crypto.encryptToByteArray(dbLockPass);
            }

            settings.setValue(settingsPath + "/" + dbDriverKey, dbDriver);
            settings.setValue(settingsPath + "/" + dbUriKey, dbUri);
            settings.setValue(settingsPath + "/" + dbNameKey, dbName);
            settings.setValue(settingsPath + "/" + dbTableKey, dbTable);
            settings.setValue(settingsPath + "/" + dbLockKey, dbLock);
            settings.setValue(settingsPath + "/" + dbLockPassKey, dbLockPass_e);
            settings.setValue(settingsPath + "/" + serialPrefixKey, serialPrefix);
            settings.setValue(settingsPath + "/" + barcodeRegexpKey, barcodeRegexp);
            settings.setValue(settingsPath + "/" + serialIntervalKey, serialInterval);
            settings.setValue(settingsPath + "/" + cipherKey, cipher);
            settings.setValue(settingsPath + "/" + removeCharsKey, removeChars);
            settings.setValue(settingsPath + "/" + omitZerosKey, omitZeros);
            settings.setValue(settingsPath + "/" + clearCodeKey, clearCode);
            settings.setValue(settingsPath + "/" + keepClearKey, keepClear);
        }

        QString dbDriver;
        QString dbUri;
        QString dbName;
        QString dbTable;
        bool dbLock;
        QByteArray dbLockPass;
        QString serialPrefix;
        QString barcodeRegexp;
        int serialInterval;
        quint64 cipher;
        QString removeChars;
        bool omitZeros;
        int clearCode;
        bool keepClear;
    };

    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    DataSettings dataSettings() const;

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
    DataSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif // SETTINGSDIALOG_H
