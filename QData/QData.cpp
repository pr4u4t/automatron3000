#include "QData.h"
#include "../api/api.h"
#include <QApplication>
#include <QTranslator>
#include <QSplitter>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressDialog>
#include <QRegularExpression>

struct QDataMenu {
    QDataMenu(QCoreApplication* app)
    : m_app(app){
        m_translator = new QTranslator();
        if (m_translator->load(QLocale::system(), "QData", "_", "translations")) { //set directory of ts
            m_app->installTranslator(m_translator);
        }

        m_database = new QAction(m_app->translate("MainWindow", "Database"), nullptr);
        m_database->setData(QVariant("QData"));
    }

    QAction* m_database = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool QData_register(Window* win, PluginsLoader* ld, QDataMenu* ctx, Logger* log) {
    log->message("QData_register()");

	if (win == nullptr) {
		return false;
	}

    QMenu* fileMenu = win->findMenu(ctx->m_app->translate("MainWindow", "&File"));
    ctx->m_database->setParent(fileMenu);
	
	QObject::connect(ctx->m_database, &QAction::triggered, win, &Window::createOrActivate);
  
	QList<QAction*> actions = fileMenu->findChildren<QAction*>(Qt::FindDirectChildrenOnly);
	fileMenu->insertAction(actions.size() > 0 ? actions[1] : nullptr, ctx->m_database);

	return true;
}

static bool QData_unregister(Window* win, PluginsLoader* ld, QDataMenu* ctx, Logger* log) {
    log->message("QData_unregister()");
    return true;
}

REGISTER_PLUGIN(
	QData, 
	Plugin::Type::WIDGET, 
	"0.0.1",
	"pawel.ciejka@gmail.com",
	"example plugin",
	QData_register,
	QData_unregister,
    QDataMenu
)

QData::QData(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
    : Widget(ld, plugins, parent, settingsPath) {
    m_db = QSqlDatabase::database();

    if (!m_db.isValid() || !m_db.isOpen()) {

        QString db = settings().value(this->settingsPath() + "/DataWindow/database", QString(database)).toString();
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(db);

        emit message(QString("QData::QData: connection to database: %1").arg(m_db.open() ? tr("success") : tr("failed")));
    }

    QString tbl = settings().value(this->settingsPath() + "/DataWindow/table", QString(table)).toString();
    if (m_db.tables().contains(tbl, Qt::CaseInsensitive) == false) {
        m_db.exec(QString(createTable).arg(tbl));
    } else {
        emit message("QData::QData: creating database");
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
    emit message("QData::timeout()", LoggerSeverity::LOG_DEBUG);

    if (m_selected.isEmpty() == true) {
        emit message("QData::timeout: selected barcode empty");
        return;
    }
        
    auto serial = plugins()->instance("QSerial", nullptr);
    auto io = serial.dynamicCast<IODevice>();

    if (io->isOpen() == false) {
        emit message("QData::timeout: serial port not open");
        return;
    }

    auto res = io->write(m_selected + '\n');
    emit message(QString("QData::timeout: write %1").arg(res));
}

void QData::exportAsCsv(bool checked) {
    Q_UNUSED(checked)
        
    emit message("QData::exportAsCsv()", LoggerSeverity::LOG_DEBUG);

    QString fileName = QFileDialog::getSaveFileName(this, 
        tr("Save File"),
        "./output.csv",
        tr("Text files (*.csv *.txt)")
    );

    if (fileName.isEmpty()) {
        emit message("QData::exportAsCsv: export cancelled");
        return;
    }

    queryToCsv(fileName, QString(exportQuery).arg(table));

    QMessageBox::information(this, tr(title),
        tr("Database export successful"),
        QMessageBox::Ok);

    emit message("QData::exportAsCsv: Database export successful");
}

void QData::importFromCsv(bool checked) {
    Q_UNUSED(checked)
    
    emit message("QData::importfromCsv()");

    QString fileName = QFileDialog::getOpenFileName(this, 
            tr("Open File"),
            "./",
            tr("Text files (*.csv *.txt)")
    );

    if (fileName.isEmpty()) {
        emit message("QData::importfromCsv: import cancelled");
        return;
    }

    int ret = QMessageBox::warning(this, tr(title),
        tr("This will wipe all data and cannot be undone"),
        QMessageBox::Ok | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Cancel) {
        emit message("QData::importfromCsv: import cancelled");
        return;
    }

    if (!clearData()) {
        QMessageBox::critical(this, tr(title),
            tr("Failed to clear existing data"),
            QMessageBox::Ok);
        emit message(QString("QData::importfromCsv: failed to clear existing data"));
        return;
    }

    QFile input(fileName);

    if (!input.open(QIODevice::ReadOnly)) {
        emit message(QString("QData::importFromCsv: %1").arg(input.errorString()));
        return;
    }

    QTextStream in(&input);
    QProgressDialog progress(tr("Importing database from CSV"), tr("Cancel"), 0, 100);
    progress.setWindowModality(Qt::WindowModal);
    qint64 size = input.size();
    qint64 i = 0;

    while (!in.atEnd()) {
        progress.setValue((100*i)/size);

        if (progress.wasCanceled()) {
            emit message("QData::importFromCsv: operation cancelled");
            break;
        }

        QString line = in.readLine();
        i += line.size();
        const QStringList columns = line.split(",");

        QSqlQuery query;
        query.prepare(QString("INSERT INTO %1 (id, part, shelf) VALUES (%2, %3, %4)")
            .arg(table).arg(columns[0]).arg(columns[1]).arg(columns[2]));

        if (!query.exec()) {
            emit message(QString("QData::importFromcsv: insert error: %1").arg(query.lastError().text()));
        }
    }
    progress.setValue(100);
    input.close();
    m_model->select();
}

void QData::importFromFiles(bool checked) {
    Q_UNUSED(checked)
    
    emit message("QData::importFromFiles", LoggerSeverity::LOG_DEBUG);
    
    QString dir = QFileDialog::getExistingDirectory(this, 
        tr("Open Directory"),
        "./",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (dir.isEmpty()) {
        emit message("QData::importFromFiles: operation cancelled");
        return;
    }

    int ret = QMessageBox::warning(this, tr(title),
        tr("This will wipe all data and cannot be undone"),
        QMessageBox::Ok | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Cancel) {
        emit message("QData::importFromFiles: operation cancelled");
        return;
    }

    if (!clearData()) {
        QMessageBox::critical(this, tr(title),
            tr("Failed to clear existing data"),
            QMessageBox::Ok);
        emit message("QData::importFromCsv: failed to clear existing data");
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
            emit message("QData::importFromCsv: operation cancelled");
            break;
        }

        QString path = it.next();
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly)) {
            emit message(QString("QData::importFromCsv: failed to open %1").arg(path));
            continue;
        }

        QByteArray contents = file.readAll();
        QFileInfo info(file);
        QString part = info.baseName();
        QString shelf(contents);

        QSqlQuery query;
        query.prepare(QString("INSERT INTO %1 (part, shelf) VALUES (%2, %3)").arg(table).arg(part).arg(shelf));

        if (!query.exec()) {
            emit message(QString("QData::importFromCsv: insert error: ").arg(query.lastError().text()));
        }
    }
    progress.setValue(count);
    m_model->select();
}

QString QData::escapedCSV(QString unexc) {
    if (!unexc.contains(QLatin1Char(','))) {
        return unexc;
    }

    return '\"' + unexc.replace(QLatin1Char('\"'), QStringLiteral("\"\"")) + '\"';
}

void QData::queryToCsv(const QString& path, const QString& queryStr) {
    emit message("QData::queryToCsv()", LoggerSeverity::LOG_DEBUG);
    QSqlQuery query(m_db);
    query.prepare(queryStr);
    QFile csvFile(path);

    if (!csvFile.open(QFile::WriteOnly | QFile::Text)) {
        emit message("QData::queryToCsv: failed to open output csv file");
        return;
    }

    if (!query.exec()) {
        emit message("QData::queryToCsv: failed to run query");
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
            emit message("QData::queryToCsv: operation cancelled");
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
    emit message("QData::QData::prepareForFocus()");
    m_edit->setFocus();
}

bool QData::clearData() {
    emit message("QData::clearData()", LoggerSeverity::LOG_DEBUG);

    QSqlQuery query(m_db);
    if (query.exec(QString("DELETE FROM %1").arg(table)) == true) {
        emit message(QString("QData::clearData(): rows affected %1").arg(query.numRowsAffected()));
        return true;
    }
    
    emit message("QData::clearData(): query failed");
    return false;
}

void QData::activated(const QModelIndex& idx) {
    emit message("QData::activated()");

    QModelIndex didx = idx.siblingAtColumn(2);
    QVariant data = didx.data();

    if (data.toString().isEmpty() == false) {
        emit message(QString("QData::activated: data %1 found").arg(data.toString()));
        m_selected = data.toString();

        m_lmodel->appendRow(new QStandardItem(idx.siblingAtColumn(1).data().toString()));
        fillInfo();

        m_timer.start();

        emit message("QData::enterPressed(): timer started");
    }
}

void QData::enterPressed() {
    emit message("QData::enterPressed()", LoggerSeverity::LOG_DEBUG);
    QString str = m_edit->text();

    if (str.isEmpty()) {
        emit message("QData::enterPressed(): input is empty");
        return;
    }

    QRegularExpression regex("^[0-9]+[\\\\\\/]([0-9\\.]+).+$");
    auto match = regex.match(str);

    if (match.hasMatch() && match.capturedLength() > 2) {
        emit message("QData::enterPressed(): input from barcode reader");
        auto res = match.captured(1);
        res.remove('.');
        m_edit->setText(res);
        str = res;
    }

    m_model->setFilter("part LIKE '%" + str + "%'");
    m_model->select();

    int row = findByPart(m_model, str);
    emit message(QString("QData::enterPressed(): find by part: %1").arg(row));

    if (row != -1) {
        QModelIndex idx = m_model->index(row, 2);
        m_view->scrollTo(idx);
        m_view->selectRow(row);
        m_selected = idx.data().toString();

        m_lmodel->appendRow(new QStandardItem(m_model->index(row, 1).data().toString()));
        fillInfo();

        m_timer.start();
        emit message("QData::enterPressed(): timer started");
    }
}