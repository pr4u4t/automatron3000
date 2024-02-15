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
        //int col = model->fieldIndex(tr("Part"));
        for (int row = 0; row < model->rowCount(); ++row){
            QVariant data = model->data(model->index(row, 1));
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

    void clearForm() {
        m_edit->clear();
        m_view->clearSelection();
        m_timer.stop();
        m_selected.clear();
        m_rack->setText(QString());
        m_shelf->setText(QString());
        m_side->setText(QString());
        m_model->setFilter(QString());
        m_model->select();
    }

    void textChanged(const QString& text) {
        if (text.isEmpty()) {
            m_model->setFilter(QString());
            m_model->select();
        }
    }

    void fillInfo() {
        if (m_selected.isEmpty()) {
            return;
        }

        qint64 cell = m_selected.toLongLong();

        if (cell >= 1 && cell <= 599 || cell >= 2500 && cell <= 3095) {
            m_rack->setText(tr("First"));
        } else if (cell >= 600 && cell <= 1199 || cell >= 3095 && cell <= 3690) {
            m_rack->setText(tr("Second"));
        } else if (cell >= 1200 && cell <= 2499 || cell >= 3690 && cell <= 5000) {
            m_rack->setText(tr("Third"));
        } 
        
        if (cell >= 1 && cell <= 119 || cell >= 1800 && cell <= 1919 || cell >= 2768 && cell <= 2859 || cell >= 600 && cell <= 719 || cell >= 1200 && cell <= 1319 || cell >= 2860 && cell <= 2951) {
            m_shelf->setText(tr("First","shelf"));
        } else if (cell >= 120 && cell <= 239 || cell >= 1920 && cell <= 2039 || cell >= 2676 && cell <= 2767 || cell >= 720 && cell <= 839 || cell >= 1320 && cell <= 1439 || cell >= 2952 && cell <= 3043) {
            m_shelf->setText(tr("Second", "shelf"));
        } else if (cell >= 240 && cell <= 359 || cell >= 2040 && cell <= 2159 || cell >= 2584 && cell <= 2675 || cell >= 840 && cell <= 959 || cell >= 1440 && cell <= 1559 || cell >= 3044 && cell <= 3135) {
            m_shelf->setText(tr("Third", "shelf"));
        } else if (cell >= 360 && cell <= 479 || cell >= 2160 && cell <= 2279 || cell >= 2492 && cell <= 2583 || cell >= 960 && cell <= 1079 || cell >= 1560 && cell <= 1679 || cell >= 3136 && cell <= 3227) {
            m_shelf->setText(tr("Fourth", "shelf"));
        } if (cell >= 480 && cell <= 599 || cell >= 2280 && cell <= 2399 || cell >= 2400 && cell <= 2491 || cell >= 1080 && cell <= 1199 || cell >= 1680 && cell <= 1799 || cell >= 3228 && cell <= 3319) {
            m_shelf->setText(tr("Fifth", "shelf"));
        }

        if (cell >= 1 && cell <= 2499){
            m_side->setText(tr("Left"));
        } else if (cell >= 2500 && cell <= 5000){
            m_side->setText(tr("Right"));
        }
    
    }

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
    QLabel* m_rack = nullptr;
    QLabel* m_shelf = nullptr;
    QLabel* m_side = nullptr;
};

#endif