#ifndef QKONSOLE_H
#define QKONSOLE_H

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QHBoxLayout>

#include "qkonsole_global.h"
#include "../ModuleLoader.h"

class Terminal : public QPlainTextEdit {
    Q_OBJECT

signals:
    void getData(const QByteArray& data);

    void logMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::NOTICE);

public:
    Terminal(QWidget* parent = nullptr);

    void setLocalEchoEnabled(bool set);

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void contextMenuEvent(QContextMenuEvent* e) override;

private:
    bool m_localEchoEnabled = false;
};


class QKONSOLE_EXPORT QKonsole : public Widget
{
    Q_OBJECT

signals:
    void getData(const QByteArray& data);

public:
    QKonsole(const Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    void putData(const QByteArray& data);

    bool saveSettings() {
        return true;
    }

public slots:
    void settingsChanged();

private:
    Terminal* m_terminal = nullptr;
};

#endif
