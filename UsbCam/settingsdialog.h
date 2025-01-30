#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>

#include "../api/api.h"

class UsbCam;

QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

struct UsbCamSettings : public PluginSettings {
    Q_GADGET
    Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString title READ title WRITE setTitle)

    static constexpr const char* imagePathKey = "imagePath";
    static constexpr const char* imagePathValue = nullptr;
    static constexpr const char* textKey = "text";
    static constexpr const char* textValue = nullptr;
    static constexpr const char* titleKey = "title";
    static constexpr const char* titleValue = nullptr;

public:
    UsbCamSettings()
        : m_imagePath(imagePathValue)
        , m_text(textValue)
        , m_title(titleValue) {
        registerMetaObject(&staticMetaObject);
    }

    virtual ~UsbCamSettings() = default;

    UsbCamSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_imagePath(settings.value(settingsPath + "/" + imagePathKey, imagePathValue).toString())
        , m_text(settings.value(settingsPath + "/" + textKey, textValue).toString())
        , m_title(settings.value(settingsPath + "/" + titleKey, titleValue).toString()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + imagePathKey, m_imagePath);
        settings.setValue(settingsPath + "/" + textKey, m_text);
        settings.setValue(settingsPath + "/" + titleKey, m_title);

        PluginSettings::save(settings, settingsPath);
    }

    QString imagePath() const {
        return m_imagePath;
    }

    void setImagePath(const QString& imagePath) {
        m_imagePath = imagePath;
    }

    QString text() const {
        return m_text;
    }

    void setText(const QString& text) {
        m_text = text;
    }

    QString title() const {
        return m_title;
    }

    void setTitle(const QString& title) {
        m_title = title;
    }

    bool operator==(const UsbCamSettings& other) const {
        return m_imagePath == other.m_imagePath
            && m_text == other.m_text
            && m_title == other.m_title;
    }

    bool operator!=(const UsbCamSettings& other) const {
        return !(*this == other);
    }

    QString m_imagePath;
    QString m_text;
    QString m_title;
};

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const UsbCam* badge);

    ~SettingsDialog();

    operator UsbCamSettings() const;

private slots:
    void ok();

    void apply();

    void cancel();

    void chooseImage();

private:
    void updateSettings();

    void fillFromSettings();

    bool verifySettings() const;

    void setup();

private:

    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
};

#endif
