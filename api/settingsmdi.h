#ifndef SETTINGS_MDI_H
#define SETTINGS_MDI_H

#include "api_global.h"
#include "mdichild.h"

struct PluginSettings;

class API_EXPORT SettingsMdi : public MdiChild {
	
	Q_OBJECT

signals:

	void settingsUpdated();

public:

	SettingsMdi(QWidget* parent, PluginSettings* setts, const QString& settingsPath) 
	: MdiChild(parent)
	, m_currentSettings(setts)
	, m_settingsPath(settingsPath){
		setAttribute(Qt::WA_DeleteOnClose);
	}

	virtual ~SettingsMdi() {
		if (m_currentSettings) {
			delete m_currentSettings;
		}
	}

	template<typename T>
	T* settings() const {
		return dynamic_cast<T*>(m_currentSettings);
	}

	template<>
	PluginSettings* settings() const {
		return m_currentSettings;
	}

	QString settingsPath() const {
		return m_settingsPath;
	}

private slots:
	virtual void ok() = 0;

	virtual void apply() = 0;
	
	virtual void cancel() = 0;

private:

	PluginSettings* m_currentSettings = nullptr;
	QString m_settingsPath;
};

#endif
