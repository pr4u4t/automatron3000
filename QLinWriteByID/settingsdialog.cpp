#include "settingsdialog.h"

// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QLinWriteByID.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new LinWriteByIDSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QLinWriteByID* write) 
    : SettingsMdi(parent, new LinWriteByIDSettings(*(write->settings<LinWriteByIDSettings>())), write->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

void SettingsDialog::setup() {
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    connect(m_ui->okButton, &QPushButton::clicked,
        this, &SettingsDialog::ok);
    connect(m_ui->applyButton, &QPushButton::clicked,
        this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked,
        this, &SettingsDialog::cancel);

    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    const LinWriteByIDSettings* setts = settings<LinWriteByIDSettings>();
    m_ui->nadEdit->setText(setts->nad());
    m_ui->didlEdit->setText(setts->didl());
    m_ui->didhEdit->setText(setts->didh());
    m_ui->interval->setValue(setts->interval());
    m_ui->tries->setValue(setts->tries());
    m_ui->title->setText(setts->title());
    m_ui->dataSource->setText(setts->dataSource());
    m_ui->previousEdit->setText(setts->previous());
    m_ui->linEdit->setText(setts->linDevice());
    m_ui->reschedulesEdit->setValue(setts->maxReschedules());
    m_ui->rescheduleIntervalEdit->setValue(setts->rescheduleInterval());
}

SettingsDialog::operator LinWriteByIDSettings() const {
    LinWriteByIDSettings ret;

    ret.setNad(m_ui->nadEdit->text().toLocal8Bit());
    ret.setDidl(m_ui->didlEdit->text().toLocal8Bit());
    ret.setDidh(m_ui->didhEdit->text().toLocal8Bit());
    ret.setInterval(m_ui->interval->value());
    ret.setTries(m_ui->tries->value());
    ret.setTitle(m_ui->title->text());
    ret.setDataSource(m_ui->dataSource->text());
    ret.setPrevious(m_ui->previousEdit->text());
    ret.setLinDevice(m_ui->linEdit->text());
    ret.setMaxReschedules(m_ui->reschedulesEdit->value());
    ret.setRescheduleInterval(m_ui->rescheduleIntervalEdit->value());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    LinWriteByIDSettings newSettings = *this;
    LinWriteByIDSettings* setts = settings<LinWriteByIDSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<LinWriteByIDSettings>(settingsPath(), setts);
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