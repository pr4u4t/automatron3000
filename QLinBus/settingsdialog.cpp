// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QLinBus.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>

//#include "DockManager.h"
#include <QCloseEvent>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new LinBusSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QLinBus* linbus)
    : SettingsMdi(parent, new LinBusSettings(*(linbus->settings<LinBusSettings>())), linbus->settingsPath())
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
    const LinBusSettings* setts = settings<LinBusSettings>();
    m_ui->scanStartID->setValue(setts->scanStartID());
    m_ui->scanStopID->setValue(setts->scanStopID());
    m_ui->scanInterval->setValue(setts->scanInterval());
    m_ui->enableColors->setChecked(setts->enableColors());
    m_ui->linEdit->setText(setts->linDevice());
}

SettingsDialog::operator LinBusSettings() const {
    LinBusSettings ret;

    ret.setScanStartID(m_ui->scanStartID->value());
    ret.setScanStopID(m_ui->scanStopID->value());
    ret.setScanInterval(m_ui->scanInterval->value());
    ret.setEnableColors(m_ui->enableColors->isChecked());
    ret.setLinDevice(m_ui->linEdit->text());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    LinBusSettings newSettings = *this;
    LinBusSettings* setts = settings<LinBusSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());

    *setts = newSettings;
    Settings::store<LinBusSettings>(settingsPath(), setts);
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