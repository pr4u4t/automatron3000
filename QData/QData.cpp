#include <QApplication>
#include <QTranslator>
#include <QSplitter>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QCryptographicHash>

#include "QData.h"
#include "../api/api.h"
#include "SettingsDialog.h"
#include "passworddialog.h"

struct QDataMenu {
    QDataMenu(QCoreApplication* app)
    : m_app(app){
        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "QData", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }

        m_dataMenu = new QMenu(m_app->translate("MainWindow", "&Data"));

        m_database = new QAction(m_app->translate("MainWindow", "Database"), m_dataMenu);
        m_database->setData(QVariant("QData"));
        m_dataMenu->addAction(m_database);
        m_dbSettings = new QAction(m_app->translate("MainWindow", "Settings"), m_dataMenu);
        m_dataMenu->addAction(m_dbSettings);
    }

    QMenu* m_dataMenu = nullptr;
    QAction* m_dbSettings = nullptr;
    QAction* m_database = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;

};

static bool QData_register(Window* win, PluginsLoader* ld, QDataMenu* ctx, Logger* log) {
    log->message("QData_register()");

	if (win == nullptr) {
		return false;
	}

    QMenu* windowMenu = win->findMenu(ctx->m_app->translate("MainWindow", "&Settings"));
    if (windowMenu != nullptr) {
        win->menuBar()->insertMenu(windowMenu->menuAction(), ctx->m_dataMenu);
    }

	QObject::connect(ctx->m_database, &QAction::triggered, win, &Window::createOrActivate);

    QObject::connect(ctx->m_dbSettings, &QAction::triggered, [ld, win, ctx] {
        QSharedPointer<Plugin> data = ld->instance("QData", win);
        SettingsDialog* dialog = new SettingsDialog(win, nullptr, data->settingsPath());
        QObject::connect(dialog, &SettingsDialog::settingsUpdated, data.dynamicCast<QData>().data(), &QData::settingsChanged);
        win->addSubWindow(dialog, ctx->m_app->translate("MainWindow", "Database-Settings"));
    });

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
    QDataMenu,
    {"QSerial"}
)

QData::QData(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path)
    : Widget(ld, plugins, parent, path)
    , m_ui(new Ui::QDataUI) {
    m_ui->setupUi(this);
    m_db = QSqlDatabase::database();

    m_ui->dbView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_lmodel = new QStandardItemModel(0, 1);
    m_ui->historyView->setModel(m_lmodel);

    connect(m_ui->importCsvButton, &QPushButton::clicked, this, &QData::importFromCsv);
    connect(m_ui->importDirButton, &QPushButton::clicked, this, &QData::importFromFiles);
    connect(m_ui->exportCSVButton, &QPushButton::clicked, this, &QData::exportAsCsv);
    connect(m_ui->dbView, &QTableView::activated, this, &QData::activated);
    connect(m_ui->barcodeEdit, &QLineEdit::returnPressed, this, &QData::enterPressed);
    connect(m_ui->barcodeEdit, &QLineEdit::textChanged, this, &QData::textChanged);
    connect(m_ui->barcodeEdit, &QLineEdit::returnPressed, this, &QData::enterPressed);
    connect(m_ui->barcodeEdit, &QLineEdit::textChanged, this, &QData::textChanged);
    connect(m_ui->searchButton, &QPushButton::pressed, this, &QData::enterPressed);
    connect(m_ui->clearButton, &QPushButton::pressed, this, &QData::clearForm);
    connect(m_ui->unlockButton, &QPushButton::pressed, this, &QData::toggleLock);

    if (m_settings.serialInterval != -1) {
        m_timer.setInterval(m_settings.serialInterval);
    }

    connect(&m_timer, &QTimer::timeout, this, &QData::timeout);
    settingsChanged();

    QTimer::singleShot(0, this, &QData::timeout);
}

void QData::timeout() {
    emit message("QData::timeout()", LoggerSeverity::LOG_DEBUG);

    auto serial = plugins()->instance("QSerial", nullptr);
    auto io = serial.dynamicCast<IODevice>();

    if (io->isOpen() == false) {
        emit message("QData::timeout: serial port not open");
        if (io->open() == false) {
            emit message("QData::timeout: failed to open serial port");
            return;
        }

        emit message("QData::timeout: serial port open success");
    }

    if (m_selected.isEmpty() == true) {
        emit message("QData::timeout: selected barcode empty");
        return;
    }
        
    if (m_settings.serialPrefix.isEmpty() == false) {
        auto res = io->write(m_settings.serialPrefix + '\n');
        emit message(QString("QData::timeout: prefix write %1 %2").arg(res).arg(m_settings.serialPrefix));
    }

    auto res = io->write(m_selected + '\n');
    emit message(QString("QData::timeout: write %1 %2").arg(res).arg(m_selected));
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

    queryToCsv(fileName, QString(exportQuery).arg(m_settings.dbTable));

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

    QFile input(fileName);
    QTextStream in(&input);

    QByteArray testLine = input.peek(4096);
    QByteArrayList list = testLine.split('\n');

    if (list.size() == 1) {
        emit message("QData::importfromCsv(): no lines detected in input invalid format");
        return;
    }

    QByteArrayList columns = list[0].split(',');
    if (columns.size() != 3) {
        emit message(QString("QData::importfromCsv(): invalid number of columns detected in input %1 columns should be separated with ',' sign").arg(columns.size()));
        return;
    }

    if (!clearData()) {
        QMessageBox::critical(this, tr(title),
            tr("Failed to clear existing data"),
            QMessageBox::Ok);
        emit message(QString("QData::importfromCsv: failed to clear existing data"));
        return;
    }

    

    if (!input.open(QIODevice::ReadOnly)) {
        emit message(QString("QData::importFromCsv: %1").arg(input.errorString()));
        return;
    }

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
            .arg(m_settings.dbTable)
            .arg(columns[0])
            .arg(columns[1])
            .arg(columns[2])
        );

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
        query.prepare(QString("INSERT INTO %1 (part, shelf) VALUES (%2, %3)")
            .arg(m_settings.dbTable)
            .arg(part)
            .arg(shelf)
        );

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

