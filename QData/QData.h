#ifndef QDATA_H
#define QDATA_H

#include "qdata_global.h"
#include "../ModuleLoader.h"
#include "datawindow.h"

class QDATA_EXPORT QData : public Plugin
{
    Q_OBJECT

public:
    QData(const Loader* ld, PluginsLoader* plugins, QWidget *parent, const QString& settingsPath)
        : Plugin(ld, plugins, parent, settingsPath) {}

    /*bool saveSettings() const {
        return true;
    }*/

    QSharedPointer<MdiChild> widget() override {
        if (m_datawindow == nullptr) {
            m_datawindow = QSharedPointer<DataWindow>(new DataWindow(this, nullptr));
        }

        return m_datawindow;
    }

private:
    QSharedPointer<DataWindow> m_datawindow;
};

#endif