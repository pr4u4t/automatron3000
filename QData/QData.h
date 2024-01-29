#ifndef QDATA_H
#define QDATA_H

#include <QTableView>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>

#include "qdata_global.h"
#include "../ModuleLoader.h"

class QDATA_EXPORT QData : public Widget{
    
    Q_OBJECT

public:
    QData(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    bool saveSettings() {
        return true;
    }

public slots:
    void settingsChanged();

protected slots:

    void exportAsCsv(bool checked = false);

    void importFromCsv(bool checked = false);

    void importFromFiles(bool checked = false);

    void prepareForFocus();

    void enterPressed();

private:

    bool clearData();

    QString escapedCSV(QString unexc);

    void queryToCsv(const QString& path, const QString& queryStr);

    static constexpr const char* database = "./szpuler.dat";
    static constexpr const char* table = "part_shelf";
    static constexpr const char* createTable = "CREATE TABLE %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, part VARCHAR(255), shelf VARCHAR(255))";
    static constexpr const char* exportQuery = "SELECT * FROM %1";
    QSqlDatabase m_db;
    QSqlTableModel* m_model = nullptr;
    QLineEdit* m_edit = nullptr;


    static constexpr const char* title = "DataWindow";
};

#endif