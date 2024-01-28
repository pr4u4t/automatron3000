#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QDirIterator>
#include <QLineEdit>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

#include "../api/api.h"
#include "datawindow.h"

DataWindow::DataWindow(Plugin* parent, QWidget* mwin)
    : MdiChild(parent,mwin){

    m_db = QSqlDatabase::database();
    QSettings settings;
    if(!m_db.isValid() || !m_db.isOpen()){

        QString db = settings.value(parent->settingsPath()+"/DataWindow/database", QString(database)).toString();
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(db);
        
        if (!m_db.open()){
            qDebug() << "Error: connection with database failed";
        }
    }
    
    QString tbl = settings.value(parent->settingsPath() + "/DataWindow/table", QString(table)).toString();
    if(m_db.tables().contains(tbl, Qt::CaseInsensitive) == false){
        m_db.exec(QString(createTable).arg(tbl));
    }
    
    QTableView *view = new QTableView();
    view->setShowGrid(true);
    view->setSortingEnabled(true);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->setAlternatingRowColors(true);
    view->setGridStyle(Qt::SolidLine);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_model = new QSqlTableModel(view,m_db);
    m_model->setTable(tbl);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    view->setModel(m_model);
    view->setColumnHidden(0, true);

    QLabel *label = new QLabel(tr("Part Number"));
    QPushButton *importCSVButton = new QPushButton(tr("Import CSV"));
    QPushButton *importDirButton = new QPushButton(tr("Import Dir"));
    QPushButton *exportButton = new QPushButton(tr("Export CSV"));
    
    connect(importCSVButton,&QPushButton::clicked,this,&DataWindow::importFromCsv);
    connect(importDirButton,&QPushButton::clicked,this,&DataWindow::importFromFiles);
    connect(exportButton,&QPushButton::clicked,this,&DataWindow::exportAsCsv);
    
    m_edit = new QLineEdit();
    m_edit->setMaxLength(24);
    m_edit->setClearButtonEnabled(true);
    connect(m_edit, &QLineEdit::returnPressed, this, &DataWindow::enterPressed);
    
    QBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget(importCSVButton);
    buttons->addWidget(importDirButton);
    buttons->addWidget(exportButton);
    buttons->addStretch();
    buttons->setSpacing(10);
    
    QBoxLayout *l = new QVBoxLayout();
    l->addWidget(view);

    QBoxLayout *h = new QHBoxLayout();
    h->addWidget(label);
    h->addWidget(m_edit);
    h->addStretch(2);
    h->setSpacing(10);
    
    l->addItem(h);
    l->addItem(buttons);
    
    setLayout(l);
}

void DataWindow::exportAsCsv(bool checked){
    Q_UNUSED(checked)
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "./output.csv",
                                                    tr("Text files (*.csv *.txt)"));
    
    if(fileName.isEmpty()){
        return;
    }
    
    queryToCsv(fileName,QString(exportQuery).arg(table));

    QMessageBox::information(this, tr(title),
        tr("Database export successful"),
        QMessageBox::Ok);
}

void DataWindow::importFromCsv(bool checked){
    Q_UNUSED(checked)
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "./",
                                                    tr("Text files (*.csv *.txt)"));
    
    if(fileName.isEmpty()){
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
    
    while (!in.atEnd()){
        QString line = in.readLine();
        const QStringList columns = line.split(",");
        
        QSqlQuery query;
        query.prepare(QString("INSERT INTO %1 (id, part, shelf) VALUES (%2, %3, %4)")
                             .arg(table).arg(columns[0]).arg(columns[1]).arg(columns[2]));
        
        if(!query.exec()){
            qDebug() << "Insert error: " << query.lastError().text();
        }
    }
    
    input.close();
}

void DataWindow::importFromFiles(bool checked){
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
    
    QDirIterator it(dir, {"*.csv", "*.txt"}, QDir::Files | QDir::NoSymLinks | QDir::Readable, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        QFile file(path);
        
        if(!file.open(QIODevice::ReadOnly)){
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
        
        if(!query.exec()){
            qDebug() << "Insert error: " << query.lastError().text();
        }
    }
    
    m_model->select();
}

QString DataWindow::escapedCSV(QString unexc){
    if (!unexc.contains(QLatin1Char(',')))
        return unexc;
    return '\"' + unexc.replace(QLatin1Char('\"'), QStringLiteral("\"\"")) + '\"';
}

void DataWindow::queryToCsv(const QString& path, const QString& queryStr){
    QSqlQuery query(m_db);
    query.prepare(queryStr);
    QFile csvFile(path);
    
    if (!csvFile.open(QFile::WriteOnly | QFile::Text)){
        qDebug("failed to open csv file");
        return;
    }
    
    if (!query.exec()){
        qDebug("failed to run query");
        return;
    }
    
    QTextStream outStream(&csvFile);
    outStream.setEncoding(QStringConverter::Utf8);
    
    while (query.next()){
        const QSqlRecord record = query.record();
        for (int i=0, recCount = record.count() ; i<recCount ; ++i){
            if (i>0){
                outStream << ',';
            }
            
            outStream << escapedCSV(record.value(i).toString());
        }
        outStream << '\n';
    }
}

void DataWindow::settingsChanged(){}

void DataWindow::prepareForFocus() {
    m_edit->setFocus();
}

bool DataWindow::clearData(){
    QSqlQuery query(m_db);
    return query.exec(QString("DELETE FROM %1").arg(table));
}

void DataWindow::enterPressed() {

}