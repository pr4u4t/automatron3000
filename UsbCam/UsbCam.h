#ifndef USBCAM_H
#define USBCAM_H

#include "usbcam_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

QT_BEGIN_NAMESPACE

namespace Ui {
    class UsbCamUI;
}

//class QIntValidator;
QT_END_NAMESPACE

class USBCAM_EXPORT UsbCam : public Widget {

    Q_OBJECT

public:
    UsbCam(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& settingsPath, UsbCamSettings* set, const QString& uuid);

    bool saveSettings();

    /*!
      \brief Destructor
      \param none
    */
    ~UsbCam();

    SettingsMdi* settingsWindow() const override;

    Q_INVOKABLE bool reset(Reset type = Reset::SOFT) {
        emit message("QBadge::reset(Reset type)");
        return true;
    }

    bool initialize() override;

    bool deinitialize() override;

public slots:

    void settingsChanged();

    QVariant exec() override {
        return QVariant();
    }

private:
    Ui::UsbCamUI* m_ui = nullptr;
};

#endif