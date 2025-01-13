@echo off
REM Check if the directory argument is provided
if "%~1" == "" (
    goto :FAIL
)

if "%~2" == "" (
    goto :FAIL
)

if "%~3" == "" (
    goto :FAIL
)

goto :EXEC

:FAIL
echo Usage: %~nx0 ^<directory^> ^<path_to_windeployqt.exe^>
exit /b 1

:EXEC

REM Loop through all DLL files in the directory
for %%f in ("%~1plugins\*.dll") do (
    echo "%~2" --qmldir "%~1" --dir "%~1%~3" --debug "%%f"
    %~2 --qmldir %~1 --dir %~1%~3 --debug %%f
)

echo Done!
