#include "UsbCam.h"

#include "../core/core.h"
#include "settingsdialog.h"
#include "ui_usbcam.h"

#include <QtGlobal>
#include <QtGui>

struct UsbCamMenu {
    UsbCamMenu(QCoreApplication* app)
        : m_app(app) {

        if (app != nullptr && Settings::localeNeeded()) {
            m_translator = new QTranslator();
            if (m_translator->load(QLocale::system(), "UsbCam", "_", "translations")) { //set directory of ts
                m_app->installTranslator(m_translator);
            }
        }
    }

    QMenu* m_usbcamMenu = nullptr;
    QMenu* m_settings = nullptr;
    QAction* m_newInstance = nullptr;

    QCoreApplication* m_app = nullptr;
    QTranslator* m_translator = nullptr;
};

static bool UsbCam_register(ModuleLoaderContext* ldctx, PluginsLoader* ld, UsbCamMenu* ctx, Logger* log) {
    log->message("UsbCam_register()");

    GuiLoaderContext* gtx = ldctx->to<GuiLoaderContext>();
    if (gtx == nullptr) {
        log->message("UsbCam_register: application is non gui not registering");
        return false;
    }

    if (gtx->m_win == nullptr) {
        log->message("UsbCam_register(): window pointer == nullptr");
        return false;
    }

    auto visualMenu = gtx->m_win->findMenu(ctx->m_app->translate("MainWindow", "V&isualization"));
    ctx->m_usbcamMenu = visualMenu->addMenu(ctx->m_app->translate("MainWindow", "UsbCam"));

    windowAddInstanceSettings(ctx->m_usbcamMenu, &ctx->m_settings, &ctx->m_newInstance, "UsbCam", ctx->m_app, log);

    QObject::connect(ctx->m_newInstance, &QAction::triggered, gtx->m_win, &Window::create);
    QObject::connect(ld, &PluginsLoader::loaded, [gtx, ctx, log, ld](const Plugin* plugin) {
        windowAddPluginSettingsAction<UsbCam, UsbCamMenu, GuiLoaderContext, SettingsDialog, Plugin>(plugin, QString("UsbCam"), gtx, ctx, log);
    });

    return true;
}

static bool UsbCam_unregister(ModuleLoaderContext* ldctx, PluginsLoader* ld, UsbCamMenu* ctx, Logger* log) {
    log->message("UsbCam_unregister()");
    return true;
}

REGISTER_PLUGIN(
    UsbCam,
    Plugin::Type::WIDGET,
    "0.0.1",
    "pawel.ciejka@gmail.com",
    "example plugin",
    UsbCam_register,
    UsbCam_unregister,
    UsbCamMenu,
    {},
    true,
    100,
    UsbCamSettings
)

UsbCam::UsbCam(Loader* ld, PluginsLoader* plugins, QWidget* parent, const QString& sPath, UsbCamSettings* set, const QString& uuid)
    : Widget(
        ld,
        plugins,
        parent,
        sPath,
        set,
        uuid
    )
    , m_ui(new Ui::UsbCamUI) {
    m_ui->setupUi(this);
}

bool UsbCam::saveSettings() {
    return true;
}

/*!
  \brief Destructor
  \param none
*/
UsbCam::~UsbCam() {
}

//void QBadge::resizeEvent(QResizeEvent* event) {
//    
//    m_pixmap = m_pixmap.scaled(event->size(), Qt::KeepAspectRatio);
//    m_ui->image->setPixmap(m_pixmap);
//
//    Widget::resizeEvent(event);
//}

