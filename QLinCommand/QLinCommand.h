#ifndef QLIN_COMMAND_H
#define QLIN_COMMAND_H

#include <QTimer>
#include <QPainter>
#include <QStyleOption>

#include "../api/api.h"
#include "settingsdialog.h"
#include "qlincommand_global.h"
#include "ui_lincommand.h"

enum class QLinCommandState {
    INITIAL,
    READ,
    PAUSE,
    STOP,
    ERR,
    SUCCESS
};

struct LinFrame {
    quint8 ID = 0xff;
    quint8 payload[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
};

struct PCI_t {
    quint8 len : 4;
    quint8 type : 4;
};

struct UDSgenericFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 SID;
    quint8 data[5] = { 0xff, 0xff, 0xff, 0xff, 0xff };
};

struct UDSnegativeFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 NSID;
    quint8 SID;
    quint8 SFID;
};

struct UDSsessionFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 SID;
    quint8 SFID;
    quint8 P2[2];
    quint8 P2Server[2];
};

union UDSframe {
    UDSgenericFrame generic;
    UDSnegativeFrame negative;
    UDSsessionFrame session;
};

struct QLinCommandData {
    QLinCommandData(Ui::QLinCommandUI* ui = nullptr)
        : m_ui(ui) {
    }

    Ui::QLinCommandUI* m_ui = nullptr;
    QSharedPointer<IODevice> m_lin = nullptr;
    UDSgenericFrame m_frame;
    QLinCommandState m_state = QLinCommandState::INITIAL;
    QTimer m_timer;
    qint32 m_try = 0;
    qint64 m_startTime;
    QEventLoop* m_loop = nullptr;
    int m_resched = 0;
};

class QLINCOMMAND_EXPORT QLinCommand : public Widget {
    Q_OBJECT

public:
    QLinCommand(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, LinCommandSettings* set, const QString& uuid);

    ~QLinCommand();

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QLinCommand::reset(Reset type)");
        initial();
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:

    void settingsChanged();

    void dataReady(const QByteArray& data);

    void send();

    void sendCommand(bool checked = true);

    void errorReady(const QByteArray& err);

    QVariant exec() {
        if (m_data.m_state != QLinCommandState::INITIAL) {
            reset();
        }

        QEventLoop loop;
        m_data.m_loop = &loop;
        sendCommand();
        if (m_data.m_state == QLinCommandState::READ) {
            loop.exec();
        }
        m_data.m_loop = nullptr;

        return (m_data.m_state == QLinCommandState::SUCCESS) ? QVariant(true) : QVariant();
    }

protected slots:

    void linClosed();

    void commandTimeout();

    void previousSuccess(const QByteArray& data);

signals:

    void success(const QByteArray& data);

protected:

    std::optional<LinFrame> dataFromResponse(const QByteArray& data) const;

    bool processFrame(const UDSframe* frame);

    QString errorString(quint8 err) const;

    void success();

    void failed();

    void inprogress();

    void initial();

    void paintEvent(QPaintEvent*) {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

private:

    QLinCommandData m_data;
};


#endif