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

struct WebSettings : public PluginSettings {
    static constexpr const char* urlKey = "url";
    static constexpr const char* urlValue = nullptr;
    static constexpr const char* refreshKey = "refresh";
    static constexpr qint32 refreshValue = -1;

    Q_GADGET

    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(qint32 refresh READ refresh WRITE setRefresh)

public:
    WebSettings()
        : m_url(urlValue)
        , m_refresh(refreshValue) {
        registerMetaObject(&staticMetaObject);
    }

    virtual ~WebSettings() = default;

    WebSettings(const QSettings& settings, const QString& settingsPath)
        : PluginSettings(settings, settingsPath)
        , m_url(settings.value(settingsPath + "/" + urlKey, urlValue).toString())
        , m_refresh(settings.value(settingsPath + "/" + refreshKey, refreshValue).toInt()) {
        registerMetaObject(&staticMetaObject);
    }

    void save(QSettings& settings, const QString& settingsPath) const {
        settings.setValue(settingsPath + "/" + urlKey, m_url);
        settings.setValue(settingsPath + "/" + refreshKey, m_refresh);

        PluginSettings::save(settings, settingsPath);
    }

    // Getter and Setter for url
    QString url() const { return m_url; }
    void setUrl(const QString& url) { m_url = url; }

    // Getter and Setter for refresh
    qint32 refresh() const { return m_refresh; }
    void setRefresh(qint32 refresh) { m_refresh = refresh; }

    bool operator==(const WebSettings& other) const {
        return m_url == other.m_url &&
            m_refresh == other.m_refresh;
    }

    bool operator!=(const WebSettings& other) const {
        return !(*this == other);
    }

private:
    QString m_url;
    qint32 m_refresh;
};

class QWeb;

class SettingsDialog : public SettingsMdi {

    Q_OBJECT

public:

    SettingsDialog(QWidget* parent, Loader* loader, const QString& settingsPath);

    SettingsDialog(QWidget* parent, const QWeb *web);

    ~SettingsDialog();

    operator WebSettings() const;

private slots:
    void ok();

    void apply();
    
    void cancel();

private:
    void updateSettings();
    
    void fillFromSettings();

    void setup();

private:
    Ui::SettingsDialog* m_ui = nullptr;
    QIntValidator* m_intValidator = nullptr;
};

#endif