#ifndef QLINWRITEBYID_H
#define QLINWRITEBYID_H

#include "qlinwritebyid_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_linwritebyid.h"
#include <QTimer>
#include <QQueue>
#include <QStyleOption>
#include <QPainter>

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif


#include <bitset>

QT_BEGIN_NAMESPACE

namespace Ui {
    class QLinWriteByIDUI;
}

//class QIntValidator;
QT_END_NAMESPACE

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
    quint8 payload[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    QByteArray dump() const {
        QByteArray ret;

        ret += "ID: ";
        QString tmp = QString::number(ID, 16);
        if (tmp.size() == 1) {
            tmp.prepend("0");
        }
        ret += tmp.toLocal8Bit() + " ";

        for (int i = 0; i < sizeof(payload); ++i) {
            tmp = QString::number(payload[i], 16);
            if (tmp.size() == 1) {
                tmp.prepend("0");
            }
            ret += tmp.toLocal8Bit() + " ";
        }

        return ret;
    }
};

struct UDSgenericFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 data[6];
};

struct UDSserviceFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 SID;
    quint8 data[5];
};

struct UDSsingleFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 SID;
    quint8 DIDH;
    quint8 DIDL;
    quint8 data[3];
};

/*
PACK(struct FFPCI_t {
    quint16 type : 4;
    quint16 len : 12;
});
*/

struct FFPCI_t {
    quint8 type;
    quint8 len;
};

PACK(struct UDSfirstFrame {
    quint8 NAD;
    FFPCI_t pci;
    quint8 SID;
    quint8 DIDH;
    quint8 DIDL;
    quint8 data[2];
});

struct UDSconsecutiveFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 data[6];
};

struct UDSnegativeFrame {
    quint8 NAD;
    PCI_t PCI;
    quint8 NSID;
    quint8 SID;
    quint8 SFID;
};

union UDSFrame {
    quint8 payload[8] = {0, 0, 0, 0, 0, 0, 0, 0 };
    UDSgenericFrame df;
    UDSsingleFrame sf;
    UDSfirstFrame ff;
    UDSconsecutiveFrame cf;
    UDSserviceFrame service;
    UDSnegativeFrame negative;
};

struct LinUdsFrame {
    quint8 ID = 0xff;
    UDSFrame uds;

    int length = sizeof(ID);

    operator QByteArray() const {
        QByteArray ret(length, 0);
        *(ret.data()) = ID;
        if (length > 1) {
            memcpy(ret.data()+1, &uds, length-1);
        }

        return ret;
    }

    QByteArray dump() const {
        QByteArray ret;

        ret += "ID: ";
        QString tmp = QString::number(ID, 16);
        if (tmp.size() == 1) {
            tmp.prepend("0");
        }
        ret += tmp.toLocal8Bit()+" ";

        for (int i = 0; i < length - 1; ++i) {
            tmp = QString::number(uds.payload[i], 16);
            if (tmp.size() == 1) {
                tmp.prepend("0");
            }
            ret += tmp.toLocal8Bit()+" ";
        }

        return ret;
    }
};

enum class QLinWriteByIDState {
    INITIAL,
    WRITE,
    WAIT,
    PAUSE,
    STOP,
    ERR,
    SUCCESS
};

struct QLinWriteByIDData {

    QLinWriteByIDData(Ui::QLinWriteByIDUI* ui)
        : m_ui(ui) {
    }

    Ui::QLinWriteByIDUI* m_ui = nullptr;
    uint16_t m_test = 0;
    QSharedPointer<IODevice> m_lin = nullptr;
    QLinWriteByIDState m_state = QLinWriteByIDState::INITIAL;
    int m_responses = 0;
    QTimer m_timer;
    int m_try = 0;
    UDSgenericFrame m_frame;
    QEventLoop* m_loop = nullptr;
    int m_resched = 0;
};

class QLINWRITEBYID_EXPORT QLinWriteByID : public Widget {
    Q_OBJECT

public:
    QLinWriteByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, LinWriteByIDSettings* set, const QString& uuid);

    ~QLinWriteByID();

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QLinWriteByID::reset(Reset type)");
        initial();
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:
    void settingsChanged();

    void dataReady(const QByteArray& data);

    void startWrite();

    void linClosed();

    void writeById();

    QVariant exec() {
        emit message("QLinWriteByID::exec()");
        if (m_data.m_state != QLinWriteByIDState::INITIAL) {
            reset();
        }

        QEventLoop loop;
        m_data.m_loop = &loop;
        writeById();
        if (m_data.m_state == QLinWriteByIDState::WRITE) {
            loop.exec();
        }
        m_data.m_loop = nullptr;

        return (m_data.m_state == QLinWriteByIDState::SUCCESS) ? QVariant(true) : QVariant();
    }

protected slots:

    void fillInput(const QByteArray& data);

protected:

    void paintEvent(QPaintEvent*){
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    std::optional<LinFrame> dataFromResponse(const QByteArray& data) const;

    bool processFrame(const UDSFrame* frame);

    QString errorString(quint8 err) const;

    QQueue<LinUdsFrame> prepareFrames(const QByteArrayList& data);//, const UDSserviceFrame* service);

    void success();

    void failed();

    void inprogress();

    void initial();

private:

    QLinWriteByIDData m_data;
    
};

#endif