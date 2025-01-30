#include "settingsdialog.h"

// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QLinReadByID.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new LinReadByIDSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QLinReadByID* read)
    : SettingsMdi(parent, new LinReadByIDSettings(*(read->settings<LinReadByIDSettings>())), read->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
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

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    const LinReadByIDSettings* setts = settings<LinReadByIDSettings>();
    m_ui->frameData->setText(setts->frameData());
    m_ui->interval->setValue(setts->interval());
    m_ui->tries->setValue(setts->tries());
    m_ui->title->setText(setts->title());
    m_ui->linEdit->setText(setts->linDevice());
    m_ui->previousEdit->setText(setts->previous());
    m_ui->maxReschedules->setValue(setts->maxReschedules());
    m_ui->rescheduleInterval->setValue(setts->rescheduleInterval());
}

SettingsDialog::operator LinReadByIDSettings() const {
    LinReadByIDSettings ret;

    ret.setFrameData(m_ui->frameData->text().toLocal8Bit());
    ret.setInterval(m_ui->interval->value());
    ret.setTries(m_ui->tries->value());
    ret.setTitle(m_ui->title->text());
    ret.setLinDevice(m_ui->linEdit->text());
    ret.setPrevious(m_ui->previousEdit->text());
    ret.setMaxReschedules(m_ui->maxReschedules->value());
    ret.setRescheduleInterval(m_ui->rescheduleInterval->value());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    LinReadByIDSettings newSettings = *this;
    LinReadByIDSettings* setts = settings<LinReadByIDSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<LinReadByIDSettings>(settingsPath(), setts);
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