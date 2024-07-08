#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

//#define WIN32_LEAN_AND_MEAN
//#define _WINSOCKAPI_

#include <winsock2.h>
#include <windows.h>

#include <QtGlobal>
#include <QString>

VOID WINAPI Service_main(int argc, LPTSTR* argv);
VOID WINAPI Service_ctrlHandler(DWORD ctrlCode);
DWORD WINAPI Service_workerThread(LPVOID lpParam);

class ServiceManager {
public:
    ServiceManager() = delete;

    ServiceManager(const ServiceManager& other) = delete;

    bool exists() const;

    bool create() const;
    
    bool start() const;
    
    bool stop() const;
    
    bool remove() const;
    
    bool isRunning() const;

    QString name() const;

    static ServiceManager* instance(const QString& serviceName = QString()) {
        if (m_instance != nullptr) {
            return m_instance;
        }

        m_instance = new ServiceManager(serviceName);
        return m_instance;
    }

private:
    ServiceManager(const QString& serviceName);
    
    QString m_serviceName;
    static ServiceManager* m_instance;
};

#endif
