// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "passworddialog.h"
#include "QData.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath) 
    : SettingsMdi(parent, new DataSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)){
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QData* data) 
    : SettingsMdi(parent, new DataSettings(*(data->settings<DataSettings>())), data->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

void SettingsDialog::setup() {
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
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    const DataSettings* setts = settings<DataSettings>();

    m_ui->dbDriver->setCurrentIndex(m_ui->dbDriver->findText(setts->dbDriver()));
    m_ui->dbUri->setText(setts->dbUri());
    m_ui->dbName->setText(setts->dbName());
    m_ui->dbTable->setText(setts->dbTable());
    m_ui->dbLock->setCurrentIndex(m_ui->dbLock->findText(setts->dbLock() ? tr("Yes") : tr("No")));
    m_ui->dbLockPass->setText(setts->dbLockPass());
    m_ui->dbLockPassConfirm->setText(setts->dbLockPass());
    m_ui->serialPrefix->setText(setts->serialPrefix());
    m_ui->codeRegexp->setText(setts->barcodeRegexp());
    m_ui->serialInterval->setText(QString::number(setts->serialInterval()));
    m_ui->removeChars->setText(setts->removeChars());
    m_ui->clearCode->setText(QString::number(setts->clearCode()));
    m_ui->keepClear->setChecked(setts->keepClear());
}

SettingsDialog::operator DataSettings() const {
    DataSettings ret;

    ret.setDbDriver(m_ui->dbDriver->currentText());
    ret.setDbUri(m_ui->dbUri->text());
    ret.setDbName(m_ui->dbName->text());
    ret.setDbTable(m_ui->dbTable->text());
    ret.setDbLock(m_ui->dbLock->currentText() == tr("Yes") ? true : false);
    ret.setDbLockPass(m_ui->dbLockPass->text().toLocal8Bit());
    ret.setSerialPrefix(m_ui->serialPrefix->text());
    ret.setBarcodeRegexp(m_ui->codeRegexp->text());
    ret.setSerialInterval(m_ui->serialInterval->text().isEmpty() ? -1 : m_ui->serialInterval->text().toInt());
    ret.setRemoveChars(m_ui->removeChars->text());
    ret.setClearCode(m_ui->clearCode->text().isEmpty() ? -1 : m_ui->clearCode->text().toInt());
    ret.setKeepClear(m_ui->keepClear->isChecked());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");
    DataSettings newSettings = *this;
   
    DataSettings* setts = settings<DataSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<DataSettings>(settingsPath(), setts);
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");
    const DataSettings* setts = settings<DataSettings>();
    if (setts->dbLock() && setts->dbLockPass().isEmpty() != true) {
        PasswordDialog d(this, setts->dbLockPass());
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
    const DataSettings* setts = settings<DataSettings>();
    if (setts->dbLock() && setts->dbLockPass().isEmpty() != true) {
        PasswordDialog d(this, setts->dbLockPass());
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