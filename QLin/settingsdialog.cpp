// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "CLin.h"
#include "QLin.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QTimer>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

bool operator==(const ChannelConfig& lhs, const ChannelConfig& rhs) {
    return  lhs.serialNumber == rhs.serialNumber
        && lhs.articleNumber == rhs.articleNumber
        && strncmp(lhs.name, rhs.name, sizeof(rhs.name));
}

bool operator==(const XLchannelConfig& lhs, const ChannelConfig& rhs) {
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
    : SettingsMdi(parent, new LinSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_model(new QStandardItemModel(8,1))
    , m_sdlcModel(new QStandardItemModel(64, 1)){
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QLin* lin)
    : SettingsMdi(parent, new LinSettings(*(lin->settings<LinSettings>())), lin->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_model(new QStandardItemModel(8, 1))
    , m_sdlcModel(new QStandardItemModel(64, 1)) {
    setup();
}

void SettingsDialog::setup() {
    emit message("SettingsDialog::SettingsDialog", LoggerSeverity::LOG_DEBUG);
    m_ui->setupUi(this);

    m_ui->slaveInitialData->setModel(m_model);
    m_ui->slaveInitialData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Value"));
    m_ui->slaveInitialData->setMinimumHeight(m_ui->slaveInitialData->verticalHeader()->sectionSize(0) * 9);

    m_ui->DLC->setModel(m_sdlcModel);
    m_ui->DLC->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_sdlcModel->setHeaderData(0, Qt::Horizontal, tr("Size"));
    m_ui->DLC->setMinimumHeight(m_ui->slaveInitialData->verticalHeader()->sectionSize(0) * 65);

    m_ui->modeGroup->setId(m_ui->masterCheck, LinSettings::Mode::MASTER);
    m_ui->modeGroup->setId(m_ui->slaveCheck, LinSettings::Mode::SLAVE);
    m_ui->modeGroup->setId(m_ui->masterNslave, LinSettings::Mode::MASTER_WITH_SLAVE);

    m_ui->checksumMethod->model()->setData(m_ui->checksumMethod->model()->index(0, 0), XL_LIN_CALC_CHECKSUM, Qt::ItemDataRole::UserRole);
    m_ui->checksumMethod->model()->setData(m_ui->checksumMethod->model()->index(1, 0), XL_LIN_CALC_CHECKSUM_ENHANCED, Qt::ItemDataRole::UserRole);

    connect(m_ui->okButton, &QPushButton::clicked,
        this, &SettingsDialog::ok);
    connect(m_ui->applyButton, &QPushButton::clicked,
        this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked,
        this, &SettingsDialog::cancel);
    connect(m_ui->vectorConfig, &QPushButton::clicked,
        this, &SettingsDialog::vectorConfig);


    QTimer::singleShot(0, this, &SettingsDialog::fillFromSettings);
}

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings", LoggerSeverity::LOG_DEBUG);
    const LinSettings* setts = settings<LinSettings>();
    switch (setts->linVersion()) {
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

    m_ui->modeGroup->button(setts->mode())->setChecked(true);

    m_ui->slaveID->setValue(setts->slaveID());
    m_ui->autoConnect->setChecked(setts->autoConnect());
    m_ui->baudrate->setValue(setts->baudrate());

    for (qsizetype it = 0, end = setts->initialData().size(); it < end; ++it) {
         m_model->setData(m_model->index(it, 0),"0x"+QString().setNum(static_cast<unsigned char>(setts->initialData().at(it)), 16));
    }

    for (qsizetype it = 0, end = setts->dlc().size(); it < end; ++it) {
        m_sdlcModel->setData(m_sdlcModel->index(it, 0), QString::number(setts->dlc().at(it)));
    }

    m_ui->configName->setText(setts->appName());
    m_ui->SlaveDLC->setValue(setts->slaveDLC());
    QList<XLchannelConfig> channels = CLin::LINGetDevices();

    if (channels.size() == 0) {
        emit message("SettingsDialog::fillFromSettings: no LIN channels found");
    }

    if (CLin::LINFindDevice(setts->hwChannel()) == false) {
        emit message("SettingsDialog::fillFromSettings: ****************************************************************", LoggerSeverity::LOG_WARNING);
        emit message("SettingsDialog::fillFromSettings: Platform configuration changed please update you settings", LoggerSeverity::LOG_WARNING);
        emit message("SettingsDialog::fillFromSettings: and save before proceeding", LoggerSeverity::LOG_WARNING);
        emit message("SettingsDialog::fillFromSettings: ****************************************************************", LoggerSeverity::LOG_WARNING);
    }

    int i = 0;
    for (auto it = channels.begin(), end = channels.end(); it != end; ++it) {
        m_ui->channelCombo->addItem(QString("%1 serial: %2").arg((*it).name).arg((*it).serialNumber), QVariant::fromValue(ChannelConfig(*it)));
        if ((*it) == setts->hwChannel()) {
            m_ui->channelCombo->setCurrentIndex(i);
        }
        ++i;
    }

    m_ui->queueSize->setValue(setts->queueSize());
    switch (m_ui->checksumMethod->currentData(Qt::ItemDataRole::UserRole).toInt()) {
        case XL_LIN_CALC_CHECKSUM:
            m_ui->checksumMethod->setCurrentIndex(0);
            break;

        case XL_LIN_CALC_CHECKSUM_ENHANCED:
            m_ui->checksumMethod->setCurrentIndex(1);
            break;
    
    }

    for (int i = 0; i < m_model->rowCount(); i++) {
        m_model->setHeaderData(i, Qt::Vertical, i);
    }

    for (int i = 0; i < m_sdlcModel->rowCount(); i++) {
        m_sdlcModel->setHeaderData(i, Qt::Vertical, i);
    }
}

SettingsDialog::operator LinSettings() const {
    LinSettings ret;

    QString version = m_ui->linVersion->currentText();

    if (version == "1.3") {
        ret.setLinVersion(XL_LIN_VERSION_1_3);
    } else if (version == "2.0") {
        ret.setLinVersion(XL_LIN_VERSION_2_0);
    } else if (version == "2.1") {
        ret.setLinVersion(XL_LIN_VERSION_2_1);
    }

    ret.setSlaveID(static_cast<unsigned char>(m_ui->slaveID->text().toUInt()));

    ret.setMode(static_cast<LinSettings::Mode>(m_ui->modeGroup->checkedId()));
    ret.setAutoConnect(m_ui->autoConnect->isChecked());
    const LinSettings* setts = settings<LinSettings>();

    QByteArray tmp = setts->initialData();
    for (auto it = m_model->index(0, 0); it.isValid(); it = it.siblingAtRow(it.row() + 1)) {
        tmp[it.row()] = it.data().toString().toInt(nullptr, 16);
    }
    ret.setInitialData(tmp);

    ret.setBaudrate(m_ui->baudrate->value());

    for (auto it = m_sdlcModel->index(0, 0); it.isValid(); it = it.siblingAtRow(it.row() + 1)) {
        setts->dlc()[it.row()] = it.data().toString().toInt();
    }

    QVariant channel = m_ui->channelCombo->itemData(m_ui->channelCombo->currentIndex(), Qt::UserRole);
    ChannelConfig config = channel.value<ChannelConfig>();
    ret.setHwChannel(config);

    ret.setQueueSize(m_ui->queueSize->value());
    ret.setChecksumMethod(m_ui->checksumMethod->currentData(Qt::ItemDataRole::UserRole).toInt());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    LinSettings newSettings = *this;
    LinSettings* setts = settings<LinSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());

    *setts = newSettings;
    Settings::store<LinSettings>(settingsPath(), setts);
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok", LoggerSeverity::LOG_DEBUG);

    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply() {
    emit message("SettingsDialog::apply", LoggerSeverity::LOG_DEBUG);

    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    emit message("SettingsDialog::cancel", LoggerSeverity::LOG_DEBUG);
    close();
}