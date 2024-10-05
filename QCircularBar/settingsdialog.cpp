#include "settingsdialog.h"
#include "ui_settingsdialog.h"

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

SettingsDialog::~SettingsDialog() {}

SettingsDialog::CircularBarSettings SettingsDialog::circularbarSettings() const {
    return m_currentSettings;
}

QString SettingsDialog::settingsPath() const {
    return m_settingsPath;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    m_ui->minValueEdit->setValue(m_currentSettings.minValue);
    m_ui->maxValueEdit->setValue(m_currentSettings.maxValue);
    m_ui->tresholdEdit->setValue(m_currentSettings.threshold);
    m_ui->precisionEdit->setValue(m_currentSettings.precision);
    m_ui->labelEdit->setText(m_currentSettings.label);
    m_ui->unitsEdit->setText(m_currentSettings.units);
    m_ui->stepsEdit->setValue(m_currentSettings.steps);
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    m_currentSettings.minValue = m_ui->minValueEdit->value();
    m_currentSettings.maxValue = m_ui->maxValueEdit->value();
    m_currentSettings.threshold = m_ui->tresholdEdit->value();
    m_currentSettings.precision = m_ui->precisionEdit->value();
    m_currentSettings.label = m_ui->labelEdit->text();
    m_currentSettings.units = m_ui->unitsEdit->text();
    m_currentSettings.steps = m_ui->stepsEdit->value();

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