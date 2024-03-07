#ifndef SETTINGS_MDI_H
#define SETTINGS_MDI_H

#include "api_global.h"
#include "mdichild.h"

class API_EXPORT SettingsMdi : public MdiChild {
	
	Q_OBJECT

signals:

	void settingsUpdated();

public:

	SettingsMdi(QWidget* parent) 
	: MdiChild(parent){
		setAttribute(Qt::WA_DeleteOnClose);
	}

private slots:
	virtual void ok() = 0;

	virtual void apply() = 0;
	
	virtual void cancel() = 0;
};

#endif
