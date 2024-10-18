#ifndef QLINTESTER_H
#define QLINTESTER_H

#include <QTimer>

#include "qlintester_global.h"
#include "../api/api.h"
#include "settingsdialog.h"
#include "ui_qlintester.h"

enum class QLinTesterState {
    INITIAL,
    SCAN,
    NEXT_TRY,
    PAUSE,
    STOP
};

struct QLinTesterData {
    QLinTesterData(Ui::QLinTesterUI* ui)
        : m_ui(ui){
    }

    Ui::QLinTesterUI* m_ui = nullptr;
    uint16_t m_test = 0;
    QSharedPointer<IODevice> m_lin = nullptr;
    QLinTesterState m_state = QLinTesterState::INITIAL;
    int m_responses = 0;
    QTimer m_timer;
    int m_try = 0;
    int m_slaveID = 0;
};

QT_BEGIN_NAMESPACE

namespace Ui {
    class QLinTesterUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class QLINTESTER_EXPORT QLinTester : public Widget {

    Q_OBJECT

public:
    QLinTester(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    SettingsMdi* settingsWindow() const;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QLinTester::reset(Reset type)");
        initial();
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:
    void settingsChanged();

    void dataReady(const QByteArray& data);

    void startTest();

    void testStep();

    void testStop();

    QVariant exec() {
        return QVariant();
    }

protected:

    void success();

    void failed();

    void inprogress();

    void initial();

protected slots:

    void linClosed();

private:
    
    QLinTesterData m_data;
};

#endif