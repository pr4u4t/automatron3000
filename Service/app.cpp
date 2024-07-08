#include "main.h"

#include <iostream>
int main(int argc, char** argv) {

	if (Main::isElevated() == false) {
		qCritical() << "Admin privileges are needed to run this program";
		return -1;
	}
	
	if (Main::createDataDir() == false) {
		qCritical() << "Failed to create data directory: " << Main::dataPath();
	}
	
	Logger log(Main::dataPath()+"\\sysinfovault.log", nullptr/*, severity*/);
	Main main(argc, argv);
	QObject::connect(&main, &Main::message, &log, &Logger::message);
	
	return main.exec();
}