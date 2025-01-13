#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QCircularBar.h"

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new CircularBarSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QCircularBar* bar)
    : SettingsMdi(parent, new CircularBarSettings(*(bar->settings<CircularBarSettings>())), bar->settingsPath())
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
    CircularBarSettings* setts = settings<CircularBarSettings>();

    m_ui->minValueEdit->setValue(setts->minValue());
    m_ui->maxValueEdit->setValue(setts->maxValue());
    m_ui->tresholdEdit->setValue(setts->threshold());
    m_ui->precisionEdit->setValue(setts->precision());
    m_ui->labelEdit->setText(setts->label());
    m_ui->unitsEdit->setText(setts->units());
    m_ui->stepsEdit->setValue(setts->steps());
}

SettingsDialog::operator CircularBarSettings() const {
    CircularBarSettings ret;
    ret.setMinValue(m_ui->minValueEdit->value());
    ret.setMaxValue(m_ui->maxValueEdit->value());
    ret.setThreshold(m_ui->tresholdEdit->value());
    ret.setPrecision(m_ui->precisionEdit->value());
    ret.setLabel(m_ui->labelEdit->text());
    ret.setUnits(m_ui->unitsEdit->text());
    ret.setSteps(m_ui->stepsEdit->value());
    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    CircularBarSettings newSettings = *this;
    CircularBarSettings* setts = settings<CircularBarSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    *setts = newSettings;
    Settings::store<CircularBarSettings>(settingsPath(), setts);
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