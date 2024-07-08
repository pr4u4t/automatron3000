#include "settingsdialog.h"

// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent)
    , m_currentSettings(Settings::get(), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_settingsPath(settingsPath) {
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
    delete m_ui;
}

SettingsDialog::LinReadByIDSettings SettingsDialog::lintesterSettings() const {
    emit message("SettingsDialog::settings");
    return m_currentSettings;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    m_ui->frameData->setText(m_currentSettings.frameData);
    m_ui->interval->setValue(m_currentSettings.interval);
    m_ui->tries->setValue(m_currentSettings.tries);
    m_ui->title->setText(m_currentSettings.title);
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    m_currentSettings.frameData = m_ui->frameData->text().toLocal8Bit();
    m_currentSettings.interval = m_ui->interval->value();
    m_currentSettings.tries = m_ui->tries->value();
    m_currentSettings.title = m_ui->title->text();

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