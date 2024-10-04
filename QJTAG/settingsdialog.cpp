#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent)
    , m_currentSettings(Settings::get(), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_settingsPath(settingsPath)
    , m_model(new QStandardItemModel(0, 3)){
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    m_model->setHeaderData(0, Qt::Horizontal, tr("Argument"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Value"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Enabled"));

    m_ui->arguments->setShowGrid(true);
    m_ui->arguments->setAlternatingRowColors(true);
    m_ui->arguments->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->arguments->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->arguments->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->arguments->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->arguments->setModel(m_model);

    m_ui->arguments->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->arguments, SIGNAL(customContextMenuRequested(QPoint)),
        SLOT(customMenuRequested(QPoint)));

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
    m_ui->triesEdit->setValue(m_currentSettings.tries);
    fillModel(m_currentSettings.arguments);
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");

    m_currentSettings.programPath = m_ui->programPathEdit->text();
    m_currentSettings.buttonLabel = m_ui->buttonLabelEdit->text();
    m_currentSettings.title = m_ui->titleEdit->text();
    m_currentSettings.arguments = arguments<QJsonArray>(m_model);
    m_currentSettings.tries = m_ui->triesEdit->value();

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

void SettingsDialog::customMenuRequested(QPoint point) {
    QMenu menu;
    menu.addAction(tr("Add"), this, &SettingsDialog::addArgument);
    menu.addAction(tr("Insert Before"), this, &SettingsDialog::insBeforeArgument);
    menu.addAction(tr("Insert After"), this, &SettingsDialog::insAfterArgument);
    menu.addSeparator();
    menu.addAction(tr("Remove"), this, &SettingsDialog::removeArgument);
    menu.exec(m_ui->arguments->mapToGlobal(point));
}

void SettingsDialog::insBeforeArgument() {
    QItemSelectionModel* selected = m_ui->arguments->selectionModel();

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
    QItemSelectionModel* selected = m_ui->arguments->selectionModel();

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
    m_model->insertRow(list[0].row()+1, items);
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
    QItemSelectionModel* selected = m_ui->arguments->selectionModel();

    if (selected->hasSelection() != true) {
        return;
    }

    QModelIndexList list = selected->selectedRows();

    if (list.size() != 1) {
        return;
    }

    m_model->removeRow(list[0].row());
}