void QData::settingsChanged() {
    Window* win = qobject_cast<Window*>(parent());
    m_settings = SettingsDialog::DataSettings(Settings::get(), settingsPath());

    if (m_db.isValid() || m_db.isOpen()) {
        m_db.close();
    }

    m_db = QSqlDatabase::addDatabase(m_settings.dbDriver);
    m_db.setDatabaseName(m_settings.dbUri);

    bool rc = m_db.open();

    emit message(QString("QData::QData: connection to database: %1").arg(rc ? tr("success") : tr("failed")));
    
    if (rc == false) {
        return;
    }

    if (m_db.tables().contains(m_settings.dbTable, Qt::CaseInsensitive) == false) {
        m_db.exec(QString(createTable).arg(m_settings.dbTable));
    } else {
        emit message("QData::QData: table already exists");
    }

    if (m_model != nullptr) {
        m_ui->dbView->setModel(nullptr);
        delete m_model;
    }

    m_model = new QSqlTableModel(nullptr, m_db);
    m_ui->dbView->setModel(m_model);
    m_model->setTable(m_settings.dbTable);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_model->setHeaderData(1, Qt::Horizontal, tr("Part"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Cell"));
    m_ui->dbView->setModel(m_model);
    m_ui->dbView->setColumnHidden(0, true);
}

void QData::prepareForFocus() {
    emit message("QData::QData::prepareForFocus()");
    m_ui->barcodeEdit->setFocus();
}

bool QData::clearData() {
    emit message("QData::clearData()", LoggerSeverity::LOG_DEBUG);

    QSqlQuery query(m_db);
    if (query.exec(QString("DELETE FROM %1").arg(m_settings.dbTable)) == true) {
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
        if (m_settings.serialInterval != -1) {
            emit message("QData::activated(): timer started");
            m_timer.start();
        } else {
            emit message("QData::activated(): single shot");
            timeout();
        }
    }
}

void QData::enterPressed() {
    emit message("QData::enterPressed()", LoggerSeverity::LOG_DEBUG);
    QString str = m_ui->barcodeEdit->text();

    if (str.isEmpty()) {
        emit message("QData::enterPressed(): input is empty");
        return;
    }

    emit message("QData::enterPressed() code: " + str);

    if (m_settings.barcodeRegexp.isEmpty() == false) {
        emit message("QData::enterPressed(): barcode regexp found "+ m_settings.barcodeRegexp);
        QRegularExpression regex(m_settings.barcodeRegexp);
        auto match = regex.match(str);

        if (match.hasMatch() && match.capturedLength() > 2) {
            emit message("QData::enterPressed(): regex matched");
            auto res = match.captured(1);
            res.remove('.');
            m_ui->barcodeEdit->setText(res);
            emit message(QString("QData::enterPressed(): %1 -> %2").arg(str).arg(res));
            str = res;
        } else {
            emit message("QData::enterPressed() regex not matched ");
        }
    }

    if (m_settings.removeChars.isEmpty() == false) {
        emit message("QData::enterPressed(): removing characters: "+m_settings.removeChars);
        QString toRemove = m_settings.removeChars;
        str.removeIf([&toRemove](const QChar& ch) {
            return toRemove.contains(ch);
        });
        m_ui->barcodeEdit->setText(str);
    }

    m_model->setFilter("part LIKE '%" + str + "%'");
    m_model->select();

    int row = findByPart(m_model, str);
    emit message(QString("QData::enterPressed(): find by part: %1").arg(row));

    if (row == -1 && m_settings.omitZeros == true) {
        emit message("QData::enterPressed(): trying search by regexp ommiting 0's at begin and end");
        row = findByPartWithOmit(m_model, str);
    }

    if (row != -1) {
        QModelIndex idx = m_model->index(row, 2);
        m_ui->dbView->scrollTo(idx);
        m_ui->dbView->selectRow(row);
        m_selected = idx.data().toString();

        m_lmodel->appendRow(new QStandardItem(m_model->index(row, 1).data().toString()));
        fillInfo();
        
        if (m_settings.serialInterval != -1) {
            emit message("QData::enterPressed(): timer start");
            m_timer.start();
        } else {
            emit message("QData::enterPressed(): single shot");
            timeout();
        }
    }
}

void QData::toggleLock() {
    if (m_ui->unlockButton->text() == tr("Unlock")) {
        if (m_settings.dbLock == true && m_settings.dbLockPass.isEmpty() == false) {
            PasswordDialog d(this, m_settings.dbLockPass);
            if (d.exec() != QDialog::Accepted) {
                return;
            }
        }

        m_ui->importCsvButton->setEnabled(true);
        m_ui->importDirButton->setEnabled(true);
        m_ui->exportCSVButton->setEnabled(true);
        m_ui->unlockButton->setText(tr("Lock"));
    } else {
        m_ui->importCsvButton->setEnabled(false);
        m_ui->importDirButton->setEnabled(false);
        m_ui->exportCSVButton->setEnabled(false);
        m_ui->unlockButton->setText(tr("Unlock"));
    }
}