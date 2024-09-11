#include "Session.h"
#include "MainWindow.h"
#include "../api/plugin.h"

Session::Session(MLoader* plugins, Logger* log, const QString& path, MainWindow* win)
    : m_plugins(plugins)
    , m_logger(log)
    , m_win(win)
    , m_settings(Settings::get()){
}

qint64 Session::store() {
    m_logger->message("Sesson::store()");
    QSettings& setts = settings();
    QString value;

    QByteArray state = window()->state();
    settings().setValue("session/state", state);

    if (settings().status() != QSettings::NoError) {
        m_logger->message("Session::store(): failed to save session/state");
    }

    QList<QSharedPointer<Plugin>> exts = plugins()->instances();
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

        QStringList result;
        auto instances = plugins()->instances();

        for (auto item : order) {
            for (auto instance : instances) {
                if (instance->name() == item) {
                    if (instance->type() == Plugin::Type::WIDGET) {
                        result << instance->name() + "-" + instance->uuid() + "@" + m_win->windowTitleByInstance(dynamic_cast<Widget*>(instance.data())).value();
                    } else {
                        result << instance->name() + "-" + instance->uuid();
                    }

                    break;
                }
            }
        }

        settings().setValue("session/plugins", result);
        if (settings().status() != QSettings::NoError) {
            m_logger->message("Session::store(): failed to save session/plugins");
        }

        return 0;
}

qint64 Session::restore() {
    m_logger->message("Session::restore()");

    const QVariant value = settings().value("session/plugins");
    const QStringList list = value.toStringList();
    qint64 ret = 0;
    QRegExp rx("^([A-Za-z0-9]+)");

    if (list.size() > 0) {
        m_logger->message("Session::restore(): restore plugins instances: " + list.join(","));
            for (auto it = list.begin(), end = list.end(); it != end; ++it, ++ret) {
                m_logger->message("Session::restore(): creating plugin: " + *it);
                if (rx.indexIn(*it) != -1) {
                    QString name = rx.cap(1);
                    const QStringList lst = it->split("@");

                    auto plugin = plugins()->instance(name, window(), lst[0]);

                    if (plugin.isNull()) {
                        m_logger->message("Session::restore(): failed to create plugin: " + *it);
                        continue;
                    }

                    if (plugin->type() == Plugin::Type::WIDGET) {
                        m_logger->message("Session::restore(): adding widget: " + *it);
                        
                        window()->addSubWindowInternal(dynamic_cast<Widget*>(plugin.data()), lst[1]);

                        //ads::CDockWidget* dockWidget = new ads::CDockWidget(plugin->name());
                        //dockWidget->setWidget(dynamic_cast<Widget*>(plugin.data()));
                        //dockWidget->setFeature(ads::CDockWidget::DontDeleteContent, true);
                        //m_dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget, m_area);
                    }
                }
            }
    } else {
        m_logger->message("Session::restore() : empty");
        return 0;
    }

    QByteArray state = settings().value("session/state").toByteArray();
    if (state.isEmpty() == false) {
        m_logger->message("Session::restore() : restoring dock session");
        window()->setState(state);
    }

    return ret;
}

QSettings& Session::settings() {
    return m_settings;
}

MLoader* Session::plugins() const {
    return m_plugins;
}

Logger* Session::logger() const {
    return m_logger;
}

MainWindow* Session::window() const {
    return m_win;
}
