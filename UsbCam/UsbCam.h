#ifndef USBCAM_H
#define USBCAM_H

#include "usbcam_global.h"
#include "settingsdialog.h"
#include "../api/api.h"

#include <QCamera>
#include <QImageCapture>
#include <QMediaRecorder>
#include <QScopedPointer>
#include <QMediaMetaData>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QAudioInput>

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

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

public slots:

    void settingsChanged();

    QVariant exec() override {
        return QVariant();
    }

    void saveMetaData();

private slots:
    void setCamera(const QCameraDevice& cameraDevice);

    void startCamera();
    void stopCamera();

    void record();
    void pause();
    void stop();
    void setMuted(bool);

    void takeImage();
    void displayCaptureError(int, QImageCapture::Error, const QString& errorString);

    void configureCaptureSettings();
    void configureVideoSettings();
    void configureImageSettings();

    void displayRecorderError();
    void displayCameraError();

    void updateCameraDevice(QAction* action);

    void updateCameraActive(bool active);
    void updateCaptureMode();
    void updateRecorderState(QMediaRecorder::RecorderState state);
    void setExposureCompensation(int index);

    void updateRecordTime();

    void processCapturedImage(int requestId, const QImage& img);

    void displayViewfinder();
    void displayCapturedImage();

    void readyForCapture(bool ready);
    void imageSaved(int id, const QString& fileName);

    void updateCameras();

    void showMetaDataDialog();

private:
    Ui::UsbCamUI* m_ui = nullptr;

    QActionGroup* videoDevicesGroup = nullptr;

    QMediaDevices m_devices;
    QScopedPointer<QImageCapture> m_imageCapture;
    QMediaCaptureSession m_captureSession;
    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QAudioInput> m_audioInput;
    QScopedPointer<QMediaRecorder> m_mediaRecorder;

    bool m_isCapturingImage = false;
    bool m_applicationExiting = false;
    bool m_doImageCapture = true;

    //MetaDataDialog* m_metaDataDialog = nullptr;
};

#endif