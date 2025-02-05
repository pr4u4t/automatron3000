#include <QApplication>
#include <QTranslator>
#include <QSplitter>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QImage>
#include <QPixmap>

#include "QData.h"
#include "../api/api.h"
#include "../core/core.h"
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
    }

    QMenu* m_dataMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;
    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;

};

static bool QData_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, QDataMenu* ctx, Logger* log) {
    log->message("QData_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("QData_register(): application is non gui not registering");
        return false;
    }

    if (gtx->m_win == nullptr) {
        log->message("QData_register(): window pointer == nullptr");
        return false;
    }

    auto dataMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "Data"));

    if (dataMenu == nullptr) {
        log->message("QData_register(): data menu == nullptr");
        return false;
    }

    if ((ctx->m_dataMenu = dataMenu->addMenu(ctx->m_app->translate("MainWindow", "Inventory"))) == nullptr) {
        log->message("QData_register(): failed to create Inventory menu");
        return false;
    }

    windowAddInstanceSettings(ctx->m_dataMenu, &ctx->m_settings, &ctx->m_newInstance, "Inventory", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<QData, QDataMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("QData"), gtx, ctx, log);
    });

    return true;
}

static bool QData_unregister(ModuleLoaderContext* win, PluginsLoader* ld, QDataMenu* ctx, Logger* log) {
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
    {"QSerial"},
    true,
    300,
    DataSettings
)

QData::QData(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& path, DataSettings* set, const QString& uuid)
    : Widget(
        ld, 
        plugins, 
        parent, 
        path, 
        set,
        uuid
    )
    , m_ui(new Ui::QDataUI) {
    m_ui->setupUi(this);
    //m_db = QSqlDatabase::database();

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

    connect(&m_timer, &QTimer::timeout, this, &QData::timeout);

    m_ui->left->setPixmap(QPixmap(":qdata/left-arrow.png"));
    m_ui->left->setEnabled(false);

    m_ui->right->setPixmap(QPixmap(":qdata/right-arrow.png"));
    m_ui->right->setEnabled(false);
}

