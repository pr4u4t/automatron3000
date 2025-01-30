#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QExternalAction.h"

#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new QExternalActionSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_model(new QStandardItemModel(0, 3)){
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QExternalAction* action)
    : SettingsMdi(parent, new QExternalActionSettings(*(action->settings<QExternalActionSettings>())), action->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this))
    , m_model(new QStandardItemModel(0, 3)) {
    setup();
}

void SettingsDialog::setup() {
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

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");

    const QExternalActionSettings* setts = settings<QExternalActionSettings>();

    m_ui->programPathEdit->setText(setts->programPath());
    m_ui->buttonLabelEdit->setText(setts->buttonLabel());
    m_ui->titleEdit->setText(setts->title());
    m_ui->triesEdit->setValue(setts->tries());
    m_ui->previousEdit->setText(setts->previous());
    fillModel(setts->arguments());
}

SettingsDialog::operator QExternalActionSettings() const {
    QExternalActionSettings ret;

    ret.setProgramPath(m_ui->programPathEdit->text());
    ret.setButtonLabel(m_ui->buttonLabelEdit->text());
    ret.setTitle(m_ui->titleEdit->text());
    ret.setArguments(arguments<QJsonArray>(m_model));
    ret.setTries(m_ui->triesEdit->value());
    ret.setPrevious(m_ui->previousEdit->text());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings");
    QExternalActionSettings newSettings = *this;
    QExternalActionSettings* setts = settings<QExternalActionSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<QExternalActionSettings>(settingsPath(), setts);
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