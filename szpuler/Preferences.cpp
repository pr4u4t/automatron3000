#include "preferences.h"
#include "ui_preferences.h"
#include "PreferencePage.h"

Preferences::Preferences(QWidget* mwin, Loader* loader, const QString& settingsPath, MLoader* plugins) 
    : SettingsMdi(mwin, nullptr, settingsPath)
    , m_ui(new Ui::Preferences)
    , m_plugins(plugins)
    , m_model(new QStandardItemModel(0,1)){
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    connect(m_ui->ok, &QPushButton::clicked,
        this, &Preferences::ok);
    connect(m_ui->apply, &QPushButton::clicked,
        this, &Preferences::apply);
    connect(m_ui->cancel, &QPushButton::clicked,
        this, &Preferences::cancel);

    auto parent = new QStandardItem("Plugins");
    m_model->appendRow(new QStandardItem("General"));
    m_model->appendRow(new QStandardItem("Session"));
    m_model->appendRow(parent);
    for (const auto plugin : plugins->instances()) {
        parent->appendRow(new QStandardItem(plugin.dynamicCast<QObject>()->objectName()));
    }
    m_ui->treeView->setHeaderHidden(true);
    m_ui->treeView->setModel(m_model);

    QObject::connect(m_ui->treeView, &QTreeView::clicked, this, &Preferences::clicked);

    //temporary
    m_ui->apply->setHidden(true);
    m_ui->cancel->setHidden(true);
    m_ui->ok->setHidden(true);
}

void Preferences::clicked(const QModelIndex& index) {
    const auto name = index.data().toString();
    const auto plugin = m_plugins->findByObjectName(name);
    if (plugin.isNull()) {
        return;
    }
    const auto win = plugin->settingsWindow();
    m_ui->scrollArea->setWidget(win);
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