#include "Session.h"
#include "MainWindow.h"
#include "../api/plugin.h"

SessionModuleInstanceList parseSessionModuleInstanceList(const QStringList& data) {
     SessionModuleInstanceList ret;
     
     for (auto it = data.begin(), end = data.end(); it != end; ++it) {
         ret << SessionModuleInstance((*it).toLocal8Bit());
     }

     return ret;
}

QStringList serializeSessionModuleInstanceList(const SessionModuleInstanceList& list) {
    QStringList ret;
    for (const auto& item : list) {
        ret << item.toString();
    }

    return ret;
}

Session::Session(MLoader* plugins, Logger* log, const QString& path, MainWindow* win)
    : m_data(log, plugins, win, SessionSettings(Settings::get(), "Session")) {
}

QStringList Session::orderOfModules() const {
    emit message("Session::orderOfModules()");

    auto exts = plugins()->loaders(MLoader::LoaderState::ENABLED);
    QStringList order;

    while (exts.size() > 0) {
        for (int i = 0; i < exts.size() && exts.size() > 0; ++i) {
            if (exts[i]->depends().size() == 0) {
                order << exts[i]->name();
                exts.removeAt(i);
                --i;
            } else {
                QStringList tmp = exts[i]->depends();
                QSet<QString> deps(tmp.begin(), tmp.end());
                QSet<QString> met(order.begin(), order.end());
                if (deps.subtract(met).size() == 0) {
                    order << exts[i]->name();
                    exts.removeAt(i);
                    --i;
                }
            }
        }
    }

    order.removeDuplicates();
    return order;
}

qint64 Session::store() {
    m_data.m_logger->message("Sesson::store()");
    QString value;
    
    auto order = orderOfModules();
    auto instances = plugins()->instances();

    for (auto item : order) {
        for (auto instance : instances) {
            if (instance->name() == item) {
                SessionModuleInstance tmp(instance);
                if (m_data.m_settings.instances.indexOf(tmp) == -1) {
                    m_data.m_settings.instances << tmp;
                }
            }
        }
    }

    SessionModuleInstanceList ordered;
    for (auto item : order) {
        for (const auto& instance : m_data.m_settings.instances) {
            if (instance.m_plugin == item && ordered.indexOf(instance) == -1) {
                ordered << instance;
            }
        }
    }

    m_data.m_settings.instances = ordered;
    auto set = Settings::get();
    m_data.m_settings.save(set, "Session");

    auto inst = plugins()->instances();

    for (auto i = inst.begin(), end = inst.end(); i != end; ++i) {
        const auto s = i->data()->settings<PluginSettings>(); 
        const QString path = i->data()->settingsPath();
        m_data.m_logger->message(QString("Sesson::store: saving %1").arg(path));
        s->save(set, path);
    }

    return 0;
}

qint64 Session::restore() {
    m_data.m_logger->message("Session::restore()");
    qint64 ret = 0;

    //TODO: ensure order is correct

    if (m_data.m_settings.instances.size() > 0) {
        m_data.m_logger->message("Session::restore(): restore plugins instances: " + serializeSessionModuleInstanceList(m_data.m_settings.instances).join(","));
        
        SessionModuleInstanceList ordered;

        auto order = orderOfModules();
        for (const auto& item : order) {
            for (const auto& instance : m_data.m_settings.instances) {
                if (item == instance.m_plugin) {
                    if (ordered.lastIndexOf(instance) == -1) {
                        m_data.m_logger->message(QString("Session::restore: adding module to order: %1").arg(instance.toString()));
                        ordered << instance;
                    } else {
                        m_data.m_logger->message(QString("Session::restore: possible module duplicate %1").arg(instance.toString()));
                    }
                }
            }
        }

        for (auto it = ordered.begin(), end = ordered.end(); it != end; ++it, ++ret) {
            auto plugin = plugins()->instance(it->m_plugin, window(), it->m_path, ModuleHint::DONT_INITIALIZE);

            if (plugin.isNull()) {
                m_data.m_logger->message("Session::restore: failed to create plugin: " + it->m_path);
            }
        }
         
        for (auto it = ordered.begin(), end = ordered.end(); it != end; ++it, ++ret) {
            auto plugin = plugins()->findByObjectName(it->m_name);

            if (plugin.isNull()) {
                m_data.m_logger->message("Session::restore: failed find plugin instance: " + it->m_path);
                continue;
            }

            plugin->initialize();

            if (plugin->type() == Plugin::Type::WIDGET) {
                m_data.m_logger->message("Session::restore: adding widget: " + *it);

                window()->addSubWindowInternal(dynamic_cast<Widget*>(plugin.data()), it->m_name);
            }
        }
    } else {
        m_data.m_logger->message("Session::restore: empty");
        return 0;
    }

    emit sessionRestored();

    return ret;
}

void Session::aboutToQuit(const MainWindowQuit& type) {
    if (type == MainWindowQuit::QUIT_SAVE) {
        store();
    }
}

MLoader* Session::plugins() const {
    return m_data.m_plugins;
}

Logger* Session::logger() const {
    return m_data.m_logger;
}

MainWindow* Session::window() const {
    return m_data.m_win;
}

void Session::loaded(const Plugin* plugin) {
    emit message("Session::loaded(const Plugin* plugin)");
    //m_data.m_settings.instances << SessionModuleInstance(plugin);
    emit message(QString("Session::loaded: loaded: %1, version: %2").arg(plugin->name()).arg(plugin->version()));
    m_data.m_changed = true;
    const QObject* object = dynamic_cast<const QObject*>(plugin);
    QObject::connect(object, SIGNAL(settingsApplied()), this, SLOT(settingsApplied()));
}

void Session::aboutToDelete(const Plugin* plugin) {
    emit message("Session::aboutToDelete(const Plugin* plugin)");
    emit message(QString("Session::aboutToDelete: is about to be deleted name: %1, uuid: %2").arg(plugin->name()).arg(plugin->uuid()));
    m_data.m_changed = true;
}

void Session::enabled(const Loader* loader) {
    emit message("Session::enabled(const Loader * loader)");
    emit message(QString("Session::enabled: plugin enabled name: %1").arg(loader->name()));
    m_data.m_changed = true;
}

void Session::disabled(const Loader* loader) {
    emit message("Session::disabled(const Loader* loader)");
    m_data.m_changed = true;
}

bool Session::hasChanged() const {
    return m_data.m_changed;
}

void Session::settingsApplied() { 
    emit message("Session::settingsApplied()");
    const QObject* source = sender();
    const Plugin* plugin = dynamic_cast<const Plugin*>(source);
    if (plugin == nullptr) {
        emit message("Session::settingsApplied: signal source is not a plugin");
        return;
    }
    emit message(QString("Session::settingsApplied: settings applied to plugin name: %1, uuid: %2").arg(plugin->name()).arg(plugin->uuid()));
    m_data.m_changed = true;
}