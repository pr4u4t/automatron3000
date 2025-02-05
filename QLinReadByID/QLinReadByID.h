#ifndef QLINREADBYID_H
#define QLINREADBYID_H

#include "qlinreadbyid_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_linreadbyid.h"

#include <QTimer>
#include <QStyleOption>
#include <QPainter>
#include <QVector>

QT_BEGIN_NAMESPACE

namespace Ui {
    class QLinReadByIDUI;
}

//class QIntValidator;
QT_END_NAMESPACE

enum class QLinReadByIDState {
    INITIAL,
    READ,
    PAUSE,
    STOP,
    ERR,
    SUCCESS
};

enum class PCITypes : quint8 {
    SF = 0,
    FF = 1,
    CF = 2
};

struct PCI_t {
    quint8 len : 4;
    quint8 type : 4;
};

struct LinFrame {
    quint8 ID = 0xff;
    quint8 payload[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
};

struct UDSgenericFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 data[6];
};

struct UDSsingleFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 SID;
    quint8 DIDH;
    quint8 DIDL;
    quint8 data[3];
};

struct UDSfirstFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 LEN;
    quint8 SID;
    quint8 DIDH;
    quint8 DIDL;
    quint8 data[2];
};

struct UDSconsecutiveFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 data[6];

    QByteArray dump() const {
        QByteArray ret;

        ret += "NAD: ";
        QString tmp = QString::number(NAD, 16);
        if (tmp.size() == 1) {
            tmp.prepend("0");
        }
        ret += tmp.toLocal8Bit() + ", ";

        ret += "PCI: ";
        tmp = QString::number(PCI.type);
        if (tmp.size() == 1) {
            tmp.prepend("0");
        }
        ret += tmp.toLocal8Bit() + ", ";

        tmp = QString::number(PCI.len);
        if (tmp.size() == 1) {
            tmp.prepend("0");
        }
        ret += tmp.toLocal8Bit() + ", ";

        for (int i = 0; i < sizeof(data); ++i) {
            tmp = QString::number(data[i], 16);
            if (tmp.size() == 1) {
                tmp.prepend("0");
            }
            ret += tmp.toLocal8Bit() + " ";
        }

        return ret;
    }
};

union UDSFrame {
    UDSgenericFrame df;
    UDSsingleFrame sf;
    UDSfirstFrame ff;
    UDSconsecutiveFrame cf;
};

struct QLinReadByIDData {
    QLinReadByIDData(Ui::QLinReadByIDUI* ui = nullptr, QLinReadByIDState state = QLinReadByIDState::INITIAL)
        : m_ui(ui)
        , m_state(state){
    }

    Ui::QLinReadByIDUI* m_ui = nullptr;
    QSharedPointer<IODevice> m_lin = nullptr;
    QLinReadByIDState m_state;
    int m_responses = 0;
    QTimer m_timer;
    int m_try = 0;
    UDSgenericFrame m_frame;
    QString m_result;
    int m_remaining;
    qint64 m_startTime;
    int m_resched = 0;
    QEventLoop* m_loop = nullptr;
    QVector<int> m_frames;
};

class QLINREADBYID_EXPORT QLinReadByID : public Widget {
    Q_OBJECT

public:
    QLinReadByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, LinReadByIDSettings* set, const QString& uuid);

    ~QLinReadByID();

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QLinReadByID::reset(Reset type)");
        initial();
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

signals:

    void success(const QByteArray& data);

    void failed();

public slots:
    void settingsChanged();

    void dataReady(const QByteArray& data);

    void startRead();

    void readById(bool checked = true);

    Q_INVOKABLE QVariant exec() override {
        if (m_data.m_state != QLinReadByIDState::INITIAL) {
            reset();
        }

        QEventLoop loop;
        m_data.m_loop = &loop;
        readById();
        if (m_data.m_state == QLinReadByIDState::READ) {
            loop.exec();
        }
        m_data.m_loop = nullptr;
        
        return (m_data.m_state == QLinReadByIDState::SUCCESS) ? QVariant(true) : QVariant(false);
    }

protected slots:

    void linClosed();

    void linOpened();

    std::optional<LinFrame> dataFromResponse(const QByteArray& data) const;

    void previousSuccess(const QByteArray& data) {
        emit message("QLinReadByID::previousSuccess(const QByteArray& data)");
        readById();
    }

protected:
       
    void paintEvent(QPaintEvent*) {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    void setAsciiResult(const QString& hexResult) {
        QStringList list = hexResult.split(' ');
        QString result;
        int pairs = 0;

        for (const auto& i : list) {
            result += QChar(i.toInt(nullptr, 16));
            result += ' ';
            ++pairs;
        }

        m_data.m_ui->resultAscii->setText(result);
        m_data.m_ui->lenLabel->setText(QString::number(pairs-1));
    }

    bool processSingleFrame(const UDSsingleFrame* frame);

    bool processFirstFrame(const UDSfirstFrame* frame);

    bool processConsecutiveFrame(const UDSconsecutiveFrame* frame);

    void success();

    void stateFailed();

    void inprogress();

    void initial();

private:
    
    QLinReadByIDData m_data;
};

#endif