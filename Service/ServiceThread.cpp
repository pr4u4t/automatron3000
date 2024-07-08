#include "ServiceThread.h"

ServiceThread::ServiceThread(QObject* parent) 
: QThread(parent){}

void ServiceThread::run() {
	
	exec();
}