#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>
#include <QStandarditemModel>
#include <QJsonArray>
#include <QJsonObject>

#include "../api/api.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    static constexpr const char* const titleKey = "title";
    static constexpr const char* const titleValue = "JTAG";
    static constexpr const char* const buttonLabelKey = "buttonLabel";
    static constexpr const char* const buttonLabelValue = "Execute";
    static constexpr const char* const programPathKey = "path";
    static constexpr const char* const programPathValue = nullptr;
    static constexpr const char* const argumentsKey = "arguments";
    static constexpr const char* const argumentsValue = nullptr;

    struct QJTAGSettings : public PluginSettings {
        QJTAGSettings() 
            : title(titleValue)
            , buttonLabel(buttonLabelValue)
            , programPath(programPath)
            , arguments(){
        }

        QJTAGSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
            , buttonLabel(settings.value(settingsPath + "/" + buttonLabelKey, buttonLabelValue).toString())
            , programPath(settings.value(settingsPath + "/" + programPathKey, programPathValue).toString())
            , arguments(settings.value(settingsPath + "/" + argumentsKey, argumentsValue).toJsonArray()) {
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + titleKey, title);
            settings.setValue(settingsPath + "/" + buttonLabelKey, buttonLabel);
            settings.setValue(settingsPath + "/" + programPathKey, programPath);
            settings.setValue(settingsPath + "/" + argumentsKey, arguments);
            
            PluginSettings::save(settings, settingsPath);
        }

        QStringList processArguments() const {
            QStringList ret;
            
            for (QJsonArray::const_iterator iter = arguments.begin(), end = arguments.end(); iter != end; ++iter) {
                QString tmp = iter->toObject()["argument"].toString();
                if (tmp.isEmpty() == false) {
                    ret << tmp;
                }

                tmp = iter->toObject()["value"].toString();
                if (tmp.isEmpty() == false) {
                    ret << tmp;
                }
            }

            return ret;
        }

        QString title;
        QString buttonLabel;
        QString programPath;
        QJsonArray arguments;
    };

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    QJTAGSettings dataSettings() const;

    QString settingsPath() const;

private slots:

    void ok();

    void apply();
    
    void cancel();

    void chooseFile();

    void customMenuRequested(QPoint point);

    void addArgument();

    void insBeforeArgument();

    void insAfterArgument();

    void removeArgument();

private:

    void updateSettings();
    
    void fillFromSettings();

    bool verifySettings() const;

    template<typename T>
    T arguments(const QStandardItemModel* model) const {
        T ret;
        static_assert(
            (std::is_same_v<T, QStringList> || std::is_same_v<T, QJsonArray>),
            "T must be either QStringList or QJsonArray"
            );
        return T;
    }

    void fillModel(const QJsonArray& array) {
        m_model->removeRows(0, m_model->rowCount());

        for (QJsonArray::const_iterator iter = array.begin(), end = array.end(); iter != end; ++iter) {
            QList<QStandardItem*> items = QList<QStandardItem*>() << new QStandardItem(iter->toObject()["argument"].toString()) << new QStandardItem(iter->toObject()["value"].toString());
            m_model->appendRow(items);
        }
    }

    template<>
    QStringList arguments(const QStandardItemModel* model) const {
        QStringList ret;
        
        for (QModelIndex idx = model->index(0, 0); idx.isValid(); idx = (idx.column() == 0) ? idx.siblingAtColumn(1) : idx.sibling(idx.row() + 1, 0)) {
            QString tmp = idx.data().toString();
            if (tmp.isEmpty()) {
                continue;
            }
            ret << tmp;
        }

        return ret;
    }

    template<>
    QJsonArray arguments(const QStandardItemModel* model) const {
        QJsonArray ret;
        QJsonObject o;

        for (QModelIndex idx = model->index(0, 0); idx.isValid(); idx = (idx.column() == 0) ? idx.siblingAtColumn(1) : idx.sibling(idx.row() + 1, 0)) {
            QString tmp = idx.data().toString();
            switch (idx.column()) {
            case 0:
                o["argument"] = tmp;
                break;

            case 1:
                o["value"] = tmp;
                ret.push_back(o);
                o = QJsonObject();
                break;
            }
        }

        return ret;
    }

private:
    QJTAGSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
    QStandardItemModel* m_model = nullptr;
};

#endif