#include "settingsdialog.h"

// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "QDataTable.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QJsonObject>

//#include "DockManager.h"
#include <QCloseEvent>

ComboBoxItemDelegate::ComboBoxItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

ComboBoxItemDelegate::~ComboBoxItemDelegate() {}

QWidget* ComboBoxItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // Create the combobox and populate it
    QComboBox* cb = new QComboBox(parent);
    const int row = index.row();
    cb->addItem("int64");
    cb->addItem("signed int64");
    cb->addItem("unsigned int64");
    cb->addItem("int32");
    cb->addItem("unsigned int32");
    cb->addItem("signed int32");
    cb->addItem("int16");
    cb->addItem("unsigned int16");
    cb->addItem("signed int16");
    cb->addItem("int8");
    cb->addItem("unsigned int8");
    cb->addItem("signed int8");
    cb->addItem("float");
    cb->addItem("double");

    return cb;
}

void ComboBoxItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QComboBox* cb = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(cb);
    // get the index of the text in the combobox that matches the current value of the item
    const QString currentText = index.data(Qt::EditRole).toString();
    const int cbIndex = cb->findText(currentText);
    // if it is valid, adjust the combobox
    if (cbIndex >= 0)
        cb->setCurrentIndex(cbIndex);
}


void ComboBoxItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QComboBox* cb = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(cb);
    model->setData(index, cb->currentText(), Qt::EditRole);
}

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath)
    : SettingsMdi(parent, new DataTableSettings(Settings::get(), settingsPath), settingsPath)
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) 
    , m_model(new QStandardItemModel(0, 3)){
    setup();
}

SettingsDialog::SettingsDialog(QWidget* parent, const QDataTable* table)
    : SettingsMdi(parent, new DataTableSettings(*(table->settings<DataTableSettings>())), table->settingsPath())
    , m_ui(new Ui::SettingsDialog)
    , m_intValidator(new QIntValidator(0, 4000000, this)) 
    , m_model(new QStandardItemModel(0, 3)) {
    setup();
}

void SettingsDialog::setup() {
    emit message("SettingsDialog::SettingsDialog");
    m_ui->setupUi(this);

    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Type"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Enabled"));

    m_ui->columns->setShowGrid(true);
    m_ui->columns->setAlternatingRowColors(true);
    m_ui->columns->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->columns->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->columns->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->columns->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui->columns->setModel(m_model);

    ComboBoxItemDelegate* cbid = new ComboBoxItemDelegate();
    m_ui->columns->setItemDelegateForColumn(1, cbid);

    m_ui->columns->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->columns, SIGNAL(customContextMenuRequested(QPoint)),
        SLOT(customMenuRequested(QPoint)));

    connect(m_ui->okButton, &QPushButton::clicked,
        this, &SettingsDialog::ok);
    connect(m_ui->applyButton, &QPushButton::clicked,
        this, &SettingsDialog::apply);
    connect(m_ui->cancelButton, &QPushButton::clicked,
        this, &SettingsDialog::cancel);

    fillFromSettings();
}

void SettingsDialog::customMenuRequested(QPoint point) {
    QMenu menu;
    menu.addAction(tr("Add"), this, &SettingsDialog::addColumn);
    menu.addAction(tr("Insert Before"), this, &SettingsDialog::insBeforeColumn);
    menu.addAction(tr("Insert After"), this, &SettingsDialog::insAfterColumn);
    menu.addSeparator();
    menu.addAction(tr("Remove"), this, &SettingsDialog::removeColumn);
    menu.exec(m_ui->columns->mapToGlobal(point));
}

