#include "ServiceManager.h"
#include "ServiceThread.h"
#include <QCoreApplication>
#include <QDebug>

SERVICE_STATUS g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;
ServiceManager* ServiceManager::m_instance = nullptr;

VOID WINAPI Service_main(int argc, LPTSTR* argv) {
    DWORD Status = E_FAIL;

    // Register our service control handler with the SCM
    g_StatusHandle = RegisterServiceCtrlHandler(ServiceManager::instance()->name().toLocal8Bit().data(), Service_ctrlHandler);

    if (g_StatusHandle == NULL) {
        return;
    }

    // Tell the service controller we are starting
    ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        qCritical() << "SetServiceStatus returned error";
    }

    /*
     * Perform tasks necessary to start the service here
     */

     // Create a service stop event to wait on later
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL) {
        // Error creating event
        // Tell service controller we are stopped and exit
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
            qCritical() << "SetServiceStatus returned error";
        }

        return;
    }

    // Tell the service controller we are started
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        qCritical() << "SetServiceStatus returned error";
    }

    //QThread* th = QThread::create<DWORD WINAPI(LPVOID), LPVOID>(Service_workerThread, nullptr);
    ServiceThread* th = new ServiceThread();
    th->start();

    // Wait until our worker thread exits signaling that the service needs to stop
    //WaitForSingleObject(hThread, INFINITE);
    th->wait();
    /*
     * Perform any cleanup tasks
     */

    CloseHandle(g_ServiceStopEvent);

    // Tell the service controller we are stopped
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        qCritical() << "SetServiceStatus returned error";
    }

}

VOID WINAPI Service_ctrlHandler(DWORD ctrlCode) {
    switch (ctrlCode) {
    case SERVICE_CONTROL_STOP:

        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        /*
        * Perform tasks necessary to stop the service here
        */

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 4;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
            qCritical() << "ServiceCtrlHandler: SetServiceStatus returned error";
        }

        // This will signal the worker thread to start shutting down
        SetEvent(g_ServiceStopEvent);

        break;

    default:
        break;
    }
}

DWORD WINAPI Service_workerThread(LPVOID lpParam) {
    for (;WaitForSingleObject(g_ServiceStopEvent, 250) != WAIT_OBJECT_0;) {
    
        
    }

    return 0;
}

//---------------------------------------------------------------------------

ServiceManager::ServiceManager(const QString& serviceName) 
: m_serviceName(serviceName){}

bool ServiceManager::exists() const  {
    bool result = true;
    SC_HANDLE manager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, GENERIC_READ);

    if (manager == NULL) {
        return false;
    }

    SC_HANDLE service = OpenServiceA(manager, m_serviceName.toLocal8Bit().data(), GENERIC_READ);

    if (service == NULL) {
        DWORD error = GetLastError();

        switch (error) {
        case ERROR_ACCESS_DENIED:
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_NAME:
        case ERROR_SERVICE_DOES_NOT_EXIST:
            result = false;
        }
    }

    if (service != nullptr) {
        CloseServiceHandle(service);
    }

    if (manager != nullptr) {
        CloseServiceHandle(manager);
    }

    return result;
}

bool ServiceManager::create() const  {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szUnquotedPath[MAX_PATH];

    //if (!GetModuleFileName(NULL, szUnquotedPath, MAX_PATH)) {
    //    qCritical() << "Cannot install service " << GetLastError();
    //    return false;
    //}

    // In case the path contains a space, it must be quoted so that
    // it is correctly interpreted. For example,
    // "d:\my share\myservice.exe" should be specified as
    // ""d:\my share\myservice.exe"".
    //TCHAR szPath[MAX_PATH];
    //StringCbPrintf(szPath, MAX_PATH, TEXT("\"%s\""), szUnquotedPath);

    QString path = QCoreApplication::applicationFilePath();

    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager) {
        qCritical() << "OpenSCManager failed: " << GetLastError();
        return false;
    }

    // Create the service

    schService = CreateServiceA(
        schSCManager,                       // SCM database 
        m_serviceName.toLocal8Bit().data(), // name of service 
        m_serviceName.toLocal8Bit().data(), // service name to display 
        SERVICE_ALL_ACCESS,                 // desired access 
        SERVICE_WIN32_OWN_PROCESS,          // service type 
        SERVICE_AUTO_START,                 // start type 
        SERVICE_ERROR_NORMAL,               // error control type 
        path.toLocal8Bit().data(),          // path to service's binary 
        NULL,                               // no load ordering group 
        NULL,                               // no tag identifier 
        NULL,                               // no dependencies 
        NULL,                               // LocalSystem account 
        NULL                                // no password 
    );

    if (schService == NULL) {
        qCritical() << "CreateService: " << m_serviceName << " failed :" << GetLastError();
        CloseServiceHandle(schSCManager);
        return false;
    }

    qInfo() << "Service: " << m_serviceName << " installed successfully";
    

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return true;
}

