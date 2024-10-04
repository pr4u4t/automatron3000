#ifndef QLINREADBYID_H
#define QLINREADBYID_H

#include "qlinreadbyid_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_linreadbyid.h"
#include <QTimer>
#include <QStyleOption>
#include <QPainter>

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
};

class QLINREADBYID_EXPORT QLinReadByID : public Widget
{
    Q_OBJECT

public:
    QLinReadByID(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    ~QLinReadByID();

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        initial();
        return true;
    }

signals:

    void success(const QByteArray& data);

    void failed();

public slots:
    void settingsChanged();

    void dataReady(const QByteArray& data);

    void startRead();

    void init();

    void readById(bool checked = true);

protected slots:

    void linClosed();

    void linOpened();

    std::optional<LinFrame> dataFromResponse(const QByteArray& data) const;

    void previousSuccess(const QByteArray& data) {
        
    }

protected:
       
    void paintEvent(QPaintEvent*) {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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