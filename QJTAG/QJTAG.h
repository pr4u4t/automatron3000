#ifndef QJTAG_H
#define QJTAG_H

#include "qjtag_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class QJTAGUI;
}

//class QIntValidator;
QT_END_NAMESPACE

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

public slots:

    void settingsChanged();

    //protected:

    //    void resizeEvent(QResizeEvent* event);

private:
    Ui::QJTAGUI* m_ui = nullptr;
};

#endif