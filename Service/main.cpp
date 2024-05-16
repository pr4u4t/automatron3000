#include "main.h"

Main::Main(int argc, char** argv)
    : QCoreApplication(argc, argv)
    , m_settings(dataPath() + "//configuration.ini", QSettings::IniFormat) {
}

bool Main::update() const {
    return true;
}

int Main::exec() {
    QStringList argc = arguments();
    
    if (argc.size() < 2) {
        if (settings().value(AutoUpdateKey, AutoUpdateValue).toBool() == true) {
            update();
        }

        SERVICE_TABLE_ENTRY ServiceTable[] = {
            {
                (LPWSTR)SERVICE_NAME, 
                (LPSERVICE_MAIN_FUNCTION) ServiceMain
            },
            {NULL, NULL}
        };

        if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
            int lastError = GetLastError();

            emit message(QString("ServiceStart: GetLastError(): %1").arg(lastError));
            return lastError;
        }
    } else {
        
        return QCoreApplication::exec();
    }
}