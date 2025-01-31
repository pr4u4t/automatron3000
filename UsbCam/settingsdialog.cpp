#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "UsbCam.h"

#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new UsbCamSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const UsbCam* cam)
    : SettingsMdi(parent, new UsbCamSettings(*(cam->settings<UsbCamSettings>())), cam->settingsPath())
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

    connect(m_ui->fileDialogButton, &QPushButton::clicked, this, &SettingsDialog::chooseImage);

    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    UsbCamSettings* setts = settings<UsbCamSettings>();
    m_ui->imagePathEdit->setText(setts->imagePath());
    m_ui->textEdit->setText(setts->text());
    m_ui->titleEdit->setText(setts->title());

}

SettingsDialog::operator UsbCamSettings() const {
    UsbCamSettings ret;
    ret.setImagePath(m_ui->imagePathEdit->text());
    ret.setText(m_ui->textEdit->document()->toMarkdown());
    ret.setTitle(m_ui->titleEdit->text());
    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    UsbCamSettings newSettings = *this;
    UsbCamSettings* setts = settings<UsbCamSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<BadgeSettings>(settingsPath(), setts);
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