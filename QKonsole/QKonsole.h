#ifndef QKONSOLE_H
#define QKONSOLE_H

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QHBoxLayout>

#include "qkonsole_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "terminal.h"

class QKONSOLE_EXPORT QKonsole : public Widget
{
    Q_OBJECT

signals:
    void getData(const QByteArray& data);

public:
    QKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, KonsoleSettings* set, const QString& uuid);

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QKonsole::reset(Reset type)");
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:
    void settingsChanged();

    void enterPressed(const QString& command);

    void putData(const QByteArray& data);

    QVariant exec() override {
        return QVariant();
    }

private:
    QTerminal* m_terminal = nullptr;
    QSharedPointer<IODevice> m_serial;
};

#endif
