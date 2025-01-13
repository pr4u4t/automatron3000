#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QCustomAction.h"

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new CustomActionSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    //, m_intValidator(new QIntValidator(0, 4000000, this))
    , m_model(new QStandardItemModel(0, 3)) {
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QCustomAction* action)
    : SettingsMdi(parent, new CustomActionSettings(*(action->settings<CustomActionSettings>())), action->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    //, m_intValidator(new QIntValidator(0, 4000000, this))
    , m_model(new QStandardItemModel(0, 3)) {
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

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    CustomActionSettings* setts = settings<CustomActionSettings>();

    m_ui->buttonEdit->setText(setts->buttonText());
    m_ui->titleEdit->setText(setts->title());
    m_ui->verboseCheckbox->setChecked(setts->verbose());
    m_ui->progressCheckbox->setChecked(setts->progress());
    m_ui->intervalSpin->setValue(setts->interval());

    fillModel(setts->pluginsActions());
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

SettingsDialog::operator CustomActionSettings() const {
    CustomActionSettings ret;
    ret.setButtonText(m_ui->buttonEdit->text());
    ret.setTitle(m_ui->titleEdit->text());
    ret.setPluginsActions(actions<QJsonArray>(m_model));
    ret.setVerbose(m_ui->verboseCheckbox->isChecked());
    ret.setProgress(m_ui->progressCheckbox->isChecked());
    ret.setInterval(m_ui->intervalSpin->value());
    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");
    CustomActionSettings* setts = settings<CustomActionSettings>();

    CustomActionSettings newSettings = *this;
    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    *setts = newSettings;
    Settings::store<CustomActionSettings>(settingsPath(), setts);
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