void SettingsDialog::insBeforeColumn() {
    QItemSelectionModel* selected = m_ui->columns->selectionModel();

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

void SettingsDialog::insAfterColumn() {
    QItemSelectionModel* selected = m_ui->columns->selectionModel();

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

void SettingsDialog::addColumn() {
    QList<QStandardItem*> items;
    items << new QStandardItem() << new QStandardItem();
    QStandardItem* check = new QStandardItem();
    check->setCheckable(true);
    check->setCheckState(Qt::Checked);
    items << check;

    m_model->appendRow(items);
}

void SettingsDialog::removeColumn() {
    QItemSelectionModel* selected = m_ui->columns->selectionModel();

    if (selected->hasSelection() != true) {
        return;
    }

    QModelIndexList list = selected->selectedRows();

    if (list.size() != 1) {
        return;
    }

    m_model->removeRow(list[0].row());
}

SettingsDialog::~SettingsDialog() {
    if (m_ui) {
        delete m_ui;
    }
}

void SettingsDialog::fillFromSettings() {
    emit message("SettingsDialog::fillFromSettings");
    const DataTableSettings* setts = settings<DataTableSettings>();
    
    auto array = setts->columns();
    for (auto begin = array.begin(), end = array.end(); begin != end; ++begin) {
        if (begin->isObject() == false) {
            continue;
        }
    
        auto object = begin->toObject();

        if (object.contains("name") == false || object.contains("type") == false) {
            continue;
        }

        QList<QStandardItem*> list;
        list << new QStandardItem(object["name"].toString()) << new QStandardItem(object["type"].toString());
        auto item = new QStandardItem();
        item->setCheckable(true);
        item->setCheckState(object["enabled"].toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        list << item;
        m_model->appendRow(list);
    }

    m_ui->mode->setCurrentIndex(static_cast<int>(setts->mode()));
    m_ui->newLine->setText(setts->newLine());
    m_ui->separator->setText(setts->separator());
    m_ui->input->setText(setts->input());
    m_ui->titleEdit->setText(setts->title());
    m_ui->magic->setText(setts->magic().toHex());
    m_ui->maxRows->setValue(setts->maxRows());
}

SettingsDialog::operator DataTableSettings() const {
    DataTableSettings ret;

    ret.setTitle(m_ui->titleEdit->text());

    if (m_ui->mode->currentIndex() == 0) {
        ret.setMode(DataTableSettings::DataTableMode::BINARY);
    } else {
        ret.setMode(DataTableSettings::DataTableMode::TEXT);
    }

    ret.setNewLine(m_ui->newLine->text());
    ret.setSeparator(m_ui->separator->text());

    QJsonArray array;
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QJsonObject o;
        o["name"] = m_model->data(m_model->index(row, 0)).toString();
        o["type"] = m_model->data(m_model->index(row,1)).toString();
        auto state = m_model->data(m_model->index(row, 2), Qt::CheckStateRole).toInt();
        o["enabled"] = state == Qt::CheckState::Checked ? true : false;
        array.append(o);
    }

    ret.setColumns(array);
    ret.setInput(m_ui->input->text());
    ret.setTitle(m_ui->titleEdit->text());
    ret.setMagic(QByteArray::fromHex(m_ui->magic->text().toLocal8Bit()));
    ret.setMaxRows(m_ui->maxRows->value());

    return ret;
}

void SettingsDialog::updateSettings() {
    emit message("SettingsDialog::updateSettings", LoggerSeverity::LOG_DEBUG);

    DataTableSettings newSettings = *this;
    DataTableSettings* setts = settings<DataTableSettings>();

    if (newSettings == *setts) {
        emit message("SettingsDialog::updateSettings: settings not changed");
        return;
    }

    //QSettings s = Settings::get();
    //newSettings.save(s, settingsPath());
    newSettings.setObjectName(setts->objectName());
    *setts = newSettings;
    //Settings::store<LinWriteByIDSettings>(settingsPath(), setts);
}

void SettingsDialog::ok() {
    emit message("SettingsDialog::ok");

    updateSettings();
    close();
    emit settingsUpdated();
}

void SettingsDialog::apply() {
    emit message("SettingsDialog::apply");

    updateSettings();
    emit settingsUpdated();
}

void SettingsDialog::cancel() {
    emit message("SettingsDialog::cancel");
    close();
}