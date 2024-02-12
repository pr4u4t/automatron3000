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
#include <QTimer>
#include <QListView>
#include <QStandardItemModel>
#include <QDirIterator>

#include "qdata_global.h"
#include "../api/api.h"

class QDATA_EXPORT QData : public Widget{
    
    Q_OBJECT

public:
    QData(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    bool saveSettings() {
        return true;
    }

protected:

    int findByPart(QSqlTableModel* model, const QString& part){
        int col = model->fieldIndex(tr("part"));
        for (int row = 0; row < model->rowCount(); ++row){
            QVariant data = model->data(model->index(row, col));
            if (data.toString() == part){
                return row;
            }
        }

        return -1;
    }

public slots:
    void settingsChanged();

protected slots:

    void exportAsCsv(bool checked = false);

    void importFromCsv(bool checked = false);

    void importFromFiles(bool checked = false);

    void prepareForFocus();

    void enterPressed();

    void activated(const QModelIndex& idx);

    void timeout();

private:

    bool clearData();

    QString escapedCSV(QString unexc);

    void queryToCsv(const QString& path, const QString& queryStr);

    static constexpr const char* database = "./szpuler.dat";
    static constexpr const char* table = "part_shelf";
    static constexpr const char* createTable = "CREATE TABLE %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, part VARCHAR(255), shelf VARCHAR(255))";
    static constexpr const char* exportQuery = "SELECT * FROM %1";
    static constexpr const char* title = "DataWindow";
    static constexpr int interval = 100;

    QSqlDatabase m_db;
    QSqlTableModel* m_model = nullptr;
    QLineEdit* m_edit = nullptr;
    QTableView* m_view = nullptr;
    QString m_selected;
    QTimer m_timer;
    QListView* m_list = nullptr;
    QStandardItemModel* m_lmodel = nullptr;
};

#endif