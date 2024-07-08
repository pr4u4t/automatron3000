#ifndef SERVICE_THREAD_H
#define SERVICE_THREAD_H

#include <QThread>

class ServiceThread : public QThread {

	Q_OBJECT

public:

	ServiceThread(QObject *parent = nullptr);

	void run() override;

};

#endif