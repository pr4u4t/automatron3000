#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QIntValidator>
#include <QSettings>
#include <QString>
#include <QRandomGenerator>

#include "../api/api.h"


QT_BEGIN_NAMESPACE

namespace Ui {
    class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    static constexpr const char* imagePathKey = "imagePath";
    static constexpr const char* imagePathValue = nullptr;
    static constexpr const char* textKey = "text";
    static constexpr const char* textValue = nullptr;
    static constexpr const char* titleKey = "title";
    static constexpr const char* titleValue = nullptr;

    struct BadgeSettings : public PluginSettings {
        BadgeSettings() 
            : imagePath(imagePathValue)
            , text(textValue)
            , title(titleValue){
        }

        BadgeSettings(const QSettings& settings, const QString& settingsPath)
            : imagePath(settings.value(settingsPath + "/" + imagePathKey, imagePathValue).toString())
            , text(settings.value(settingsPath + "/" + textKey, textValue).toString())
            , title(settings.value(settingsPath + "/" + titleKey, titleValue).toString()){
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + imagePathKey, imagePath);
            settings.setValue(settingsPath + "/" + textKey, text);
            settings.setValue(settingsPath + "/" + titleKey, title);
        }

        QString imagePath;
        QString text;
        QString title;
    };

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    BadgeSettings dataSettings() const;

    QString settingsPath() const;

private slots:
    void ok();

    void apply();
    
    void cancel();

    void chooseImage();

private:
    void updateSettings();
    
    void fillFromSettings();

    bool verifySettings() const;

private:
    BadgeSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif