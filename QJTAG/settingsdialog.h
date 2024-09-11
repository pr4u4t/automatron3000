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

    static constexpr const char* const titleKey = "title";
    static constexpr const char* const titleValue = "JTAG";
    static constexpr const char* const buttonLabelKey = "buttonLabel";
    static constexpr const char* const buttonLabelValue = "Execute";
    static constexpr const char* const programPathKey = "path";
    static constexpr const char* const programPathValue = nullptr;
    static constexpr const char* const argumentsKey = "arguments";
    static constexpr const char* const argumentsValue = nullptr;

    struct QJTAGSettings : public PluginSettings {
        QJTAGSettings() 
            : title(titleValue)
            , buttonLabel(buttonLabelValue)
            , programPath(programPath)
            , arguments(argumentsValue){
        }

        QJTAGSettings(const QSettings& settings, const QString& settingsPath)
            : PluginSettings(settings, settingsPath)
            , title(settings.value(settingsPath + "/" + titleKey, titleValue).toString())
            , buttonLabel(settings.value(settingsPath + "/" + buttonLabelKey, buttonLabelValue).toString())
            , programPath(settings.value(settingsPath + "/" + programPathKey, programPathValue).toString())
            , arguments(settings.value(settingsPath + "/" + argumentsKey, argumentsValue).toString()) {
        }

        void save(QSettings& settings, const QString& settingsPath) const {
            settings.setValue(settingsPath + "/" + titleKey, title);
            settings.setValue(settingsPath + "/" + buttonLabelKey, buttonLabel);
            settings.setValue(settingsPath + "/" + programPathKey, programPath);
            settings.setValue(settingsPath + "/" + argumentsKey, arguments);

            PluginSettings::save(settings, settingsPath);
        }

        QString title;
        QString buttonLabel;
        QString programPath;
        QString arguments;
    };

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    ~SettingsDialog();

    QJTAGSettings dataSettings() const;

    QString settingsPath() const;

private slots:
    void ok();

    void apply();
    
    void cancel();

    void chooseFile();

private:
    void updateSettings();
    
    void fillFromSettings();

    bool verifySettings() const;

private:
    QJTAGSettings m_currentSettings;
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
    QString m_settingsPath;
};

#endif