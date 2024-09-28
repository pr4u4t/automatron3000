#ifndef QJTAG_H
#define QJTAG_H

#include <QProcess>

#include "qjtag_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QJTAGUI;
}

//class QIntValidator;
QT_END_NAMESPACE

struct QJTAGData {
    QJTAGData(Ui::QJTAGUI* ui)
        : m_ui(ui){}

    Ui::QJTAGUI* m_ui = nullptr;
    QProcess m_process;
    QString m_program;
    QStringList m_arguments;
};

class QJTAG_EXPORT QJTAG : public Widget {

    Q_OBJECT

public:
    QJTAG(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~QJTAG();

    SettingsMdi* settingsWindow() const;

public slots:

    void settingsChanged();

    void command(bool checked = false);

    void exec();

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
private:
    
    QJTAGData m_data;
};

#endif