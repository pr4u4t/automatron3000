// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QSerial.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath) 
    : SettingsMdi(parent, new SerialSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QSerial* serial)
    : SettingsMdi(parent, new SerialSettings(*(serial->settings<SerialSettings>())), serial->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::~SettingsDialog(){
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::setup() {
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    m_ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(m_ui->okButton, &QPushButton::clicked,
        this, &SettingsDialog::ok);
    connect(m_ui->applyButton, &QPushButton::clicked,
        this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked,
        this, &SettingsDialog::cancel);
    connect(m_ui->serialPortInfoListBox, &QComboBox::currentIndexChanged,
        this, &SettingsDialog::showPortInfo);
    connect(m_ui->baudRateBox, &QComboBox::currentIndexChanged,
        this, &SettingsDialog::checkCustomBaudRatePolicy);
    connect(m_ui->serialPortInfoListBox, &QComboBox::currentIndexChanged,
        this, &SettingsDialog::checkCustomDevicePathPolicy);

    fillPortsParameters();
    fillPortsInfo();

    fillFromSettings();
}

void SettingsDialog::showPortInfo(int idx){
    emit message("SettingsDialog::showPortInfo");
    if (idx == -1)
        return;

    const QString blankString = tr(::blankString);

    const QStringList list = m_ui->serialPortInfoListBox->itemData(idx).toStringList();
    m_ui->descriptionLabel->setText(tr("Description: %1").arg(list.value(1, blankString)));
    m_ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.value(2, blankString)));
    m_ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.value(3, blankString)));
    m_ui->locationLabel->setText(tr("Location: %1").arg(list.value(4, blankString)));
    m_ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.value(5, blankString)));
    m_ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.value(6, blankString)));
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");
    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply(){
    emit message("SettingsDialog::apply");
    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    close();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx){
    emit message("SettingsDialog::checkCustomBaudRatePolicy");
    const bool isCustomBaudRate = !m_ui->baudRateBox->itemData(idx).isValid();
    m_ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_ui->baudRateBox->clearEditText();
        QLineEdit *edit = m_ui->baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx){
    emit message("SettingsDialog::checkCustomDevicePathPolicy");
    const bool isCustomPath = !m_ui->serialPortInfoListBox->itemData(idx).isValid();
    m_ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        m_ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::fillPortsParameters(){
    emit message("SettingsDialog::fillPortsParameters");
    m_ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ui->baudRateBox->addItem(tr("Custom"));

    m_ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ui->dataBitsBox->setCurrentIndex(3);

    m_ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    m_ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    m_ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    m_ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    m_ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    m_ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo(){
    emit message("SettingsDialog::fillPortsInfo");
    m_ui->serialPortInfoListBox->clear();
    const QString blankString = tr(::blankString);
    const auto infos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        const QString description = info.description();
        const QString manufacturer = info.manufacturer();
        const QString serialNumber = info.serialNumber();
        const auto vendorId = info.vendorIdentifier();
        const auto productId = info.productIdentifier();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (vendorId ? QString::number(vendorId, 16) : blankString)
             << (productId ? QString::number(productId, 16) : blankString);

        m_ui->serialPortInfoListBox->addItem(list.constFirst(), list);
    }

    m_ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void SettingsDialog::fillFromSettings(){
    emit message("SettingsDialog::fillFromSettings");
    int idx;
    const SerialSettings* setts = settings<SerialSettings>();
    if((idx = m_ui->serialPortInfoListBox->findText(setts->name())) == -1){
        if(setts->name().isEmpty()) {
            m_ui->serialPortInfoListBox->setCurrentIndex(0);
        }else{
            m_ui->serialPortInfoListBox->setCurrentIndex(4);
            m_ui->serialPortInfoListBox->setCurrentText(setts->name());
        }
    } else{
        m_ui->serialPortInfoListBox->setCurrentIndex(idx);
    }
    
    if((idx = m_ui->baudRateBox->findText(QString::number(setts->baudRate()))) == -1){
        m_ui->baudRateBox->setCurrentIndex(4);
        m_ui->baudRateBox->setCurrentText(QString::number(setts->baudRate()));
    } else{
        m_ui->baudRateBox->setCurrentIndex(idx);
    }
    
    m_ui->dataBitsBox->setCurrentIndex(m_ui->dataBitsBox->findData(setts->dataBits()));
    m_ui->parityBox->setCurrentIndex(m_ui->parityBox->findData(setts->parity()));
    m_ui->stopBitsBox->setCurrentIndex(m_ui->stopBitsBox->findData(setts->stopBits()));
    m_ui->flowControlBox->setCurrentIndex(m_ui->flowControlBox->findData(setts->flowControl()));
    m_ui->localEchoCheckBox->setChecked(setts->localEchoEnabled());
    m_ui->autoConnectCheckBox->setChecked(setts->autoConnect());
}

SettingsDialog::operator SerialSettings() const {
    SerialSettings ret;
    ret.setName(m_ui->serialPortInfoListBox->currentText());

    if (m_ui->baudRateBox->currentIndex() == 4) {
        ret.setBaudRate(m_ui->baudRateBox->currentText().toInt());
    } else {
        const auto baudRateData = m_ui->baudRateBox->currentData();
        ret.setBaudRate(baudRateData.value<QSerialPort::BaudRate>());
    }

    const auto dataBitsData = m_ui->dataBitsBox->currentData();
    ret.setDataBits(dataBitsData.value<QSerialPort::DataBits>());

    const auto parityData = m_ui->parityBox->currentData();
    ret.setParity(parityData.value<QSerialPort::Parity>());

    const auto stopBitsData = m_ui->stopBitsBox->currentData();
    ret.setStopBits(stopBitsData.value<QSerialPort::StopBits>());

    const auto flowControlData = m_ui->flowControlBox->currentData();
    ret.setFlowControl(flowControlData.value<QSerialPort::FlowControl>());

    ret.setLocalEchoEnabled(m_ui->localEchoCheckBox->isChecked());
    ret.setAutoConnect(m_ui->autoConnectCheckBox->isChecked());

    return ret;
}

void SettingsDialog::updateSettings(){
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);
    SerialSettings* setts = settings<SerialSettings>();

    SerialSettings newSettings = *this;

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<SerialSettings>(settingsPath(), setts);
}

void SettingsDialog::settingsChanged(){
    emit message("SettingsDialog::settingsChanged");
}
