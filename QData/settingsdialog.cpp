// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "passworddialog.h"

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
    , m_settingsPath(settingsPath){
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    m_ui->dbLock->setInsertPolicy(QComboBox::NoInsert);
    m_ui->dbDriver->setInsertPolicy(QComboBox::NoInsert);

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

SettingsDialog::DataSettings SettingsDialog::dataSettings() const {
    emit message("SettingsDialog::settings");
    return m_currentSettings;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    m_ui->dbDriver->setCurrentIndex(m_ui->dbDriver->findText(m_currentSettings.dbDriver));
    m_ui->dbUri->setText(m_currentSettings.dbUri);
    m_ui->dbName->setText(m_currentSettings.dbName);
    m_ui->dbTable->setText(m_currentSettings.dbTable);
    m_ui->dbLock->setCurrentIndex(m_ui->dbLock->findText(m_currentSettings.dbLock ? tr("Yes") : tr("No")));
    m_ui->dbLockPass->setText(m_currentSettings.dbLockPass);
    m_ui->dbLockPassConfirm->setText(m_currentSettings.dbLockPass);
    m_ui->serialPrefix->setText(m_currentSettings.serialPrefix);
    m_ui->codeRegexp->setText(m_currentSettings.barcodeRegexp);
    m_ui->serialInterval->setText(QString::number(m_currentSettings.serialInterval));
    m_ui->removeChars->setText(m_currentSettings.removeChars);
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    m_currentSettings.dbDriver = m_ui->dbDriver->currentText();
    m_currentSettings.dbUri = m_ui->dbUri->text();
    m_currentSettings.dbName = m_ui->dbName->text();
    m_currentSettings.dbTable = m_ui->dbTable->text();
    m_currentSettings.dbLock = m_ui->dbLock->currentText() == tr("Yes") ? true : false;
    m_currentSettings.dbLockPass = m_ui->dbLockPass->text().toLocal8Bit();
    m_currentSettings.serialPrefix = m_ui->serialPrefix->text();
    m_currentSettings.barcodeRegexp = m_ui->codeRegexp->text();
    m_currentSettings.serialInterval = m_ui->serialInterval->text().toInt();
    m_currentSettings.removeChars = m_ui->removeChars->text();

    QSettings s = Settings::get();
    m_currentSettings.save(s, settingsPath());
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");
    if (m_currentSettings.dbLock && m_currentSettings.dbLockPass.isEmpty() != true) {
        PasswordDialog d(this, m_currentSettings.dbLockPass);
        if (d.exec() != QDialog::Accepted) {
            return;
        }
    }

    if (m_ui->dbLockPass->text() != m_ui->dbLockPassConfirm->text()) {
        QMessageBox::critical(this, tr("Error"), tr("Password mismatch"));
        return;
    }

    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply() {
    emit message("SettingsDialog::apply");

    if (m_currentSettings.dbLock && m_currentSettings.dbLockPass.isEmpty() != true) {
        PasswordDialog d(this, m_currentSettings.dbLockPass);
        if (d.exec() != QDialog::Accepted) {
            return;
        }
    }

    if (m_ui->dbLockPass->text() != m_ui->dbLockPassConfirm->text()) {
        QMessageBox::critical(this, tr("Error"), tr("Password mismatch"));
        return;
    }

    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    emit message("SettingsDialog::cancel");
    close();
}