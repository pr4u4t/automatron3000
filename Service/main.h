#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>
#include <QDir>

#include <winsock2.h>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "../api/api.h"
#include "ServiceManager.h"
#include "CmdHandler.h"

class Main : public QCoreApplication {

    Q_OBJECT

public:
	
    static constexpr const char* AutoUpdateKey = "AutoUpdate";
    static constexpr const bool AutoUpdateValue = false;

    Main(int& argc, char** argv);

    int exec();

    QSettings& settings() {
        return m_settings;
    }

	static bool isElevated() {
        bool ret = FALSE;
        HANDLE hToken = NULL;

        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            TOKEN_ELEVATION Elevation;
            DWORD cbSize = sizeof(TOKEN_ELEVATION);
            if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
                ret = Elevation.TokenIsElevated ? true : false;
            }
        }
        if (hToken) {
            CloseHandle(hToken);
        }

        return ret;
    }

    static bool isSeparateConsole() {
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            //printf("GetConsoleScreenBufferInfo failed: %lu\n", GetLastError());
            return false;
        }

        // if cursor position is (0,0) then we were launched in a separate console
        return ((!csbi.dwCursorPosition.X) && (!csbi.dwCursorPosition.Y)) ? true : false;
    }

    static QString dataPath() {
        wchar_t* dirp = nullptr;
        HRESULT res = SHGetKnownFolderPath(
            FOLDERID_ProgramData,
            0,
            NULL,
            &dirp
        );
        QString ret = QString::fromWCharArray(dirp);
        ret += "\\sysinfovault\\";
        CoTaskMemFree(dirp);
        return ret;
    }

    static bool createDataDir() {
        QString dir = dataPath();
        
        if (QDir(dir).exists() == true) {
            return true;
        }

        return QDir().mkdir(dir);
    }

signals:

    void message(const QString& message, LoggerSeverity severity = LoggerSeverity::LOG_INFO);

protected:

    bool update() const;

private:

    QSettings m_settings;
};

#endif
