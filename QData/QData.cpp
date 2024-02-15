#include "QData.h"
#include "../api/api.h"
#include <QApplication>
#include <QTranslator>
#include <QSplitter>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressDialog>
#include <QRegularExpression>

bool QData_register(Window* win, PluginsLoader* ld) {
	if (win == nullptr) {
		return false;
	}

    QCoreApplication* app = QApplication::instance();

    QTranslator* translator = new QTranslator();
    if (translator->load(QLocale::system(), "QData", "_","translations")) { //set directory of ts
        app->installTranslator(translator);
    }

	QAction* database = new QAction(app->translate("MainWindow", "Database"), win);
	database->setData(QVariant("QData"));
	QObject::connect(database, &QAction::triggered, win, &Window::createOrActivate);
  
    QMenu* menu = win->findMenu(app->translate("MainWindow", "&File"));
	if (menu == nullptr) {
		if ((menu = win->menuBar()->addMenu(app->translate("MainWindow", "&File"))) == nullptr) {
			return false;
		}
	}

	QList<QAction*> actions = menu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	menu->insertAction(actions[1], database);
	return true;
}

bool QData_unregister(Window* win, PluginsLoader* ld) {
	return true;
}

REGISTER_PLUGIN(
	QData, 
	Plugin::Type::WIDGET, 
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"example plugin",
	QData_register,
	QData_unregister
)

QData::QData(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath) {
    m_db = QSqlDatabase::database();

    if (!m_db.isValid() || !m_db.isOpen()) {

        QString db = settings().value(this->settingsPath() + "/DataWindow/database", QString(database)).toString();
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(db);

        if (!m_db.open()) {
            qDebug() << "Error: connection with database failed";
        }
    }

    QString tbl = settings().value(this->settingsPath() + "/DataWindow/table", QString(table)).toString();
    if (m_db.tables().contains(tbl, Qt::CaseInsensitive) == false) {
        m_db.exec(QString(createTable).arg(tbl));
    }

    m_view = new QTableView();
    m_view->setShowGrid(true);
    m_view->setSortingEnabled(true);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setAlternatingRowColors(true);
    m_view->setGridStyle(Qt::SolidLine);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_model = new QSqlTableModel(m_view, m_db);
    m_model->setTable(tbl);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_model->setHeaderData(1, Qt::Horizontal, tr("Part"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Cell"));
    m_view->setModel(m_model);
    m_view->setColumnHidden(0, true);

    QLabel* label = new QLabel(tr("Part Number"));
    QPushButton* importCSVButton = new QPushButton(tr("Import &CSV"));
    QPushButton* importDirButton = new QPushButton(tr("Import &Dir"));
    QPushButton* exportButton = new QPushButton(tr("&Export CSV"));

    connect(importCSVButton, &QPushButton::clicked, this, &QData::importFromCsv);
    connect(importDirButton, &QPushButton::clicked, this, &QData::importFromFiles);
    connect(exportButton, &QPushButton::clicked, this, &QData::exportAsCsv);
    connect(m_view, &QTableView::activated, this, &QData::activated);

    m_edit = new QLineEdit();
    m_edit->setMaxLength(24);
    m_edit->setClearButtonEnabled(true);
    connect(m_edit, &QLineEdit::returnPressed, this, &QData::enterPressed);
    connect(m_edit, &QLineEdit::textChanged, this, &QData::textChanged);

    QBoxLayout* buttons = new QHBoxLayout();
    buttons->addWidget(importCSVButton);
    buttons->addWidget(importDirButton);
    buttons->addWidget(exportButton);
    buttons->addStretch();
    buttons->setSpacing(10);

    QBoxLayout* l = new QVBoxLayout();
    l->addWidget(m_view);

    QBoxLayout* h = new QHBoxLayout();
    h->addWidget(label);
    h->addWidget(m_edit);
    h->addStretch(2);
    h->setSpacing(10);

    QPushButton* search = new QPushButton(tr("&Search"));
    connect(search, &QPushButton::pressed, this, &QData::enterPressed);
    h->addWidget(search);

    QPushButton* clear = new QPushButton(tr("C&lear"));
    connect(clear, &QPushButton::pressed, this, &QData::clearForm);
    h->addWidget(clear);

    l->addItem(h);
    
    m_lmodel = new QStandardItemModel(0, 1);
    m_list = new QListView();
    m_list->setModel(m_lmodel);
    m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);

    QBoxLayout* r = new QVBoxLayout();
    r->addWidget(m_list);

    QGridLayout* g = new QGridLayout();
    m_rack = new QLabel();
    m_shelf = new QLabel();
    m_side = new QLabel();

    g->addWidget(new QLabel(tr("Rack:")), 0, 0);
    g->addWidget(m_rack, 0, 1);

    g->addWidget(new QLabel(tr("Shelf:")), 1, 0);
    g->addWidget(m_shelf, 1, 1);

    g->addWidget(new QLabel(tr("Side:")), 2, 0);
    g->addWidget(m_side, 2, 1);

    r->addItem(g);

    QSplitter* split = new QSplitter();
    QGroupBox* left = new QGroupBox(tr("Database"));
    QGroupBox* right = new QGroupBox(tr("History"));
    split->addWidget(left);
    split->addWidget(right);

    left->setLayout(l);
    right->setLayout(r);

    QBoxLayout* z = new QVBoxLayout();
    z->addWidget(split);
    z->addItem(buttons);

    setLayout(z);

    m_timer.setInterval(interval);
    connect(&m_timer, &QTimer::timeout, this, &QData::timeout);
}

void QData::timeout() {
    if (m_selected.isEmpty() != false) {
        auto serial = plugins()->instance("QSerial", nullptr);
        auto io = serial.dynamicCast<IODevice>();

        io->write(m_selected+'\n');
    }
}

void QData::exportAsCsv(bool checked) {
    Q_UNUSED(checked)
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
            "./output.csv",
            tr("Text files (*.csv *.txt)"));

    if (fileName.isEmpty()) {
        return;
    }

    queryToCsv(fileName, QString(exportQuery).arg(table));

    QMessageBox::information(this, tr(title),
        tr("Database export successful"),
        QMessageBox::Ok);
}

