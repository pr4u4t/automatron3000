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
#include <QRegExp>

#include "qdata_global.h"
#include "../api/api.h"
#include "SettingsDialog.h"
#include "ui_qdataui.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QDataUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class QDATA_EXPORT QData : public Widget{
    
    Q_OBJECT

public:
    QData(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    bool saveSettings() {
        return true;
    }

protected:

    int findByPart(QSqlTableModel* model, const QString& part);

    int findByPartWithOmit(QSqlTableModel* model, const QString& part);

public slots:
    void settingsChanged();

protected slots:

    void toggleLock();

    void exportAsCsv(bool checked = false);

    void importFromCsv(bool checked = false);

    void importFromFiles(bool checked = false);

    void prepareForFocus();

    void enterPressed();

    void activated(const QModelIndex& idx);

    void timeout();

    void clearForm();

    void textChanged(const QString& text);

    void fillInfo();

private:

    bool clearData();

    void send();
    
    QString escapedCSV(QString unexc);

    void queryToCsv(const QString& path, const QString& queryStr);

    static constexpr const char* createTable = "CREATE TABLE %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, part VARCHAR(255), shelf VARCHAR(255))";
    static constexpr const char* exportQuery = "SELECT * FROM %1";
    static constexpr const char* title = "QData";

    QSqlDatabase m_db;
    QSqlTableModel* m_model = nullptr;
    QString m_selected;
    QTimer m_timer;
    QStandardItemModel* m_lmodel = nullptr;
    SettingsDialog::DataSettings m_settings;
    Ui::QDataUI* m_ui = nullptr;
};

#endif