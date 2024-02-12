#ifndef QKONSOLE_H
#define QKONSOLE_H

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QHBoxLayout>

#include "qkonsole_global.h"
#include "../api/api.h"

class QKonsole;

class Terminal : public QPlainTextEdit {
    Q_OBJECT

signals:
    void getData(const QByteArray& data);

    void logMessage(const QString& msg, LoggerSeverity severity = LoggerSeverity::NOTICE);

    void enterPressed();

public:
    Terminal(QKonsole* parent = nullptr);

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

    QString prompt() const {
        return m_prompt;
    }

    void setPrompt(const QString& prompt) {
        m_prompt = prompt;
    }

public slots:
    void settingsChanged();

    void enterPressed();

protected:

    void putPrompt(const QString& prompt);

private:
    Terminal* m_terminal = nullptr;
    QString m_prompt = "#> ";
};

#endif
