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

public slots:
    void settingsChanged();

    void dataReady(const QByteArray& data);

    void startTest();

    void testStep();

    void testStop();

    void init();

protected slots:

    void linClosed();

private:
    
    Ui::QLinTesterUI* m_ui = nullptr;
    uint16_t m_test = 0;
    QSharedPointer<IODevice> m_lin;
    QLinTesterState m_state;
    int m_responses = 0;
    QTimer m_timer;
    int m_try = 0;
    int m_slaveID = 0;
};

#endif