#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QMLVisual.h"

#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new MLVisualSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QMLVisual* visual)
    : SettingsMdi(parent, new MLVisualSettings(*(visual->settings<MLVisualSettings>())), visual->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)){
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

    connect(m_ui->viewPathButton, &QPushButton::clicked, this, &SettingsDialog::selectViewPath);
    connect(m_ui->searchPathButton, &QPushButton::clicked, this, &SettingsDialog::selectSearchPath);

    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    const MLVisualSettings* setts = settings<MLVisualSettings>();
    m_ui->viewPathEdit->setText(setts->viewerPath());
    m_ui->searchPathEdit->setText(setts->searchPath());
}

SettingsDialog::operator MLVisualSettings() const {
    MLVisualSettings ret;
    ret.setViewerPath(m_ui->viewPathEdit->text());
    ret.setSearchPath(m_ui->searchPathEdit->text());
    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    MLVisualSettings newSettings = *this;
    MLVisualSettings* setts = settings<MLVisualSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<MLVisualSettings>(settingsPath(), setts);
}

void SettingsDialog::selectViewPath() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
        "/home",
        tr("View (*.qml)"));

    m_ui->viewPathEdit->setText(fileName);
}

void SettingsDialog::selectSearchPath() {
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
        ".",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    m_ui->searchPathEdit->setText(dirName);
}

bool SettingsDialog::verifySettings() const {
    emit message("SettingsDialog::verifySettings");
    return  QFile::exists(m_ui->viewPathEdit->text())
            && QFile::exists(m_ui->searchPathEdit->text());
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