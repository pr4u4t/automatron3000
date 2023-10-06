// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QIntValidator>
#include <QSettings>

#include "mdichild.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

//class QIntValidator;
QT_END_NAMESPACE

class SettingsDialog : public MdiChild{

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
        SerialSettings(){
            name = nameValue;
            baudRate = bRateValue;
            dataBits = dataBitsValue;
            parity = parityValue;
            stopBits = stopBitsValue;
            flowControl = flowControlValue;
            localEchoEnabled = localEchoEnabledValue;
            autoConnect = autoConnectValue;
        }
        
        SerialSettings(const QSettings& settings){
            name = settings.value(nameKey, nameValue).toString();
            baudRate = settings.value(bRateKey, bRateValue).toInt();
            dataBits = static_cast<QSerialPort::DataBits>(settings.value(dataBitsKey, dataBitsValue).toInt());
            parity = static_cast<QSerialPort::Parity>(settings.value(parityKey, parityValue).toInt());
            stopBits = static_cast<QSerialPort::StopBits>(settings.value(stopBitsKey, stopBitsValue).toInt());
            flowControl = static_cast<QSerialPort::FlowControl>(settings.value(flowControlKey, flowControlValue).toInt());
            localEchoEnabled = settings.value(localEchoEnabledKey, localEchoEnabledValue).toBool();
            autoConnect = settings.value(autoConnectKey, autoConnectValue).toBool(); 
        }
        
        void save(QSettings* settings) const{
            settings->setValue(nameKey, name);
            settings->setValue(bRateKey, baudRate);
            settings->setValue(dataBitsKey, dataBits);
            settings->setValue(parityKey, parity);
            settings->setValue(stopBitsKey, stopBits);
            settings->setValue(flowControlKey, flowControl);
            settings->setValue(localEchoEnabledKey, localEchoEnabled);
            settings->setValue(autoConnectKey, autoConnect);
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
    
    Q_INVOKABLE SettingsDialog(QWidget* parent = nullptr, QWidget* mwin = nullptr);
    
    ~SettingsDialog();

    SerialSettings serialSettings() const;

private slots:
    void showPortInfo(int idx);
    void apply();
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
};

#endif // SETTINGSDIALOG_H
