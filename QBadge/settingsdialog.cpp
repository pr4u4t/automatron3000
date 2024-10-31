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

    connect(m_ui->fileDialogButton, &QPushButton::clicked, this, &SettingsDialog::chooseImage);

    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {}

BadgeSettings SettingsDialog::dataSettings() const {
    return m_currentSettings;
}

QString SettingsDialog::settingsPath() const {
    return m_settingsPath;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    m_ui->imagePathEdit->setText(m_currentSettings.imagePath());
    m_ui->textEdit->setText(m_currentSettings.text());
    m_ui->titleEdit->setText(m_currentSettings.title());

}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    m_currentSettings.setImagePath(m_ui->imagePathEdit->text());
    m_currentSettings.setText(m_ui->textEdit->document()->toMarkdown());
    m_currentSettings.setTitle(m_ui->titleEdit->text());

    QSettings s = Settings::get();
    m_currentSettings.save(s, settingsPath());
}

void SettingsDialog::chooseImage() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        "/home",
        tr("Images (*.png *.xpm *.jpg *.bmp *.jpeg)"));

    m_ui->imagePathEdit->setText(fileName);
}

bool SettingsDialog::verifySettings() const {
    emit message("SettingsDialog::verifySettings");
    return QFile::exists(m_ui->imagePathEdit->text());
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