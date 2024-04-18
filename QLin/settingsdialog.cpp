// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "CLin.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

inline bool operator==(const ChannelConfig& lhs, const ChannelConfig& rhs) {
    return  lhs.serialNumber == rhs.serialNumber
        && lhs.articleNumber == rhs.articleNumber
        && strncmp(lhs.name, rhs.name, sizeof(rhs.name));
}

inline bool operator==(const XLchannelConfig& lhs, const ChannelConfig& rhs) {
    return  lhs.serialNumber == rhs.serialNumber
            && lhs.articleNumber == rhs.articleNumber
            && strncmp(lhs.name, rhs.name, sizeof(rhs.name)) == 0;
}

QDataStream& operator<< (QDataStream& stream, const ChannelConfig& object) {
    stream.writeRawData(object.name, sizeof(object.name));

    stream  << object.serialNumber
            << object.articleNumber
            << object.appChannel
            << object.busType
            << object.channelIndex
            << object.hwChannel
            << object.hwIndex;

    return stream;
}

QDataStream& operator>> (QDataStream& stream, ChannelConfig& object) {
    stream.readRawData(object.name, sizeof(object.name));
    stream >> object.serialNumber;
    stream >> object.articleNumber;
    stream >> object.appChannel;
    stream >> object.busType;
    stream >> object.channelIndex;
    stream >> object.hwChannel;
    stream >> object.hwIndex;
    return stream;
}

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent)
    , m_currentSettings(Settings::get(), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_settingsPath(settingsPath)
    , m_model(new QStandardItemModel(8,1))
    , m_sdlcModel(new QStandardItemModel(64, 1)){
    
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);
    
    m_ui->slaveInitialData->setModel(m_model);
    m_ui->slaveInitialData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Value"));
    m_ui->slaveInitialData->setMinimumHeight(m_ui->slaveInitialData->verticalHeader()->sectionSize(0)*9);

    m_ui->DLC->setModel(m_sdlcModel);
    m_ui->DLC->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_sdlcModel->setHeaderData(0, Qt::Horizontal, tr("Size"));
    m_ui->DLC->setMinimumHeight(m_ui->slaveInitialData->verticalHeader()->sectionSize(0) * 65);

    m_ui->modeGroup->setId(m_ui->masterCheck, SettingsDialog::LinSettings::Mode::MASTER);
    m_ui->modeGroup->setId(m_ui->slaveCheck, SettingsDialog::LinSettings::Mode::SLAVE);
    m_ui->modeGroup->setId(m_ui->masterNslave, SettingsDialog::LinSettings::Mode::MASTER_WITH_SLAVE);

    m_ui->checksumMethod->model()->setData(m_ui->checksumMethod->model()->index(0, 0), XL_LIN_CALC_CHECKSUM);
    m_ui->checksumMethod->model()->setData(m_ui->checksumMethod->model()->index(1, 0), XL_LIN_CALC_CHECKSUM_ENHANCED);

    connect(m_ui->okButton, &QPushButton::clicked,
        this, &SettingsDialog::ok);
    connect(m_ui->applyButton, &QPushButton::clicked,
        this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked,
        this, &SettingsDialog::cancel);
    connect(m_ui->vectorConfig, &QPushButton::clicked,
        this, &SettingsDialog::vectorConfig);


    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {
    delete m_ui;
}

SettingsDialog::LinSettings SettingsDialog::linSettings() const {
    emit message("SettingsDialog::settings");
    return m_currentSettings;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    switch (m_currentSettings.linVersion) {
        case XL_LIN_VERSION_1_3:
            m_ui->linVersion->setCurrentIndex(m_ui->linVersion->findText("1.3"));
            break;

        case XL_LIN_VERSION_2_0:
            m_ui->linVersion->setCurrentIndex(m_ui->linVersion->findText("2.0"));
            break;

        case XL_LIN_VERSION_2_1:
            m_ui->linVersion->setCurrentIndex(m_ui->linVersion->findText("2.1"));
            break;
    }

    if (m_currentSettings.mode == SettingsDialog::LinSettings::Mode::MASTER) {
        m_ui->masterCheck->setChecked(true);
    } else {
        m_ui->slaveCheck->setChecked(true);
    }

    m_ui->slaveID->setValue(m_currentSettings.slaveID);
    m_ui->autoConnect->setChecked(m_currentSettings.autoConnect);
    m_ui->baudrate->setValue(m_currentSettings.baudrate);

    for (qsizetype it = 0, end = m_currentSettings.initialData.size(); it < end; ++it) {
         m_model->setData(m_model->index(it, 0),"0x"+QString().setNum(static_cast<unsigned char>(m_currentSettings.initialData.at(it)), 16));
    }

    for (qsizetype it = 0, end = m_currentSettings.dlc.size(); it < end; ++it) {
        m_sdlcModel->setData(m_sdlcModel->index(it, 0), QString::number(m_currentSettings.dlc.at(it)));
    }

    m_ui->configName->setText(m_currentSettings.appName);
    m_ui->SlaveDLC->setValue(m_currentSettings.slaveDLC);
    QList<XLchannelConfig> channels = CLin::LINGetDevices();

    int i = 0;
    for (auto it = channels.begin(), end = channels.end(); it != end; ++it) {
        m_ui->channelCombo->addItem(QString((*it).name), QVariant::fromValue(ChannelConfig(*it)));
        if ((*it) == m_currentSettings.hwChannel) {
            m_ui->channelCombo->setCurrentIndex(i);
        }
        ++i;
    }

    m_ui->queueSize->setValue(m_currentSettings.queueSize);
    switch (m_ui->checksumMethod->currentData(2).toInt()) {
        case XL_LIN_CALC_CHECKSUM:
            m_ui->checksumMethod->setCurrentIndex(0);
            break;

        case XL_LIN_CALC_CHECKSUM_ENHANCED:
            m_ui->checksumMethod->setCurrentIndex(1);
            break;
    
    }
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    QString version = m_ui->linVersion->currentText();

    if (version == "1.3") {
        m_currentSettings.linVersion = XL_LIN_VERSION_1_3;
    } else if (version == "2.0") {
        m_currentSettings.linVersion = XL_LIN_VERSION_2_0;
    } else if (version == "2.1") {
        m_currentSettings.linVersion = XL_LIN_VERSION_2_1;
    }

    m_currentSettings.slaveID = static_cast<unsigned char>(m_ui->slaveID->text().toUInt());

    m_currentSettings.mode = static_cast<LinSettings::Mode>(m_ui->modeGroup->checkedId());
    m_currentSettings.autoConnect = m_ui->autoConnect->isChecked();

    for (auto it = m_model->index(0, 0); it.isValid(); it = it.siblingAtRow(it.row() + 1)) {
        m_currentSettings.initialData[it.row()] = it.data().toString().toInt(nullptr, 16);
    }

    m_currentSettings.baudrate = m_ui->baudrate->value();

    for (auto it = m_sdlcModel->index(0, 0); it.isValid(); it = it.siblingAtRow(it.row() + 1)) {
        m_currentSettings.dlc[it.row()] = it.data().toString().toInt();
    }

    QVariant channel = m_ui->channelCombo->itemData(m_ui->channelCombo->currentIndex(), Qt::UserRole);
    ChannelConfig config = channel.value<ChannelConfig>();
    m_currentSettings.hwChannel = config;

    m_currentSettings.queueSize = m_ui->queueSize->value();
    m_currentSettings.checksumMethod = m_ui->checksumMethod->currentData(2).toInt();

    QSettings s = Settings::get();
    m_currentSettings.save(s, settingsPath());
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");

    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply() {
    emit message("SettingsDialog::apply");

    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    emit message("SettingsDialog::cancel");
    close();
}