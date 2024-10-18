#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>
#include <QJsonArray>
#include <QStandardItemModel>
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

    static constexpr const char* const pluginsActionsKey = "pluginsActions";
    static constexpr const char* const pluginsActionsValue = nullptr;
    
    static constexpr const char* const buttonTextKey = "buttonText";
    static constexpr const char* const buttonTextValue = "Exec";

    static constexpr const char* const titleKey = "title";
    static constexpr const char* const titleValue = "Custom Action";

    static constexpr const char* const progressKey = "progress";
    static constexpr const bool progressValue = false;

    static constexpr const char* const verboseKey = "verbose";
    static constexpr const bool verboseValue = false;

    static constexpr const char* const intervalKey = "interval";
    static constexpr const int intervalValue = 0;

    struct CustomActionSettings : public PluginSettings {
        CustomActionSettings()
            : pluginsActions()
            , buttonText(buttonTextValue)
            , title(titleValue)
            , progress(progressValue)
            , verbose(verboseValue)
            , interval(intervalValue){
        }

        virtual ~CustomActionSettings() = default;

        CustomActionSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , pluginsActions(settings.value(settingsPath + "/" + pluginsActionsKey, pluginsActionsValue).toJsonArray())
            , buttonText(settings.value(settingsPath + "/" + buttonTextKey, buttonTextValue).toString())
            , title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
            , progress(settings.value(settingsPath + "/" + progressKey, progressValue).toBool()) 
            , verbose(settings.value(settingsPath + "/" + verboseKey, verboseValue).toBool()) 
            , interval(settings.value(settingsPath + "/" + intervalKey, intervalValue).toInt()){
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + pluginsActionsKey, pluginsActions);
            settings.setValue(settingsPath + "/" + buttonTextKey, buttonText);
            settings.setValue(settingsPath + "/" + titleKey, title);
            settings.setValue(settingsPath + "/" + progressKey, progress);
            settings.setValue(settingsPath + "/" + verboseKey, verbose);
            settings.setValue(settingsPath + "/" + intervalKey, interval);

            PluginSettings::save(settings, settingsPath);
        }

        QStringList jobList() const {
            QStringList ret;

            for (QJsonArray::const_iterator iter = pluginsActions.begin(), end = pluginsActions.end(); iter != end; ++iter) {
                const QJsonObject o = iter->toObject();
                if (o["enabled"].toBool() == false) {
                    continue;
                }

                QString tmp = o["objectName"].toString();
                if (tmp.isEmpty() == false) {
                    ret << tmp;
                }

                tmp = o["function"].toString();
                if (tmp.isEmpty() == false) {
                    ret << tmp;
                }

                const bool r = o["return"].toBool();
                ret << (r ? "true" : "false");
            }

            return ret;
        }

        QJsonArray pluginsActions;
        QString buttonText;
        QString title;
        bool progress;
        bool verbose;
        int interval;
    };

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    CustomActionSettings dataSettings() const;

    QString settingsPath() const;

private slots:
    void ok();

    void apply();

    void cancel();

    void customMenuRequested(QPoint point);

    void insBeforeArgument();

    void insAfterArgument();

    void addArgument();

    void removeArgument();

protected:

    template<typename T>
    T actions(const QStandardItemModel* model) const {
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
            QList<QStandardItem*> items = QList<QStandardItem*>()
                << new QStandardItem(iter->toObject()["objectName"].toString())
                << new QStandardItem(iter->toObject()["function"].toString());

            QStandardItem* check = new QStandardItem();
            check->setCheckable(true);
            check->setCheckState(iter->toObject()["enabled"].toBool() ? Qt::Checked : Qt::Unchecked);
            items << check;

            QStandardItem* ret = new QStandardItem();
            check->setCheckable(true);
            check->setCheckState(iter->toObject()["return"].toBool() ? Qt::Checked : Qt::Unchecked);
            items << ret;

            m_model->appendRow(items);
        }
    }

    template<>
    QStringList actions(const QStandardItemModel* model) const {
        QStringList ret;

        for (QModelIndex idx = model->index(0, 0); idx.isValid(); idx = (idx.column() == 0) ? idx.siblingAtColumn(1) : idx.sibling(idx.row() + 1, 0)) {
            QModelIndex process = idx.siblingAtColumn(2);
            if (process.data(Qt::CheckStateRole).toBool() == false) {
                continue;
            }

            QString tmp = idx.data().toString();
            if (tmp.isEmpty()) {
                continue;
            }
            ret << tmp;
        }

        return ret;
    }

    template<>
    QJsonArray actions(const QStandardItemModel* model) const {
        QJsonArray ret;

        for (QModelIndex idx = model->index(0, 0); idx.isValid(); idx = idx.sibling(idx.row() + 1, 0)) {
            QJsonObject o;
            o["objectName"] = idx.data().toString();

            idx = idx.siblingAtColumn(1);
            o["function"] = idx.data().toString();

            idx = idx.siblingAtColumn(2);
            o["enabled"] = idx.data(Qt::CheckStateRole).toBool();

            ret.push_back(o);
        }

        return ret;
    }

private:
    void updateSettings();

    void fillFromSettings();

    bool verifySettings() const;

private:
    CustomActionSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    //QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
    QStandardItemModel* m_model = nullptr;
};

#endif
