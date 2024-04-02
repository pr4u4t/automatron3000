#ifndef PREFERENCE_PAGE_H
#define PREFERENCE_PAGE_H

#include <QWidget>
#include "Preferences.h"

class PreferencePage : public QWidget {
	Q_OBJECT

public:

	PreferencePage(PageLoader* ld) 
		: m_loader(ld){
	}

	~PreferencePage() = default;

	virtual void updateSettings() = 0;

	QString name() const {
		return m_loader->name();
	}

	QPixmap icon() const {
		return m_loader->icon();
	}

private:

	PageLoader* m_loader = nullptr;
};

#endif