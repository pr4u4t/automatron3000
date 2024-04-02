#include "preferences.h"
#include "ui_preferences.h"
#include "PreferencePage.h"

Preferences::Preferences(QWidget* mwin, Loader* loader, const QString& settingsPath) 
    : SettingsMdi(mwin)
    , m_ui(new Ui::Preferences){
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    connect(m_ui->ok, &QPushButton::clicked,
        this, &Preferences::ok);
    connect(m_ui->apply, &QPushButton::clicked,
        this, &Preferences::apply);
    connect(m_ui->cancel, &QPushButton::clicked,
        this, &Preferences::cancel);
}

Preferences::~Preferences() {
    delete m_ui;
}

void Preferences::apply() {
    updateSettings();
}
    
void Preferences::cancel() {
    close();
}
    
void Preferences::ok() {
    updateSettings();
    close();
}

void Preferences::updateSettings() {
    QWidget* w = m_ui->scrollArea->widget();
    if (w == nullptr) {
        emit message("Preferences::updateSettings(): widget == nullptr");
        return;
    }

    PreferencePage* page = qobject_cast<PreferencePage*>(w);
    if (page == nullptr) {
        emit message("Preferences::updateSettings(): page == nullptr");
        return;
    }

    page->updateSettings();
}