#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include <QTableView>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>

#include "mdichild.h"

class DataWindow : public MdiChild{ 

    Q_OBJECT
    
public:
    
    Q_INVOKABLE DataWindow(QWidget* parent = nullptr, QWidget* mwin = nullptr);

public slots:
    void settingsChanged();
    
private:
    static constexpr const char* database = "./szpuler.dat";
    static constexpr const char* table = "part_shelf";
    QSqlDatabase m_db;
};

#endif
