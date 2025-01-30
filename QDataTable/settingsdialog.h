#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

#include "../api/api.h"

class QDataTable;

class ComboBoxItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxItemDelegate(QObject* parent = nullptr);
    ~ComboBoxItemDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

struct DataTableSettings : public PluginSettings {
    Q_GADGET
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(DataTableMode mode READ mode WRITE setMode)
    Q_PROPERTY(QJsonArray columns READ columns WRITE setColumns)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)
    Q_PROPERTY(QString newLine READ newLine WRITE setNewLine)
    Q_PROPERTY(QString input READ input WRITE setInput)

public:

    enum class DataTableMode {
        BINARY,
        TEXT
    };

    Q_ENUM(DataTableMode)

private:
    static constexpr const char* const titleKey = "title";
    static constexpr const char* const titleValue = nullptr;
    static constexpr const char* const modeKey = "mode";
    static constexpr const DataTableMode modeValue = DataTableMode::BINARY;
    static constexpr const char* const columnsKey = "columns";
    static constexpr const char* const columnsValue = nullptr;
    static constexpr const char* const separatorKey = "separator";
    static constexpr const char* const separatorValue = ",";
    static constexpr const char* const newLineKey = "newLine";
    static constexpr const char* const newLineValue = "\\r\\n";
    static constexpr const char* const inputKey = "input";
    static constexpr const char* const inputValue = nullptr;
    static constexpr const char* const magicKey = "magic";
    static constexpr const char* const magicValue = nullptr;
    static constexpr const char* const maxRowsKey = "maxRows";
    static constexpr const qlonglong maxRowsValue = 1000000;

public:

    DataTableSettings()
        : m_title(titleValue)
        , m_mode(modeValue)
        , m_columns()
        , m_separator(separatorValue)
        , m_newLine(newLineValue)
        , m_input(inputValue)
        , m_magic(magicValue)
        , m_maxRows(maxRowsValue){
        registerMetaObject(&staticMetaObject);
    }

    virtual ~DataTableSettings() = default;

    DataTableSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_title(settings.value(settingsPath + "/" + titleKey, titleValue).toString()) 
        , m_mode(static_cast<DataTableMode>(settings.value(settingsPath + "/" + modeKey, QVariant::fromValue(modeValue)).toInt()))
        , m_columns(settings.value(settingsPath + "/" + columnsKey, columnsValue).toJsonArray())
        , m_separator(settings.value(settingsPath + "/" + separatorKey, separatorValue).toString())
        , m_newLine(settings.value(settingsPath + "/" + newLineKey, newLineValue).toString())
        , m_input(settings.value(settingsPath + "/" + inputKey, inputValue).toString())
        , m_magic(settings.value(settingsPath + "/" + magicKey, magicValue).toByteArray())
        , m_maxRows(settings.value(settingsPath + "/" + maxRowsKey, maxRowsValue).toLongLong()){
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        
        settings.setValue(settingsPath + "/" + titleKey, m_title);
        settings.setValue(settingsPath + "/" + modeKey, QVariant::fromValue(m_mode).toInt());
        settings.setValue(settingsPath + "/" + columnsKey, m_columns);
        settings.setValue(settingsPath + "/" + separatorKey, m_separator);
        settings.setValue(settingsPath + "/" + newLineKey, m_newLine);
        settings.setValue(settingsPath + "/" + inputKey, m_input);
        settings.setValue(settingsPath + "/" + magicKey, m_magic);
        settings.setValue(settingsPath + "/" + maxRowsKey, m_maxRows);

        PluginSettings::save(settings, settingsPath);
    }

    QString title() const { return m_title; }
    void setTitle(const QString& title) { m_title = title; }

    // Getter and Setter for m_mode
    DataTableMode mode() const { return m_mode; }
    void setMode(DataTableMode mode) { m_mode = mode; }

    // Getter and Setter for m_columns
    QJsonArray columns() const { return m_columns; }
    void setColumns(const QJsonArray& columns) { m_columns = columns; }

    // Getter and Setter for m_separator
    QString separator() const { return m_separator; }
    void setSeparator(const QString& separator) { m_separator = separator; }

    // Getter and Setter for m_newLine
    QString newLine() const { return m_newLine; }
    void setNewLine(const QString& newLine) { m_newLine = newLine; }

    QString input() const { return m_input; }
    void setInput(const QString& input) { m_input = input; }

    QByteArray magic() const { return m_magic; }
    void setMagic(const QByteArray& magic) { m_magic = magic; }

    qlonglong maxRows() const { return m_maxRows; }
    void setMaxRows(qlonglong maxRows) { m_maxRows = maxRows; }

    bool operator==(const DataTableSettings& other) const {
        return m_title == other.m_title &&
            m_mode == other.m_mode &&
            m_columns == other.m_columns &&
            m_separator == other.m_separator &&
            m_newLine == other.m_newLine &&
            m_input == other.m_input &&
            m_magic == other.m_magic &&
            m_maxRows == other.m_maxRows;
    }

    bool operator!=(const DataTableSettings& other) const {
        return !(*this == other);
    }

private:

    QString m_title;
    DataTableMode m_mode;
    QJsonArray m_columns;
    QString m_separator;
    QString m_newLine;
    QString m_input;
    QByteArray m_magic;
    qlonglong m_maxRows;
};

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QDataTable* table);

    ~SettingsDialog();

    operator DataTableSettings() const;

private slots:
    void ok();

    void apply();

    void cancel();

    void customMenuRequested(QPoint point);

    void insBeforeColumn();

    void insAfterColumn();

    void addColumn();

    void removeColumn();

private:
    void updateSettings();

    void fillFromSettings();

    void setup();

private:

    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QStandardItemModel* m_model = nullptr;
};

#endif