bool QData::initialize() {
    
    //Window* win = qobject_cast<Window*>(parent());
    //*(settings<DataSettings>()) = DataSettings(Settings::get(), settingsPath());
    const auto set = settings<DataSettings>();
    //*set = *(Settings::fetch<DataSettings>(settingsPath()));

    if (m_db.isValid() || m_db.isOpen()) {
        m_db.close();
    }

    m_db = QSqlDatabase::addDatabase(set->dbDriver());
    m_db.setDatabaseName(set->dbUri());

    bool rc = m_db.open();

    emit message(QString("QData::QData: connection to database: %1").arg(rc ? tr("success") : tr("failed")));

    if (rc == false) {
        return false;
    }

    if (m_db.tables().contains(set->dbTable(), Qt::CaseInsensitive) == false) {
        m_db.exec(QString(createTable).arg(set->dbTable()));
    } else {
        emit message("QData::QData: table already exists");
    }

    if (m_model != nullptr) {
        m_ui->dbView->setModel(nullptr);
        delete m_model;
    }

    m_model = new QSqlTableModel(nullptr, m_db);
    m_ui->dbView->setModel(m_model);
    m_model->setTable(set->dbTable());
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_model->setHeaderData(1, Qt::Horizontal, tr("Part"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Cell"));
    m_ui->dbView->setModel(m_model);
    m_ui->dbView->setColumnHidden(0, true);

    m_timer.stop();

    if (set->serialInterval() != -1) {
        m_timer.setInterval(set->serialInterval());
    }

    if (set->keepClear() == true && set->clearCode() != -1) {
        m_selected = QString::number(set->clearCode());
    }

    if (set->keepClear() == true
        && set->serialInterval() != -1
        && set->clearCode() != -1) {
        m_timer.start();
    }

    //QTimer::singleShot(0, this, &QData::timeout);
    auto serial = plugins()->instance("QSerial", nullptr);
    auto io = serial.dynamicCast<IODevice>();

    if (io->isOpen() == false) {
        emit message("QData::timeout: serial port not open");
        if (io->open() == false) {
            emit message("QData::timeout: failed to open serial port");
            return false;
        }

        emit message("QData::timeout: serial port open success");
    }

    return true;
}

bool QData::deinitialize() {
    return true;
}

SettingsMdi* QData::settingsWindow() const {
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &QData::settingsChanged);
    return ret;
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
        
    if (settings<DataSettings>()->serialPrefix().isEmpty() == false) {
        auto res = io->write(settings<DataSettings>()->serialPrefix() + '\n');
        emit message(QString("QData::timeout: prefix write %1 %2").arg(res).arg(settings<DataSettings>()->serialPrefix()));
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

    queryToCsv(fileName, QString(exportQuery).arg(settings<DataSettings>()->dbTable()));

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

    if (!input.open(QIODevice::ReadOnly)) {
        emit message(QString("QData::importFromCsv: inpu open failed %1").arg(input.errorString()));
        return;
    }

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
            .arg(settings<DataSettings>()->dbTable())
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
            .arg(settings<DataSettings>()->dbTable())
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
    emit message("QData::settingsChanged()");

    const auto set = settings<DataSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<DataSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

void QData::prepareForFocus() {
    emit message("QData::QData::prepareForFocus()");
    m_ui->barcodeEdit->setFocus();
}

bool QData::clearData() {
    emit message("QData::clearData()", LoggerSeverity::LOG_DEBUG);

    QSqlQuery query(m_db);
    if (query.exec(QString("DELETE FROM %1").arg(settings<DataSettings>()->dbTable())) == true) {
        emit message(QString("QData::clearData(): rows affected %1").arg(query.numRowsAffected()));
        return true;
    }
    
    emit message("QData::clearData(): query failed");
    return false;
}

void QData::send() {
    emit message("QData::send()");

    if (settings<DataSettings>()->clearCode() != -1) {
        emit message(QString("QData::send(): sending clear code: %1").arg(QString::number(settings<DataSettings>()->clearCode())));
        auto serial = plugins()->instance("QSerial", nullptr);
        auto io = serial.dynamicCast<IODevice>();
        auto res = io->write(QString::number(settings<DataSettings>()->clearCode()) + '\n');
    }

    if (settings<DataSettings>()->serialInterval() != -1) {
        if (settings<DataSettings>()->keepClear() == false) {
            emit message("QData::send(): timer start");
            m_timer.start();
        }
    }

    emit message("QData::send(): timeout shot");
    timeout();
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
        
        send();
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

    if (settings<DataSettings>()->barcodeRegexp().isEmpty() == false) {
        emit message("QData::enterPressed(): barcode regexp found "+ settings<DataSettings>()->barcodeRegexp());
        QRegularExpression regex(settings<DataSettings>()->barcodeRegexp());
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

    if (settings<DataSettings>()->removeChars().isEmpty() == false) {
        emit message("QData::enterPressed(): removing characters: "+settings<DataSettings>()->removeChars());
        QString toRemove = settings<DataSettings>()->removeChars();
        str.removeIf([&toRemove](const QChar& ch) {
            return toRemove.contains(ch);
        });
        m_ui->barcodeEdit->setText(str);
    }

    str = str.remove(QRegularExpression("^0+")).remove(QRegularExpression("0+$"));

    m_model->setFilter("part LIKE '%" + str + "%'");
    m_model->select();

    int row = findByPart(m_model, str);
    emit message(QString("QData::enterPressed(): find by part: %1").arg(row));

    if (row == -1 && settings<DataSettings>()->omitZeros() == true) {
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
        
        send();
    }
}

void QData::toggleLock() {
    if (m_ui->unlockButton->text() == tr("Unlock")) {
        if (settings<DataSettings>()->dbLock() == true
            && settings<DataSettings>()->dbLockPass().isEmpty() == false
           ) {
            PasswordDialog d(this, settings<DataSettings>()->dbLockPass());
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

int QData::findByPart(QSqlTableModel* model, const QString& part) {
    //int col = model->fieldIndex(tr("Part"));
    for (int row = 0; row < model->rowCount(); ++row) {
        QVariant data = model->data(model->index(row, 1));
        if (data.toString() == part) {
            return row;
        }
    }

    return -1;
}

int QData::findByPartWithOmit(QSqlTableModel* model, const QString& part) {
    QRegExp rx("0*" + part + "0*");
    for (int row = 0; row < model->rowCount(); ++row) {
        QVariant data = model->data(model->index(row, 1));
        if (rx.exactMatch(data.toString()) == true) {
            return row;
        }
    }

    return -1;
}

void QData::clearForm() {
    emit message(QString("QData::clearForm()"));
    m_ui->barcodeEdit->clear();
    m_ui->dbView->clearSelection();
    
    if (settings<DataSettings>()->keepClear() == false) {
        emit message(QString("QData::clearForm(): clearing selected and stopping timer"));
        m_selected.clear();
        m_timer.stop();
    } else {
        emit message(QString("QData::clearForm(): setting selected to clear code"));
        m_selected = QString::number(settings<DataSettings>()->clearCode());
    }
    
    if (settings<DataSettings>()->clearCode() != -1) {
        emit message(QString("QData::clearForm(): sending clear code: %1").arg(QString::number(settings<DataSettings>()->clearCode())));
        auto serial = plugins()->instance("QSerial", nullptr);
        auto io = serial.dynamicCast<IODevice>();
        auto res = io->write(QString::number(settings<DataSettings>()->clearCode()) + '\n');
    }

    m_model->setFilter(QString());
    m_model->select();

    m_ui->rackLabel->setText(QString());
    m_ui->sideLabel->setText(QString());
    m_ui->shelfLabel->setText(QString());

    m_ui->right->setEnabled(false);
    m_ui->left->setEnabled(false);
}

void QData::textChanged(const QString& text) {
    if (text.isEmpty()) {
        m_model->setFilter(QString());
        m_model->select();
    }
}

void QData::fillInfo() {
    if (m_selected.isEmpty()) {
        return;
    }

    qint64 cell = m_selected.toLongLong();

    if (cell >= 1 && cell <= 599 || cell >= 2500 && cell <= 3095) {
        m_ui->rackLabel->setText(tr("First"));
    }
    else if (cell >= 600 && cell <= 1199 || cell >= 3095 && cell <= 3690) {
        m_ui->rackLabel->setText(tr("Second"));
    }
    else if (cell >= 1200 && cell <= 2499 || cell >= 3690 && cell <= 5000) {
        m_ui->rackLabel->setText(tr("Third"));
    }

    if (cell >= 1 && cell <= 119 || cell >= 1800 && cell <= 1919 || cell >= 2768 && cell <= 2859 || cell >= 600 && cell <= 719 || cell >= 1200 && cell <= 1319 || cell >= 2860 && cell <= 2951) {
        m_ui->shelfLabel->setText(tr("First", "shelf"));
    }
    else if (cell >= 120 && cell <= 239 || cell >= 1920 && cell <= 2039 || cell >= 2676 && cell <= 2767 || cell >= 720 && cell <= 839 || cell >= 1320 && cell <= 1439 || cell >= 2952 && cell <= 3043) {
        m_ui->shelfLabel->setText(tr("Second", "shelf"));
    }
    else if (cell >= 240 && cell <= 359 || cell >= 2040 && cell <= 2159 || cell >= 2584 && cell <= 2675 || cell >= 840 && cell <= 959 || cell >= 1440 && cell <= 1559 || cell >= 3044 && cell <= 3135) {
        m_ui->shelfLabel->setText(tr("Third", "shelf"));
    }
    else if (cell >= 360 && cell <= 479 || cell >= 2160 && cell <= 2279 || cell >= 2492 && cell <= 2583 || cell >= 960 && cell <= 1079 || cell >= 1560 && cell <= 1679 || cell >= 3136 && cell <= 3227) {
        m_ui->shelfLabel->setText(tr("Fourth", "shelf"));
    } if (cell >= 480 && cell <= 599 || cell >= 2280 && cell <= 2399 || cell >= 2400 && cell <= 2491 || cell >= 1080 && cell <= 1199 || cell >= 1680 && cell <= 1799 || cell >= 3228 && cell <= 3319) {
        m_ui->shelfLabel->setText(tr("Fifth", "shelf"));
    }

    if (cell >= 1 && cell <= 2499) {
        m_ui->sideLabel->setText(tr("Left"));
        m_ui->left->setEnabled(true);
        m_ui->right->setEnabled(false);
    }
    else if (cell >= 2500 && cell <= 5000) {
        m_ui->sideLabel->setText(tr("Right"));
        m_ui->right->setEnabled(true);
        m_ui->left->setEnabled(false);
    }

}