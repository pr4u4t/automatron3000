#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent)
    , m_currentSettings(Settings::get(), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    //, m_intValidator(new QIntValidator(0, 4000000, this))
    , m_settingsPath(settingsPath)
    , m_model(new QStandardItemModel(0, 3)) {
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    connect(m_ui->okButton, &QPushButton::clicked,
        this, &SettingsDialog::ok);
    connect(m_ui->applyButton, &QPushButton::clicked,
        this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked,
        this, &SettingsDialog::cancel);

    m_model->setHeaderData(0, Qt::Horizontal, tr("Object Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Function"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Enabled"));

    m_ui->tableView->setShowGrid(true);
    m_ui->tableView->setAlternatingRowColors(true);
    m_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->tableView->setModel(m_model);

    m_ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->tableView, SIGNAL(customContextMenuRequested(QPoint)),
        SLOT(customMenuRequested(QPoint)));

    fillFromSettings();
}

SettingsDialog::~SettingsDialog() {}

CustomActionSettings SettingsDialog::dataSettings() const {
    return m_currentSettings;
}

QString SettingsDialog::settingsPath() const {
    return m_settingsPath;
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    m_ui->buttonEdit->setText(m_currentSettings.buttonText());
    m_ui->titleEdit->setText(m_currentSettings.title());
    m_ui->verboseCheckbox->setChecked(m_currentSettings.verbose());
    m_ui->progressCheckbox->setChecked(m_currentSettings.progress());
    m_ui->intervalSpin->setValue(m_currentSettings.interval());

    fillModel(m_currentSettings.pluginsActions());
}

void SettingsDialog::customMenuRequested(QPoint point) {
    QMenu menu;
    menu.addAction(tr("Add"), this, &SettingsDialog::addArgument);
    menu.addAction(tr("Insert Before"), this, &SettingsDialog::insBeforeArgument);
    menu.addAction(tr("Insert After"), this, &SettingsDialog::insAfterArgument);
    menu.addSeparator();
    menu.addAction(tr("Remove"), this, &SettingsDialog::removeArgument);
    menu.exec(m_ui->tableView->mapToGlobal(point));
}

void SettingsDialog::insBeforeArgument() {
    QItemSelectionModel* selected = m_ui->tableView->selectionModel();

    if (selected->hasSelection() != true) {
        return;
    }

    QModelIndexList list = selected->selectedRows();

    if (list.size() != 1) {
        return;
    }

    QList<QStandardItem*> items;
    items << new QStandardItem() << new QStandardItem();
    QStandardItem* check = new QStandardItem();
    check->setCheckable(true);
    check->setCheckState(Qt::Checked);
    items << check;
    m_model->insertRow(list[0].row(), items);
}

void SettingsDialog::insAfterArgument() {
    QItemSelectionModel* selected = m_ui->tableView->selectionModel();

    if (selected->hasSelection() != true) {
        return;
    }

    QModelIndexList list = selected->selectedRows();

    if (list.size() != 1) {
        return;
    }

    QList<QStandardItem*> items;
    items << new QStandardItem() << new QStandardItem();
    QStandardItem* check = new QStandardItem();
    check->setCheckable(true);
    check->setCheckState(Qt::Checked);
    items << check;
    m_model->insertRow(list[0].row() + 1, items);
}

void SettingsDialog::addArgument() {
    QList<QStandardItem*> items;
    items << new QStandardItem() << new QStandardItem();
    QStandardItem* check = new QStandardItem();
    check->setCheckable(true);
    check->setCheckState(Qt::Checked);
    items << check;

    m_model->appendRow(items);
}

void SettingsDialog::removeArgument() {
    QItemSelectionModel* selected = m_ui->tableView->selectionModel();

    if (selected->hasSelection() != true) {
        return;
    }

    QModelIndexList list = selected->selectedRows();

    if (list.size() != 1) {
        return;
    }

    m_model->removeRow(list[0].row());
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    m_currentSettings.setButtonText(m_ui->buttonEdit->text());
    m_currentSettings.setTitle(m_ui->titleEdit->text());
    m_currentSettings.setPluginsActions(actions<QJsonArray>(m_model));
    m_currentSettings.setVerbose(m_ui->verboseCheckbox->isChecked());
    m_currentSettings.setProgress(m_ui->progressCheckbox->isChecked());
    m_currentSettings.setInterval(m_ui->intervalSpin->value());

    QSettings s = Settings::get();
    m_currentSettings.save(s, settingsPath());
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