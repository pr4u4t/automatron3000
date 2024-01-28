#ifndef QKONSOLE_H
#define QKONSOLE_H

#include "qkonsole_global.h"
#include "../ModuleLoader.h"
#include "console.h"

class QKONSOLE_EXPORT QKonsole : public Plugin
{
    Q_OBJECT

public:
    QKonsole(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath)
        : Plugin(ld, plugins, parent, settingsPath) {}

    /*bool saveSettings() const {
        return true;
    }*/

    QSharedPointer<MdiChild> widget() override {
        if (m_console == nullptr) {
            m_console = QSharedPointer<Console>(new Console(this, nullptr));
        }

        return m_console;
    }
private:
    QSharedPointer<Console> m_console;
};

#endif
