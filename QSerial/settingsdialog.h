// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QIntValidator>
#include <QSettings>

#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

//class QIntValidator;
QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi{

    Q_OBJECT

public:
    
    static constexpr const char* nameKey = "serial/name";
    static constexpr const char* nameValue = "";
    
    static constexpr const char* bRateKey = "serial/baudRate";
    static constexpr qint32 bRateValue = 9800;
    
    static constexpr const char* dataBitsKey = "serial/dataBits";
    static constexpr QSerialPort::DataBits dataBitsValue = QSerialPort::DataBits::Data5;
    
    static constexpr const char* parityKey = "serial/parity";
    static constexpr QSerialPort::Parity parityValue = QSerialPort::Parity::NoParity;
    
    static constexpr const char* stopBitsKey = "serial/stopBits";
    static constexpr QSerialPort::StopBits stopBitsValue = QSerialPort::StopBits::OneStop;
    
    static constexpr const char* flowControlKey = "serial/flowControl";
    static constexpr QSerialPort::FlowControl flowControlValue = QSerialPort::FlowControl::NoFlowControl;
    
    static constexpr const char* localEchoEnabledKey = "serial/localEchoEnabled";
    static constexpr bool localEchoEnabledValue = true;
    
    static constexpr const char* autoConnectKey = "serial/autoConnect";
    static constexpr bool autoConnectValue = false;
    
    struct SerialSettings{
        SerialSettings()
            : name(nameValue)
            , baudRate(bRateValue)
            , dataBits(dataBitsValue)
            , parity(parityValue)
            , stopBits(stopBitsValue)
            , flowControl(flowControlValue)
            , localEchoEnabled(localEchoEnabledValue)
            , autoConnect(autoConnectValue){}
        
        SerialSettings(const QSettings& settings, const QString& settingsPath) 
            : name(settings.value(settingsPath + "/" + nameKey, nameValue).toString())
            , baudRate(settings.value(settingsPath + "/" + bRateKey, bRateValue).toInt())
            , dataBits(static_cast<QSerialPort::DataBits>(settings.value(settingsPath + "/" + dataBitsKey, dataBitsValue).toInt()))
            , parity(static_cast<QSerialPort::Parity>(settings.value(settingsPath + "/" + parityKey, parityValue).toInt()))
            , stopBits(static_cast<QSerialPort::StopBits>(settings.value(settingsPath + "/" + stopBitsKey, stopBitsValue).toInt()))
            , flowControl(static_cast<QSerialPort::FlowControl>(settings.value(settingsPath + "/" + flowControlKey, flowControlValue).toInt()))
            , localEchoEnabled(settings.value(settingsPath + "/" + localEchoEnabledKey, localEchoEnabledValue).toBool())
            , autoConnect(settings.value(settingsPath + "/" + autoConnectKey, autoConnectValue).toBool()){}
        
        void save(QSettings& settings, const QString& settingsPath) const{
            settings.setValue(settingsPath + "/" + nameKey, name);
            settings.setValue(settingsPath + "/" + bRateKey, baudRate);
            settings.setValue(settingsPath + "/" + dataBitsKey, dataBits);
            settings.setValue(settingsPath + "/" + parityKey, parity);
            settings.setValue(settingsPath + "/" + stopBitsKey, stopBits);
            settings.setValue(settingsPath + "/" + flowControlKey, flowControl);
            settings.setValue(settingsPath + "/" + localEchoEnabledKey, localEchoEnabled);
            settings.setValue(settingsPath + "/" + autoConnectKey, autoConnect);
        }
        
        QString name;
        qint32 baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
        bool localEchoEnabled;
        bool autoConnect;
    };
    
    SettingsDialog(QWidget* mwin, Loader* loader, const QString& settingsPath);
    
    ~SettingsDialog();

    SerialSettings serialSettings() const;

    bool saveSettings() {
        return true;
    }

    QString settingsPath() const {
        return m_settingsPath;
    }

private slots:
    void showPortInfo(int idx);
    void apply();
    void cancel();
    void ok();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

    void settingsChanged();
    
private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();
    void fillFromSettings();
    
private:
    SerialSettings m_currentSettings;
    Ui::SettingsDialog *m_ui = nullptr;
    QIntValidator *m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif // SETTINGSDIALOG_H