void QData::importFromCsv(bool checked) {
    Q_UNUSED(checked)
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
            "./",
            tr("Text files (*.csv *.txt)"));

    if (fileName.isEmpty()) {
        return;
    }

    int ret = QMessageBox::warning(this, tr(title),
        tr("This will wipe all data and cannot be undone"),
        QMessageBox::Ok | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Cancel) {
        return;
    }

    if (!clearData()) {
        QMessageBox::critical(this, tr(title),
            tr("Failed to clear existing data"),
            QMessageBox::Ok);

        return;
    }

    QFile input(fileName);

    if (!input.open(QIODevice::ReadOnly)) {
        qDebug() << input.errorString();
    }

    QTextStream in(&input);
    QProgressDialog progress(tr("Importing database from CSV"), tr("Cancel"), 0, 100);
    progress.setWindowModality(Qt::WindowModal);
    qint64 size = input.size();
    qint64 i = 0;

    while (!in.atEnd()) {
        progress.setValue((100*i)/size);

        if (progress.wasCanceled()) {
            break;
        }

        QString line = in.readLine();
        i += line.size();
        const QStringList columns = line.split(",");

        QSqlQuery query;
        query.prepare(QString("INSERT INTO %1 (id, part, shelf) VALUES (%2, %3, %4)")
            .arg(table).arg(columns[0]).arg(columns[1]).arg(columns[2]));

        if (!query.exec()) {
            qDebug() << "Insert error: " << query.lastError().text();
        }
    }
    progress.setValue(100);
    input.close();
    m_model->select();
}

