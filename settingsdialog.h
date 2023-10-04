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
    
    static constexpr const char* sbRateKey = "serial/stringBaudRate";
    static constexpr const char* sbRateValue = "9800";
    
    static constexpr const char* dataBitsKey = "serial/dataBits";
    static constexpr QSerialPort::DataBits dataBitsValue = QSerialPort::DataBits::Data5;
    
    static constexpr const char* parityKey = "serial/parity";
    static constexpr QSerialPort::Parity parityValue = QSerialPort::Parity::NoParity;
    
    static constexpr const char* strParityKey = "serial/strParity";
    static constexpr const char* strParityValue = "NoParity";
    
    static constexpr const char* stopBitsKey = "serial/stopBits";
    static constexpr QSerialPort::StopBits stopBitsValue = QSerialPort::StopBits::OneStop;
    
    static constexpr const char* strStopBitsKey = "serial/strStopBits";
    static constexpr const char* strStopBitsValue = "OneStop";
    
    static constexpr const char* flowControlKey = "serial/flowControl";
    static constexpr QSerialPort::FlowControl flowControlValue = QSerialPort::FlowControl::NoFlowControl;
    
    static constexpr const char* strFlowControlKey = "serial/strFlowControl";
    static constexpr const char* strFlowControlValue = "NoFlowControl";
    
    static constexpr const char* localEchoEnabledKey = "serial/localEchoEnabled";
    static constexpr bool localEchoEnabledValue = true;
    
    struct SerialSettings{
        SerialSettings(){
            name = nameValue;
            baudRate = bRateValue;
            stringBaudRate = sbRateValue;
            dataBits = dataBitsValue;
            parity = parityValue;
            stringParity = strParityValue;
            stopBits = stopBitsValue;
            stringStopBits = strStopBitsValue;
            flowControl = flowControlValue;
            stringFlowControl = strFlowControlValue;
            localEchoEnabled = localEchoEnabledValue;
        }
        
        SerialSettings(const QSettings& settings){
            name = settings.value(nameKey, nameValue).toString();
            baudRate = settings.value(bRateKey, bRateValue).toInt();
            stringBaudRate = settings.value(sbRateKey, sbRateValue).toString();
            dataBits = static_cast<QSerialPort::DataBits>(settings.value(dataBitsKey, dataBitsValue).toInt());
            parity = static_cast<QSerialPort::Parity>(settings.value(parityKey, parityValue).toInt());
            stringParity = settings.value(strParityKey, strParityValue).toString();
            stopBits = static_cast<QSerialPort::StopBits>(settings.value(stopBitsKey, stopBitsValue).toInt());
            stringStopBits = settings.value(strStopBitsKey, strStopBitsValue).toString();
            flowControl = static_cast<QSerialPort::FlowControl>(settings.value(flowControlKey, flowControlValue).toInt());
            stringFlowControl = settings.value(strFlowControlKey, strFlowControlValue).toString();
            localEchoEnabled = settings.value(localEchoEnabledKey, localEchoEnabledValue).toBool();
        }
        
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    
    Q_INVOKABLE SettingsDialog(QWidget* parent = nullptr, QWidget* mwin = nullptr);
    
    ~SettingsDialog();

    SerialSettings settings() const;

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
    
private:
    SerialSettings m_currentSettings;
    Ui::SettingsDialog *m_ui = nullptr;
    QIntValidator *m_intValidator = nullptr;
};

#endif // SETTINGSDIALOG_H
