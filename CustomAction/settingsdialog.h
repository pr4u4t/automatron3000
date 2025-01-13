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

struct CustomActionSettings : public PluginSettings {
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

    Q_GADGET

    Q_PROPERTY(QJsonArray pluginsActions READ pluginsActions WRITE setPluginsActions NOTIFY pluginsActionsChanged)
    Q_PROPERTY(QString buttonText READ buttonText WRITE setButtonText NOTIFY buttonTextChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(bool verbose READ verbose WRITE setVerbose NOTIFY verboseChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

public:

    CustomActionSettings()
        : m_pluginsActions()
        , m_buttonText(buttonTextValue)
        , m_title(titleValue)
        , m_progress(progressValue)
        , m_verbose(verboseValue)
        , m_interval(intervalValue) {
        registerMetaObject(&staticMetaObject);
    }

    virtual ~CustomActionSettings() = default;

    CustomActionSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_pluginsActions(settings.value(settingsPath + "/" + pluginsActionsKey, pluginsActionsValue).toJsonArray())
        , m_buttonText(settings.value(settingsPath + "/" + buttonTextKey, buttonTextValue).toString())
        , m_title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
        , m_progress(settings.value(settingsPath + "/" + progressKey, progressValue).toBool())
        , m_verbose(settings.value(settingsPath + "/" + verboseKey, verboseValue).toBool())
        , m_interval(settings.value(settingsPath + "/" + intervalKey, intervalValue).toInt()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + pluginsActionsKey, m_pluginsActions);
        settings.setValue(settingsPath + "/" + buttonTextKey, m_buttonText);
        settings.setValue(settingsPath + "/" + titleKey, m_title);
        settings.setValue(settingsPath + "/" + progressKey, m_progress);
        settings.setValue(settingsPath + "/" + verboseKey, m_verbose);
        settings.setValue(settingsPath + "/" + intervalKey, m_interval);

        PluginSettings::save(settings, settingsPath);
    }

    QStringList jobList() const {
        QStringList ret;

        for (QJsonArray::const_iterator iter = m_pluginsActions.begin(), end = m_pluginsActions.end(); iter != end; ++iter) {
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

    QJsonArray pluginsActions() const {
        return m_pluginsActions;
    }

    void setPluginsActions(const QJsonArray& pluginsActions) {
        m_pluginsActions = pluginsActions;
    }

    QString buttonText() const {
        return m_buttonText;
    }

    void setButtonText(const QString& buttonText) {
        m_buttonText = buttonText;
    }

    QString title() const {
        return m_title;
    }

    void setTitle(const QString& title) {
        m_title = title;
    }

    bool progress() const {
        return m_progress;
    }

    void setProgress(bool progress) {
        m_progress = progress;
    }

    bool verbose() const {
        return m_verbose;
    }

    void setVerbose(bool verbose) {
        m_verbose = verbose;
    }

    int interval() const {
        return m_interval;
    }

    void setInterval(int interval) {
        m_interval = interval;
    }

    bool operator==(const CustomActionSettings& other) const {
        return m_pluginsActions == other.m_pluginsActions &&
            m_buttonText == other.m_buttonText &&
            m_title == other.m_title &&
            m_progress == other.m_progress &&
            m_verbose == other.m_verbose &&
            m_interval == other.m_interval;
    }

    bool operator!=(const CustomActionSettings& other) const {
        return !(*this == other);
    }

private:
    QJsonArray m_pluginsActions;
    QString m_buttonText;
    QString m_title;
    bool m_progress;
    bool m_verbose;
    int m_interval;
};

class QCustomAction;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QCustomAction* action);

    ~SettingsDialog();

    operator CustomActionSettings() const;

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

    void setup();

private:
    Ui::SettingsDialog* m_ui = nullptr;
    //QIntValidator* m_intValidator = nullptr;
    QStandardItemModel* m_model = nullptr;
};

#endif
