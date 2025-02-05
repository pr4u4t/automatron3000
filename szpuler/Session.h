#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include "../api/api.h"
#include "../core/core.h"
#include "MainWindow.h"



struct SessionModuleInstance {
    QString m_name;
    QString m_path;
    QString m_plugin;

    SessionModuleInstance() {}

    SessionModuleInstance(const QByteArray& data) {
        QRegExp rx("^([A-Za-z0-9]+)");
        if (rx.indexIn(data) != -1) {
            m_plugin = rx.cap(1);
            const QByteArrayList lst = data.split('@');
            m_name = lst[1];
            m_path = lst[0];
        }
    }

    SessionModuleInstance(const QString& plugin, const QString& uuid, const QString& name) 
        : m_plugin(plugin)
        , m_path(uuid)
        , m_name(name){
    }

    SessionModuleInstance(const MLoader::PluginType& instance) 
        : m_plugin(instance->name())
        , m_path(QString("%1-%2").arg(instance->name()).arg(instance->uuid()))
        , m_name(instance.dynamicCast<QObject>()->objectName()) {
    }

    SessionModuleInstance(const Plugin* instance)
        : m_plugin(instance->name())
        , m_path(QString("%1-%2").arg(instance->uuid()))
        , m_name(dynamic_cast<const QObject*>(instance)->objectName()) {
    }

    inline operator QByteArray() const {
        return toByteArray();
    }

    inline bool operator==(const SessionModuleInstance& rhs) const {
        return m_name == rhs.m_name 
            && m_path == rhs.m_path && 
            m_plugin == rhs.m_plugin;
    }

    inline QByteArray toByteArray() const {
        return toString().toLocal8Bit();
    }

    inline QString toString() const {
        return QString("%1@%2").arg(m_path).arg(m_name);
    }
};

typedef QList<SessionModuleInstance> SessionModuleInstanceList;

SessionModuleInstanceList parseSessionModuleInstanceList(const QStringList& data);

QStringList serializeSessionModuleInstanceList(const SessionModuleInstanceList& list);

struct SessionSettings {
    static constexpr const char* const instancesKey = "instances";
    static constexpr const std::initializer_list<SessionModuleInstance> instancesValue = {};
    static constexpr const char* const lockKey = "lock";
    static constexpr const bool lockValue = false;

    SessionSettings()
        : instances(instancesValue)
        , lock(lockValue) {
    }

    SessionSettings(const QSettings& settings, const QString& settingsPath)
        : instances(parseSessionModuleInstanceList(settings.value(settingsPath + "/" + instancesKey, serializeSessionModuleInstanceList(instancesValue)).toStringList())) //TODO: jesus christ
        , lock(settings.value(settingsPath + "/" + lockKey, lockValue).toBool()) {
    }

    void save(QSettings& settings, const QString& settingsPath) {
        settings.setValue(settingsPath + "/" + instancesKey, serializeSessionModuleInstanceList(instances));
        settings.setValue(settingsPath + "/" + lockKey, lock);
    }

    SessionModuleInstanceList instances;
    bool lock;
};

struct SessionData {
    SessionData(Logger* logger, MLoader* plugins, MainWindow* win, const SessionSettings& settings) 
        : m_logger(logger)
        , m_plugins(plugins)
        , m_win(win)
        , m_settings(settings){
    }
    
    Logger* m_logger = nullptr;
    MLoader* m_plugins = nullptr;
    MainWindow* m_win = nullptr;
    bool m_changed = false;
    SessionSettings m_settings;
};

class Session : public QObject {
    
    Q_OBJECT

public:

    Session(MLoader* plugins, Logger* log, const QString& path, MainWindow* win);

    inline MLoader* plugins() const;

    inline Logger* logger() const;

    inline MainWindow* window() const;

    inline bool hasChanged() const;

public slots:

    qint64 store();

    qint64 restore();

    void aboutToQuit(const MainWindowQuit& type);

    void loaded(const Plugin* plugin);

    void aboutToDelete(const Plugin* plugin);

    void enabled(const Loader* loader);

    void disabled(const Loader* loader);

    void settingsApplied();

signals:

    void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

    void sessionRestored();

private:

    QStringList orderOfModules() const;

    SessionData m_data;
};

#endif