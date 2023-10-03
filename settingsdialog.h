// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QIntValidator>

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
    
    struct SerialSettings{
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
