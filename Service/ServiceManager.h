#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <QtGlobal>

#include <Winsock2.h>
#include <windows.h>

#define SERVICE_NAME TEXT("sysinfovault")

VOID WINAPI Service_Main(int argc, LPTSTR* argv);
VOID WINAPI Service_CtrlHandler(DWORD ctrlCode);
DWORD WINAPI Service_WorkerThread(LPVOID lpParam);

class ServiceManager {
public:
    ServiceManager();

    bool exists();
    bool create();
    bool start();
    bool stop();
    bool remove();
    bool isRunning();
};

#endif
