#ifndef QDATA_TABLE_H
#define QDATA_TABLE_H

#include "qdatatable_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "../api/plugin.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QDataTableUI;
}

//class QIntValidator;
QT_END_NAMESPACE

enum TypeSignedness {
    NONE,
    SIGNED,
    UNSIGNED
};

enum VariableType {
    INTEGER,
    FLOAT,
    DOUBLE,
    UNKNOWN
};

struct ColumnData {

    ColumnData(const QString& name, const QString& sign, const QString& type, const QString& length) 
    : m_name(name)
    , m_signedness(signednessFromString(sign))
    , m_length(lengthFromString(length))
    , m_type(variableTypeFromString(type)){
        switch (m_type) {
        case VariableType::FLOAT:
            m_length = sizeof(float);
            break;
        case VariableType::DOUBLE:
            m_length = sizeof(double);
            break;
        }
    }

    operator QString() const {
        return QString("name: %1, signedness: %2, type: %3, length: %4").arg(m_name).arg(signednessToString(m_signedness)).arg(typeToString(m_type)).arg(m_length);
    }

    QString m_name;
    TypeSignedness m_signedness;
    VariableType m_type;
    int8_t m_length;

private:

    QString signednessToString(const TypeSignedness& sign) const {
        switch (sign) {
        case TypeSignedness::SIGNED:
            return "signed";
        case TypeSignedness::UNSIGNED:
            return "unsigned";
        default:
            return "none";
        }
    }

    QString typeToString(const VariableType& type) const {
        switch (type) {
        case VariableType::DOUBLE:
            return "double";
        case VariableType::FLOAT:
            return "float";
        case VariableType::INTEGER:
            return "int";
       default:
            return "unknown";
        }
    }

    TypeSignedness signednessFromString(const QString& sign) const {
        if (sign == "signed") {
            return TypeSignedness::SIGNED;
        }

        if (sign == "unsigned") {
            return TypeSignedness::UNSIGNED;
        }

        return TypeSignedness::NONE;
    }

    VariableType variableTypeFromString(const QString& type) const {
        if (type == "int") {
            return VariableType::INTEGER;
        }

        if (type == "float") {
            return VariableType::FLOAT;
        }

        if (type == "double") {
            return VariableType::DOUBLE;
        }

        return VariableType::UNKNOWN;
    }

    int8_t lengthFromString(const QString& length) const {
        return length.toInt()/8;
    }


};
typedef QList<ColumnData> QListColumns;

struct QDataTablePriv {
    QDataTablePriv(Ui::QDataTableUI* ui, QStandardItemModel* model = nullptr)
    : m_ui(ui)
    , m_model(model){}

    QStandardItemModel* m_model = nullptr;
    Ui::QDataTableUI* m_ui = nullptr;
    QListColumns m_columns;
    QByteArray m_buffer;
};

class QDATATABLE_EXPORT QDataTable : public Widget {

    Q_OBJECT

public:
    QDataTable(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, DataTableSettings* set, const QString& uuid);

    bool saveSettings() {
        return true;
    }

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT);

    bool initialize() override;

    bool deinitialize() override;

public slots:
    void settingsChanged();

    QVariant exec() override {
        return QVariant();
    }

    void dataReady(const QByteArray& data);

private:

    void handleBinaryData(const QByteArray& data, const DataTableSettings* set);

    void handleAsciiData(const QByteArray& data, const DataTableSettings* set);

    int columnsLength(const QListColumns& columns) const;

    QRegularExpression columnsToRegExp(const QListColumns& columns, const DataTableSettings* set) const;

    QDataTablePriv m_data;

};

#endif