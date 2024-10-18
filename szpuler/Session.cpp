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

    return ret;
}

Session::Session(MLoader* plugins, Logger* log, const QString& path, MainWindow* win)
    : m_data(log, plugins, win, SessionSettings(Settings::get(), "Session")) {
}

QStringList Session::orderOfModules() const {
    auto exts = plugins()->loaders();
    QStringList order;

    while (exts.size() > 0) {
        for (int i = 0; i < exts.size() && exts.size() > 0; ++i) {
            if (exts[i]->depends().size() == 0) {
                order << exts[i]->name();
                exts.removeAt(i);
                --i;
            }
            else {
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

   // QByteArray state = window()->state();
    //FIX

 //   if (settings().status() != QSettings::NoError) {
 //       m_data.m_logger->message("Session::store(): failed to save session/state");
 //   }
    
    auto order = orderOfModules();
    auto instances = plugins()->instances();
    SessionModuleInstanceList session;

    for (auto item : order) {
            for (auto instance : instances) {
                if (instance->name() == item) {
                    session << SessionModuleInstance(instance);
                    /*
                    if (instance->type() == Plugin::Type::WIDGET) {
                        result << instance->name() + "-" + instance->uuid() + "@" + m_data.m_win->windowTitleByInstance(dynamic_cast<Widget*>(instance.data())).value();
                    } else {
                        result << instance->name() + "-" + instance->uuid();
                    }
                    */
                }
            }
        }

       // settings().setValue("session/plugins", result);
       // if (settings().status() != QSettings::NoError) {
       //     m_data.m_logger->message("Session::store(): failed to save session/plugins");
       // }

    return 0;
}

qint64 Session::restore() {
    m_data.m_logger->message("Session::restore()");
    qint64 ret = 0;

    //TODO: ensure order is correct

    if (m_data.m_settings.instances.size() > 0) {
        m_data.m_logger->message("Session::restore(): restore plugins instances: " + serializeSessionModuleInstanceList(m_data.m_settings.instances).join(","));
            
        for (auto it = m_data.m_settings.instances.begin(), end = m_data.m_settings.instances.end(); it != end; ++it, ++ret) {
            auto plugin = plugins()->instance(it->m_name, window(), it->m_path, ModuleHint::DONT_INITIALIZE);

            if (plugin.isNull()) {
                m_data.m_logger->message("Session::restore(): failed to create plugin: " + it->m_path);
            }
        }
         
        for (auto it = m_data.m_settings.instances.begin(), end = m_data.m_settings.instances.end(); it != end; ++it, ++ret) {
            auto plugin = plugins()->findByObjectName(it->m_name);

            if (plugin.isNull()) {
                m_data.m_logger->message("Session::restore(): failed find plugin instance: " + it->m_path);
                continue;
            }

            plugin->initialize();

            if (plugin->type() == Plugin::Type::WIDGET) {
                m_data.m_logger->message("Session::restore(): adding widget: " + *it);

                window()->addSubWindowInternal(dynamic_cast<Widget*>(plugin.data()), it->m_name);
            }
        }
    } else {
        m_data.m_logger->message("Session::restore() : empty");
        return 0;
    }

    //QByteArray state = settings().value("session/state").toByteArray();
    //if (state.isEmpty() == false) {
    //    m_logger->message("Session::restore() : restoring dock session");
        window()->setState(QByteArray());
    //}

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
    m_data.m_settings.instances << SessionModuleInstance(plugin);
}

void Session::aboutToDelete(const Plugin* plugin) {

}

void Session::enabled(const Loader* loader) {

}

void Session::disabled(const Loader* loader) {

}