bool UsbCam::initialize() {
    emit message("UsbCam::initialize()");
    //settingsChanged();
    const auto set = settings<UsbCamSettings>();
//    m_ui->label->setText(set->text());
    //m_ui->title->setText(set->title());
#if QT_CONFIG(permissions)
// camera
    QCameraPermission cameraPermission;
    switch (qApp->checkPermission(cameraPermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(cameraPermission, this, &UsbCam::initialize);
        return false;
    case Qt::PermissionStatus::Denied:
        qWarning("Camera permission is not granted!");
        return false;
    case Qt::PermissionStatus::Granted:
        break;
    }
    // microphone
    QMicrophonePermission microphonePermission;
    switch (qApp->checkPermission(microphonePermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(microphonePermission, this, &UsbCam::initialize);
        return false;
    case Qt::PermissionStatus::Denied:
        qWarning("Microphone permission is not granted!");
        return false;
    case Qt::PermissionStatus::Granted:
        break;
    }
#endif

    m_audioInput.reset(new QAudioInput);
    m_captureSession.setAudioInput(m_audioInput.get());

    // Camera devices:

    videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    updateCameras();
    connect(&m_devices, &QMediaDevices::videoInputsChanged, this, &UsbCam::updateCameras);

    connect(videoDevicesGroup, &QActionGroup::triggered, this, &UsbCam::updateCameraDevice);
    connect(m_ui->captureWidget, &QTabWidget::currentChanged, this, &UsbCam::updateCaptureMode);

    connect(m_ui->metaDataButton, &QPushButton::clicked, this, &UsbCam::showMetaDataDialog);
    connect(m_ui->exposureCompensation, &QAbstractSlider::valueChanged, this,
        &UsbCam::setExposureCompensation);

    setCamera(QMediaDevices::defaultVideoInput());

    return true;
}

bool UsbCam::deinitialize() {
    emit message("UsbCam::deinitialize()");
    return true;
}

void UsbCam::settingsChanged() {
    emit message("UsbCam::settingsChanged()", LoggerSeverity::LOG_DEBUG);
    const auto set = settings<UsbCamSettings>();
    const auto src = qobject_cast<SettingsDialog*>(sender());
    const auto nset = src->settings<UsbCamSettings>();
    *set = *nset;

    initialize();

    emit settingsApplied();
}

SettingsMdi* UsbCam::settingsWindow() const {
    emit message("UsbCam::settingsWindow()");
    auto ret = new SettingsDialog(nullptr, this);
    QObject::connect(ret, &SettingsDialog::settingsUpdated, this, &UsbCam::settingsChanged);
    return ret;
}

void UsbCam::setCamera(const QCameraDevice& cameraDevice){
    emit message("UsbCam::setCamera(const QCameraDevice& cameraDevice)");
    m_camera.reset(new QCamera(cameraDevice));
    m_captureSession.setCamera(m_camera.data());

    connect(m_camera.data(), &QCamera::activeChanged, this, &UsbCam::updateCameraActive);
    connect(m_camera.data(), &QCamera::errorOccurred, this, &UsbCam::displayCameraError);

    if (!m_mediaRecorder) {
        m_mediaRecorder.reset(new QMediaRecorder);
        m_captureSession.setRecorder(m_mediaRecorder.data());
        connect(m_mediaRecorder.data(), &QMediaRecorder::recorderStateChanged, this,
            &UsbCam::updateRecorderState);
        connect(m_mediaRecorder.data(), &QMediaRecorder::durationChanged, this,
            &UsbCam::updateRecordTime);
        connect(m_mediaRecorder.data(), &QMediaRecorder::errorChanged, this,
            &UsbCam::displayRecorderError);
    }

    if (!m_imageCapture) {
        m_imageCapture.reset(new QImageCapture);
        m_captureSession.setImageCapture(m_imageCapture.get());
        connect(m_imageCapture.get(), &QImageCapture::readyForCaptureChanged, this,
            &UsbCam::readyForCapture);
        connect(m_imageCapture.get(), &QImageCapture::imageCaptured, this,
            &UsbCam::processCapturedImage);
        connect(m_imageCapture.get(), &QImageCapture::imageSaved, this, &UsbCam::imageSaved);
        connect(m_imageCapture.get(), &QImageCapture::errorOccurred, this,
            &UsbCam::displayCaptureError);
    }

    m_captureSession.setVideoOutput(m_ui->viewfinder);

    updateCameraActive(m_camera->isActive());
    updateRecorderState(m_mediaRecorder->recorderState());
    readyForCapture(m_imageCapture->isReadyForCapture());

    updateCaptureMode();

    m_camera->start();
}

void UsbCam::keyPressEvent(QKeyEvent* event){
    emit message("UsbCam::keyPressEvent(QKeyEvent* event)");

    if (event->isAutoRepeat())
        return;

    /*switch (event->key()) {
    case Qt::Key_CameraFocus:
        displayViewfinder();
        event->accept();
        break;
    case Qt::Key_Camera:
        if (m_doImageCapture) {
            takeImage();
        }
        else {
            if (m_mediaRecorder->recorderState() == QMediaRecorder::RecordingState)
                stop();
            else
                record();
        }
        event->accept();
        break;
    default:*/
        Widget::keyPressEvent(event);
    //}
}

void UsbCam::keyReleaseEvent(QKeyEvent* event){
    emit message("UsbCam::keyReleaseEvent(QKeyEvent* event)");
    Widget::keyReleaseEvent(event);
}

void UsbCam::updateRecordTime(){
    emit message("UsbCam::updateRecordTime()");
    QString str = QString("Recorded %1 sec").arg(m_mediaRecorder->duration() / 1000);
    //m_ui->statusbar->showMessage(str);
}

void UsbCam::processCapturedImage(int requestId, const QImage& img){
    emit message("UsbCam::processCapturedImage(int requestId, const QImage& img)");
    Q_UNUSED(requestId);
    QImage scaledImage = img.scaled(m_ui->viewfinder->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    m_ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // Display captured image for 4 seconds.
    displayCapturedImage();
    QTimer::singleShot(4000, this, &UsbCam::displayViewfinder);
}

void UsbCam::configureCaptureSettings(){
    emit message("UsbCam::configureCaptureSettings()");

    if (m_doImageCapture) {
        configureImageSettings();
    } else {
        configureVideoSettings();
    }
}

void UsbCam::configureVideoSettings() {
    emit message("UsbCam::configureVideoSettings()");
    
    /*
    VideoSettings settingsDialog(m_mediaRecorder.data());
    settingsDialog.setWindowFlags(settingsDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if (settingsDialog.exec()) {
        settingsDialog.applySettings();
    }
    */
}

void UsbCam::configureImageSettings(){
    emit message("UsbCam::configureImageSettings()");
    /*
    ImageSettings settingsDialog(m_imageCapture);
    settingsDialog.setWindowFlags(settingsDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if (settingsDialog.exec()) {
        settingsDialog.applyImageSettings();
    }
    */
}

void UsbCam::record(){
    emit message("UsbCam::record()");
    m_mediaRecorder->record();
    updateRecordTime();
}

void UsbCam::pause(){
    emit message("UsbCam::pause()");
    m_mediaRecorder->pause();
}

void UsbCam::stop(){
    emit message("UsbCam::stop()");
    m_mediaRecorder->stop();
}

void UsbCam::setMuted(bool muted){
    emit message("UsbCam::setMuted(bool muted)");
    m_captureSession.audioInput()->setMuted(muted);
}

void UsbCam::takeImage(){
    emit message("UsbCam::takeImage()");
    m_isCapturingImage = true;
    m_imageCapture->captureToFile();
}

void UsbCam::displayCaptureError(int id, const QImageCapture::Error error, const QString& errorString){
    emit message("UsbCam::displayCaptureError(int id, const QImageCapture::Error error, const QString& errorString)");
    Q_UNUSED(id);
    Q_UNUSED(error);
    emit message(QString("UsbCam::displayCaptureError: %1").arg(errorString));
    m_isCapturingImage = false;
}

void UsbCam::startCamera(){
    emit message("UsbCam::startCamera()");
    m_camera->start();
}

void UsbCam::stopCamera(){
    emit message("UsbCam::stopCamera()");
    m_camera->stop();
}

void UsbCam::updateCaptureMode(){
    emit message("UsbCam::updateCaptureMode()");
    int tabIndex = m_ui->captureWidget->currentIndex();
    m_doImageCapture = (tabIndex == 0);
}

void UsbCam::updateCameraActive(bool active){
    emit message("UsbCam::updateCameraActive(bool active)");
    /*
    if (active) {
        m_ui->actionStartCamera->setEnabled(false);
        m_ui->actionStopCamera->setEnabled(true);
        m_ui->captureWidget->setEnabled(true);
        m_ui->actionSettings->setEnabled(true);
    } else {
        m_ui->actionStartCamera->setEnabled(true);
        m_ui->actionStopCamera->setEnabled(false);
        m_ui->captureWidget->setEnabled(false);
        m_ui->actionSettings->setEnabled(false);
    }
    */
}

void UsbCam::updateRecorderState(QMediaRecorder::RecorderState state){
    emit message("UsbCam::updateRecorderState(QMediaRecorder::RecorderState state)");
    switch (state) {
    case QMediaRecorder::StoppedState:
        m_ui->recordButton->setEnabled(true);
        m_ui->pauseButton->setEnabled(true);
        m_ui->stopButton->setEnabled(false);
        m_ui->metaDataButton->setEnabled(true);
        break;
    case QMediaRecorder::PausedState:
        m_ui->recordButton->setEnabled(true);
        m_ui->pauseButton->setEnabled(false);
        m_ui->stopButton->setEnabled(true);
        m_ui->metaDataButton->setEnabled(false);
        break;
    case QMediaRecorder::RecordingState:
        m_ui->recordButton->setEnabled(false);
        m_ui->pauseButton->setEnabled(true);
        m_ui->stopButton->setEnabled(true);
        m_ui->metaDataButton->setEnabled(false);
        break;
    }
}

void UsbCam::setExposureCompensation(int index){
    emit message("UsbCam::setExposureCompensation(int index)");
    m_camera->setExposureCompensation(index * 0.5);
}

void UsbCam::displayRecorderError(){
    emit message("UsbCam::displayRecorderError()");

    if (m_mediaRecorder->error() != QMediaRecorder::NoError) {
        emit message(QString("UsbCam::displayRecorderError: %1").arg(m_mediaRecorder->errorString()));
    }
}

void UsbCam::displayCameraError(){
    emit message("UsbCam::displayCameraError()");
    if (m_camera->error() != QCamera::NoError) {
        emit message(QString("UsbCam::displayCameraError: %1").arg(m_camera->errorString()));
    }
}

void UsbCam::updateCameraDevice(QAction* action){
    emit message("UsbCam::updateCameraDevice(QAction* action)");
    setCamera(qvariant_cast<QCameraDevice>(action->data()));
}

void UsbCam::displayViewfinder(){
    emit message("UsbCam::displayViewfinder()");
    m_ui->stackedWidget->setCurrentIndex(0);
}

void UsbCam::displayCapturedImage(){
    emit message("UsbCam::displayCapturedImage()");
    m_ui->stackedWidget->setCurrentIndex(1);
}

void UsbCam::readyForCapture(bool ready){
    emit message("UsbCam::readyForCapture(bool ready)");
    m_ui->takeImageButton->setEnabled(ready);
}

void UsbCam::imageSaved(int id, const QString& fileName){
    emit message("UsbCam::imageSaved(int id, const QString& fileName)");
    Q_UNUSED(id);
    //ui->statusbar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName)));

    m_isCapturingImage = false;
    if (m_applicationExiting) {
        close();
    }
}

void UsbCam::closeEvent(QCloseEvent* event){
    emit message("UsbCam::closeEvent(QCloseEvent* event)");
    /*if (m_isCapturingImage) {
        setEnabled(false);
        m_applicationExiting = true;
        event->ignore();
    }
    else {
        event->accept();
    }
    */
}

void UsbCam::updateCameras(){
    emit message("UsbCam::updateCameras()");
    //m_ui->menuDevices->clear();
    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    for (const QCameraDevice& cameraDevice : availableCameras) {
        QAction* videoDeviceAction = new QAction(cameraDevice.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraDevice));
        if (cameraDevice == QMediaDevices::defaultVideoInput())
            videoDeviceAction->setChecked(true);

        //m_ui->menuDevices->addAction(videoDeviceAction);
    }
}

void UsbCam::showMetaDataDialog(){
    emit message("UsbCam::showMetaDataDialog()");

    /*
    if (!m_metaDataDialog) {
        m_metaDataDialog = new MetaDataDialog(this);
    }

    m_metaDataDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    if (m_metaDataDialog->exec() == QDialog::Accepted) {
        saveMetaData();
    }
    */
}

void UsbCam::saveMetaData(){
    emit message("UsbCam::saveMetaData()");
    /*
    QMediaMetaData data;
    for (int i = 0; i < QMediaMetaData::NumMetaData; i++) {
        QString val = m_metaDataDialog->m_metaDataFields[i]->text();
        if (!val.isEmpty()) {
            auto key = static_cast<QMediaMetaData::Key>(i);
            if (i == QMediaMetaData::CoverArtImage) {
                QImage coverArt(val);
                data.insert(key, coverArt);
            }
            else if (i == QMediaMetaData::ThumbnailImage) {
                QImage thumbnail(val);
                data.insert(key, thumbnail);
            }
            else if (i == QMediaMetaData::Date) {
                QDateTime date = QDateTime::fromString(val);
                data.insert(key, date);
            }
            else {
                data.insert(key, val);
            }
        }
    }
    m_mediaRecorder->setMetaData(data);
    */
}