bool ServiceManager::start() const {
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // servicesActive database 
        SC_MANAGER_ALL_ACCESS    // full access rights 
    );

    if (NULL == schSCManager){
        qCritical() << "OpenSCManager failed " << GetLastError();
        return false;
    }

    // Get a handle to the service.

    schService = OpenService(
        schSCManager,                       // SCM database 
        m_serviceName.toLocal8Bit().data(), // name of service 
        SERVICE_ALL_ACCESS                  // full access 
    );

    if (schService == NULL){
        qCritical() << "OpenService: " << m_serviceName << " failed " << GetLastError();
        CloseServiceHandle(schSCManager);
        return false;
    }

    // Check the status in case the service is not stopped. 

    if (!QueryServiceStatusEx(
        schService,                     // handle to service 
        SC_STATUS_PROCESS_INFO,         // information level
        (LPBYTE)&ssStatus,             // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded)) {              // size needed if buffer is too small
        qCritical() << "QueryServiceStatusEx failed " << GetLastError();
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);

        return false;
    }

    // Check if the service is already running. It would be possible 
    // to stop the service here, but for simplicity this example just returns. 

    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING){
        qCritical() << "Cannot start the service: " << m_serviceName << " because it is already running";
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    // Wait for the service to stop before attempting to start it.

    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth of the wait hint but not less than 1 second  
        // and not more than 10 seconds. 

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000) {
            dwWaitTime = 1000;
        } else if (dwWaitTime > 10000) {
            dwWaitTime = 10000;
        }

        Sleep(dwWaitTime);

        // Check the status until the service is no longer stop pending. 

        if (!QueryServiceStatusEx(
            schService,                     // handle to service 
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE)&ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))              // size needed if buffer is too small
        {
            qCritical() << "QueryServiceStatusEx failed " << GetLastError();
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return false;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint) {
            // Continue to wait and check.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        } else {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint) {
                qCritical() << "Timeout waiting for service to stop";
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return false;
            }
        }
    }

    // Attempt to start the service.

    if (!StartService(
        schService,  // handle to service 
        0,           // number of arguments 
        NULL)) {     // no arguments 
        qCritical() << "StartService failed: " << GetLastError();
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    } else {
        qInfo() << "Service start pending..";
    }
    // Check the status until the service is no longer start pending. 

    if (!QueryServiceStatusEx(
        schService,                     // handle to service 
        SC_STATUS_PROCESS_INFO,         // info level
        (LPBYTE)&ssStatus,             // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded))              // if buffer too small
    {
        qCritical() << "QueryServiceStatusEx failed: " << GetLastError();
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING){
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000) {
            dwWaitTime = 1000;
        } else if (dwWaitTime > 10000) {
            dwWaitTime = 10000;
        }

        Sleep(dwWaitTime);

        // Check the status again. 

        if (!QueryServiceStatusEx(
            schService,                         // handle to service 
            SC_STATUS_PROCESS_INFO,             // info level
            (LPBYTE)&ssStatus,                  // address of structure
            sizeof(SERVICE_STATUS_PROCESS),     // size of structure
            &dwBytesNeeded)) {                  // if buffer too small
                qCritical() << "QueryServiceStatusEx failed: " << GetLastError();
                break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint){
            // Continue to wait and check.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        } else{
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint) {
                // No progress made within the wait hint.
                break;
            }
        }
    }

    // Determine whether the service is running.

    if (ssStatus.dwCurrentState == SERVICE_RUNNING) { 

        qCritical() << "Service started successfully.";
    } else{
        qCritical() << "Service not started.";
        qCritical() << "  Current State: " << ssStatus.dwCurrentState;
        qCritical() << "  Exit Code: " << ssStatus.dwWin32ExitCode;
        qCritical() << "  Check Point: %d" << ssStatus.dwCheckPoint;
        qCritical() << "  Wait Hint: %d" << ssStatus.dwWaitHint;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}

bool ServiceManager::stop() const  {
    SC_HANDLE hSCManager = NULL;
    SC_HANDLE hService = NULL;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        qCritical() << "Open Service Manager failed: " << GetLastError();
        return false;
    }

    hService = OpenService(hSCManager, m_serviceName.toLocal8Bit().data(), DELETE | SERVICE_STOP);
    if (hService == NULL) {
        CloseServiceHandle(hSCManager);
        qCritical() << "Open service failed: " << GetLastError();
        return false;
    }

    // Stopping the service  
    g_ServiceStatus = { 0 };
    ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&g_ServiceStatus);

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return true;
}

bool ServiceManager::remove() const  {
    SC_HANDLE hSCManager = NULL;
    SC_HANDLE hService = NULL;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        qCritical() << "Open Service Manager failed: " << GetLastError();
        return false;
    }

    hService = OpenService(hSCManager, m_serviceName.toLocal8Bit().data(), DELETE | SERVICE_STOP);
    if (hService == NULL) {
        CloseServiceHandle(hSCManager);
        qCritical() << "Open service:" << m_serviceName << " failed: " << GetLastError();
        return false;
    }


    // Deleting the service
    if (DeleteService(hService) == false) {
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        qCritical() << "Delete service:" << m_serviceName << " failed:" << GetLastError();
        return false;
    }

    qInfo() << "Service:" << m_serviceName << " deleted";

    // Stopping the service  
    g_ServiceStatus = { 0 };
    ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&g_ServiceStatus);

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}

bool ServiceManager::isRunning() const  {
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;

    SC_HANDLE manager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (!manager) {
        return false;
    }

    SC_HANDLE service = OpenService(manager, m_serviceName.toLocal8Bit().data(), SERVICE_QUERY_STATUS);
    if (!service) {
        CloseServiceHandle(manager);
        return false;
    }

    auto result = QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
        reinterpret_cast<LPBYTE>(&ssStatus), sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded);

    CloseServiceHandle(service);
    CloseServiceHandle(manager);

    if (result == 0) {
        return false;
    }

    return ssStatus.dwCurrentState != SERVICE_STOPPED;
}

QString ServiceManager::name() const {
    return m_serviceName;
}
