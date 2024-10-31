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

struct SerialSettings : public PluginSettings {
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
    
    Q_GADGET

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(qint32 baudRate READ baudRate WRITE setBaudRate)
    Q_PROPERTY(QSerialPort::DataBits dataBits READ dataBits WRITE setDataBits)
    Q_PROPERTY(QSerialPort::Parity parity READ parity WRITE setParity)
    Q_PROPERTY(QSerialPort::StopBits stopBits READ stopBits WRITE setStopBits)
    Q_PROPERTY(QSerialPort::FlowControl flowControl READ flowControl WRITE setFlowControl)
    Q_PROPERTY(bool localEchoEnabled READ localEchoEnabled WRITE setLocalEchoEnabled)
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect)

public:

    SerialSettings()
        : m_name(nameValue)
        , m_baudRate(bRateValue)
        , m_dataBits(dataBitsValue)
        , m_parity(parityValue)
        , m_stopBits(stopBitsValue)
        , m_flowControl(flowControlValue)
        , m_localEchoEnabled(localEchoEnabledValue)
        , m_autoConnect(autoConnectValue) {
        registerMetaObject(&staticMetaObject);
    }

    SerialSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_name(settings.value(settingsPath + "/" + nameKey, nameValue).toString())
        , m_baudRate(settings.value(settingsPath + "/" + bRateKey, bRateValue).toInt())
        , m_dataBits(static_cast<QSerialPort::DataBits>(settings.value(settingsPath + "/" + dataBitsKey, dataBitsValue).toInt()))
        , m_parity(static_cast<QSerialPort::Parity>(settings.value(settingsPath + "/" + parityKey, parityValue).toInt()))
        , m_stopBits(static_cast<QSerialPort::StopBits>(settings.value(settingsPath + "/" + stopBitsKey, stopBitsValue).toInt()))
        , m_flowControl(static_cast<QSerialPort::FlowControl>(settings.value(settingsPath + "/" + flowControlKey, flowControlValue).toInt()))
        , m_localEchoEnabled(settings.value(settingsPath + "/" + localEchoEnabledKey, localEchoEnabledValue).toBool())
        , m_autoConnect(settings.value(settingsPath + "/" + autoConnectKey, autoConnectValue).toBool()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + nameKey, m_name);
        settings.setValue(settingsPath + "/" + bRateKey, m_baudRate);
        settings.setValue(settingsPath + "/" + dataBitsKey, m_dataBits);
        settings.setValue(settingsPath + "/" + parityKey, m_parity);
        settings.setValue(settingsPath + "/" + stopBitsKey, m_stopBits);
        settings.setValue(settingsPath + "/" + flowControlKey, m_flowControl);
        settings.setValue(settingsPath + "/" + localEchoEnabledKey, m_localEchoEnabled);
        settings.setValue(settingsPath + "/" + autoConnectKey, m_autoConnect);

        PluginSettings::save(settings, settingsPath);
    }

    // Getter and Setter for name
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    // Getter and Setter for baudRate
    qint32 baudRate() const { return m_baudRate; }
    void setBaudRate(qint32 baudRate) { m_baudRate = baudRate; }

    // Getter and Setter for dataBits
    QSerialPort::DataBits dataBits() const { return m_dataBits; }
    void setDataBits(QSerialPort::DataBits dataBits) { m_dataBits = dataBits; }

    // Getter and Setter for parity
    QSerialPort::Parity parity() const { return m_parity; }
    void setParity(QSerialPort::Parity parity) { m_parity = parity; }

    // Getter and Setter for stopBits
    QSerialPort::StopBits stopBits() const { return m_stopBits; }
    void setStopBits(QSerialPort::StopBits stopBits) { m_stopBits = stopBits; }

    // Getter and Setter for flowControl
    QSerialPort::FlowControl flowControl() const { return m_flowControl; }
    void setFlowControl(QSerialPort::FlowControl flowControl) { m_flowControl = flowControl; }

    // Getter and Setter for localEchoEnabled
    bool localEchoEnabled() const { return m_localEchoEnabled; }
    void setLocalEchoEnabled(bool localEchoEnabled) { m_localEchoEnabled = localEchoEnabled; }

    // Getter and Setter for autoConnect
    bool autoConnect() const { return m_autoConnect; }
    void setAutoConnect(bool autoConnect) { m_autoConnect = autoConnect; }

private:
    QString m_name;
    qint32 m_baudRate;
    QSerialPort::DataBits m_dataBits;
    QSerialPort::Parity m_parity;
    QSerialPort::StopBits m_stopBits;
    QSerialPort::FlowControl m_flowControl;
    bool m_localEchoEnabled;
    bool m_autoConnect;
};

class SettingsDialog : public SettingsMdi{

    Q_OBJECT

public:
        
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
