#include "datawindow.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDebug>

DataWindow::DataWindow(QWidget* parent, QWidget* mwin)
    : MdiChild(parent){

    QString db = qobject_cast<MainWindow*>(mwin)->settings().value("DataWindow/database", QString(database)).toString();
    QString tbl = qobject_cast<MainWindow*>(mwin)->settings().value("DataWindow/table", QString(table)).toString();
        
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(db);
        
    if (!m_db.open()){
        qDebug() << "Error: connection with database failed";
    }
    
    if(m_db.tables().contains(tbl, Qt::CaseInsensitive) == false){
        m_db.exec(QString("CREATE TABLE %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, part VARCHAR(255), shelf VARCHAR(255))").arg(tbl));
    }
    
    QTableView *view = new QTableView();
    QSqlTableModel* model = new QSqlTableModel(view,m_db);
    model->setTable(tbl);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    view->setModel(model);
    
    QBoxLayout *l = new QVBoxLayout();
    l->addWidget(view);
    setLayout(l);
}


void DataWindow::settingsChanged(){}
