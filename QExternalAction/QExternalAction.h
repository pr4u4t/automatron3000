#ifndef QJTAG_H
#define QJTAG_H

#include <QProcess>

#include "qexternalaction_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

#include <QStyleOption>
#include <QPainter>

QT_BEGIN_NAMESPACE

namespace Ui {
    class QExternalActionUI;
}

//class QIntValidator;
QT_END_NAMESPACE

struct QExternalActionData {
    QExternalActionData(Ui::QExternalActionUI* ui)
        : m_ui(ui){}

    Ui::QExternalActionUI* m_ui = nullptr;
    QProcess m_process;
    QString m_program;
    QStringList m_arguments;
    int m_try;
};

class QEXTERNALACTION_EXPORT QExternalAction : public Widget {

    Q_OBJECT

public:
    QExternalAction(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, QExternalActionSettings* set, const QString& uuid);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~QExternalAction();

    SettingsMdi* settingsWindow() const;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QJTAG::reset(Reset type)");
        initial();
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:

    void settingsChanged();

    void command(bool checked = false);

    QVariant exec() override;

protected slots:

    void errorOccurred(QProcess::ProcessError error);

    void finished(int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit);

    void readyReadStandardError();

    void readyReadStandardOutput();

    void started();
    
    void stateChanged(QProcess::ProcessState newState);

    QString exitStatusString(QProcess::ExitStatus exitStatus) const;

    QString stateString(QProcess::ProcessState state) const;

    QString errorString(QProcess::ProcessError err) const;

    void success();

    void failed();

    void inprogress();

    void initial();

    void previousSuccess(const QByteArray& data);

signals:

    void success(const QByteArray& data);

protected:

    void paintEvent(QPaintEvent*) {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

private:
    
    QExternalActionData m_data;
};

#endif