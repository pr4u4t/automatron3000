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
    QKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    SettingsMdi* settingsWindow() const override;

public slots:
    void settingsChanged();

    void enterPressed(const QString& command);

    void putData(const QByteArray& data);

private:
    QTerminal* m_terminal = nullptr;
    QSharedPointer<IODevice> m_serial;
};

#endif
