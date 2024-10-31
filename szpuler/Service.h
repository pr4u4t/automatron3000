#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include "../api/api.h"

enum class ServiceState {
	STOPPED,
	STARTING,
	RUNNING,
	STOPING,
	FAILED
};

class Service : public QObject {

	Q_OBJECT

public:
	Service(const ServiceState& state = ServiceState::STOPPED) 
		: m_state(state){}

	virtual bool start() = 0;

	virtual void stop() = 0;

	virtual bool restart() {
		stop();
		return start();
	}

	ServiceState state() const {
		return m_state;
	}

signals:

	void message(const QString& msg, LoggerSeverity severity = LoggerSeverity::LOG_NOTICE) const;

protected:

	void setState(const ServiceState& state) {
		m_state = state;
	}

private:

	ServiceState m_state = ServiceState::STOPPED;
};

#endif
