// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QLinTester.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new LinTesterSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QLinTester* tester)
    : SettingsMdi(parent, new LinTesterSettings(*(tester->settings<LinTesterSettings>())), tester->settingsPath())
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
    const LinTesterSettings* setts = settings<LinTesterSettings>();
    m_ui->testStartID->setValue(setts->testStartID());
    m_ui->testStopID->setValue(setts->testStopID());
    m_ui->interval->setValue(setts->testInterval());
    m_ui->tries->setValue(setts->tries());
    m_ui->linEdit->setText(setts->linDevice());
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    LinTesterSettings newSettings = *this;
    LinTesterSettings* setts = settings<LinTesterSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());

    *setts = newSettings;
    Settings::store<LinTesterSettings>(settingsPath(), setts);
}

SettingsDialog::operator LinTesterSettings() const {
    LinTesterSettings ret;

    ret.setTestStartID(m_ui->testStartID->value());
    ret.setTestStopID(m_ui->testStopID->value());
    ret.setTestInterval(m_ui->interval->value());
    ret.setTries(m_ui->tries->value());
    ret.setLinDevice(m_ui->linEdit->text());

    return ret;
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