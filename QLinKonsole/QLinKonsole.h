#ifndef QLINKONSOLE_H
#define QLINKONSOLE_H

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QHBoxLayout>

#include "qlinkonsole_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "terminal.h"

class QLINKONSOLE_EXPORT QLinKonsole : public Widget
{
    Q_OBJECT

signals:
    void getData(const QByteArray& data);

public:
    QLinKonsole(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

public slots:
    void settingsChanged();

    void enterPressed(const QString& command);

    void putData(const QByteArray& data);

private:
    QTerminal* m_terminal = nullptr;
    SettingsDialog::KonsoleSettings m_settings;
    QSharedPointer<IODevice> m_lin;
};

#endif
