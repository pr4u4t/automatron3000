#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFileDialog>

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

    connect(m_ui->programPathButton, &QPushButton::clicked, this, &SettingsDialog::chooseFile);

    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {}

SettingsDialog::QJTAGSettings SettingsDialog::dataSettings() const {
    return m_currentSettings;
}

QString SettingsDialog::settingsPath() const {
    return m_settingsPath;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    m_ui->programPathEdit->setText(m_currentSettings.programPath);
    m_ui->buttonLabelEdit->setText(m_currentSettings.buttonLabel);
    m_ui->titleEdit->setText(m_currentSettings.title);
    m_ui->argumentsEdit->setPlainText(m_currentSettings.arguments);

}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    m_currentSettings.programPath = m_ui->programPathEdit->text();
    m_currentSettings.buttonLabel = m_ui->buttonLabelEdit->text();
    m_currentSettings.title = m_ui->titleEdit->text();
    m_currentSettings.arguments = m_ui->argumentsEdit->toPlainText();

    QSettings s = Settings::get();
    m_currentSettings.save(s, settingsPath());
}

void SettingsDialog::chooseFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        "/home",
        tr("Executable (*.exe *.bat)"));

    m_ui->programPathEdit->setText(fileName);
}

bool SettingsDialog::verifySettings() const {
    emit message("SettingsDialog::verifySettings");
    return QFile::exists(m_ui->programPathEdit->text());
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");
    if (verifySettings() == false) {
        emit message("SettingsDialog::ok: failed to verify settings");
        return;
    }
    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply() {
    emit message("SettingsDialog::apply");
    if (verifySettings() == false) {
        emit message("SettingsDialog::ok: failed to verify settings");
        return;
    }
    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    emit message("SettingsDialog::cancel");
    close();
}