void QData::importFromFiles(bool checked) {
    Q_UNUSED(checked)
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
            "./",
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);

    int ret = QMessageBox::warning(this, tr(title),
        tr("This will wipe all data and cannot be undone"),
        QMessageBox::Ok | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Cancel) {
        return;
    }

    if (!clearData()) {
        QMessageBox::critical(this, tr(title),
            tr("Failed to clear existing data"),
            QMessageBox::Ok);

        return;
    }

    qsizetype count = QDir(dir).entryList({ "*.csv", "*.txt" }, QDir::Files | QDir::NoSymLinks | QDir::Readable).count();
    QProgressDialog progress(tr("Importing database from directory"), tr("Cancel"), 0, count);
    progress.setWindowModality(Qt::WindowModal);

    QDirIterator it(dir, { "*.csv", "*.txt" }, QDir::Files | QDir::NoSymLinks | QDir::Readable, QDirIterator::Subdirectories);
    qsizetype i = 0;
    while (it.hasNext()) {
        progress.setValue(i++);

        if (progress.wasCanceled()) {
            break;
        }

        QString path = it.next();
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "error";
            continue;
        }

        QByteArray contents = file.readAll();
        QFileInfo info(file);
        QString part = info.baseName();
        QString shelf(contents);

        qDebug() << part << shelf;
        QSqlQuery query;
        query.prepare(QString("INSERT INTO %1 (part, shelf) VALUES (%2, %3)").arg(table).arg(part).arg(shelf));

        if (!query.exec()) {
            qDebug() << "Insert error: " << query.lastError().text();
        }
    }
    progress.setValue(count);
    m_model->select();
}

QString QData::escapedCSV(QString unexc) {
    if (!unexc.contains(QLatin1Char(',')))
        return unexc;
    return '\"' + unexc.replace(QLatin1Char('\"'), QStringLiteral("\"\"")) + '\"';
}

void QData::queryToCsv(const QString& path, const QString& queryStr) {
    QSqlQuery query(m_db);
    query.prepare(queryStr);
    QFile csvFile(path);

    if (!csvFile.open(QFile::WriteOnly | QFile::Text)) {
        qDebug("failed to open csv file");
        return;
    }

    if (!query.exec()) {
        qDebug("failed to run query");
        return;
    }

    QTextStream outStream(&csvFile);
    outStream.setEncoding(QStringConverter::Utf8);
    int count = query.numRowsAffected();
    QProgressDialog progress(tr("Exporting database to CSV"), tr("Cancel"), 0, count);
    progress.setWindowModality(Qt::WindowModal);
    int i = 0;

    while (query.next()) {
        progress.setValue(i++);

        if (progress.wasCanceled()) {
            break;
        }

        const QSqlRecord record = query.record();
        for (int i = 0, recCount = record.count(); i < recCount; ++i) {
            if (i > 0) {
                outStream << ',';
            }

            outStream << escapedCSV(record.value(i).toString());
        }
        outStream << '\n';
    }

    progress.setValue(count);
}

void QData::settingsChanged() {}

void QData::prepareForFocus() {
    m_edit->setFocus();
}

bool QData::clearData() {
    QSqlQuery query(m_db);
    return query.exec(QString("DELETE FROM %1").arg(table));
}

void QData::activated(const QModelIndex& idx) {
    QModelIndex didx = idx.siblingAtColumn(2);
    QVariant data = didx.data();

    if (data.toString().isEmpty() == false) {
        m_selected = data.toString();
        m_timer.start();

        m_lmodel->appendRow(new QStandardItem(idx.siblingAtColumn(1).data().toString()));
        fillInfo();
    }
}

void QData::enterPressed() {
    QString str = m_edit->text();

    if (str.isEmpty()) {
        return;
    }

    QRegularExpression regex("^[0-9]+[\\\\\\/]([0-9\\.]+).+$");
    auto match = regex.match(str);

    if (match.hasMatch() && match.capturedLength() > 2) {
        auto res = match.captured(1);
        res.remove('.');
        m_edit->setText(res);
        str = res;
    }

    m_model->setFilter("part LIKE '%" + str + "%'");
    m_model->select();

    int row = findByPart(m_model, str);
    if (row != -1) {
        QModelIndex idx = m_model->index(row, 2);
        m_view->scrollTo(idx);
        m_view->selectRow(row);
        m_selected = idx.data().toString();
        m_timer.start();

        m_lmodel->appendRow(new QStandardItem(m_model->index(row, 1).data().toString()));
        fillInfo();
    }
}