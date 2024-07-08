#include "main.h"

Main::Main(int& argc, char** argv)
    : QCoreApplication(argc, argv)
    , m_settings(dataPath() + "//configuration.ini", QSettings::IniFormat) {
}

bool Main::update() const {
    return true;
}

int Main::exec() {
    QStringList args = arguments();
    ServiceManager* manager = ServiceManager::instance("sysinfovault");

    if (args.size() < 2) {
        if (settings().value(AutoUpdateKey, AutoUpdateValue).toBool() == true) {
            update();
        }

        SERVICE_TABLE_ENTRY ServiceTable[] = {
            {
                manager->name().toLocal8Bit().data(),
                (LPSERVICE_MAIN_FUNCTION) Service_main
            },
            {NULL, NULL}
        };

        if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
            int lastError = GetLastError();

            emit message(QString("ServiceStart: GetLastError(): %1").arg(lastError));
            return lastError;
        }
    } 
    
    CmdHandler cmd;

    return cmd.exec(args);
}