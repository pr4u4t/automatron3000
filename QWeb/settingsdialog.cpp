#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QWeb.h"

#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new WebSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QWeb* web)
    : SettingsMdi(parent, new WebSettings(*(web->settings<WebSettings>())), web->settingsPath())
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
    const WebSettings* setts = settings<WebSettings>();
    m_ui->urlEdit->setText(setts->url());
    m_ui->refreshEdit->setValue(setts->refresh());
}

SettingsDialog::operator WebSettings() const {
    WebSettings ret;

    ret.setUrl(m_ui->urlEdit->text());
    ret.setRefresh(m_ui->refreshEdit->value());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    WebSettings newSettings = *this;
    WebSettings* setts = settings<WebSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<WebSettings>(settingsPath(), setts);
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");
    //if (verifySettings() == false) {
    //    emit message("SettingsDialog::ok: failed to verify settings");
    //    return;
    //}
    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply() {
    emit message("SettingsDialog::apply");
    //if (verifySettings() == false) {
    //    emit message("SettingsDialog::ok: failed to verify settings");
    //    return;
    //}
    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    emit message("SettingsDialog::cancel");
    close();
}