@echo off
setlocal

:: 设置相关路径
set EXE_NAME=ChatApp.exe
set BIN_DIR=%~dp0..\bin
set QML_DIR=%~dp0..\qml

echo [1/3] Checking for executable...
if not exist "%BIN_DIR%\%EXE_NAME%" (
    echo Error: %EXE_NAME% not found in %BIN_DIR%. 
    echo Please build the project in Release mode first.
    pause
    exit /b 1
)

echo [2/3] Running windeployqt...
:: 假设 windeployqt 已在 PATH 中，或者用户需要手动指定路径
:: 例如: set QT_PATH=C:\Qt\5.15.2\msvc2019_64\bin
:: "%QT_PATH%\windeployqt.exe" --qmldir "%QML_DIR%" "%BIN_DIR%\%EXE_NAME%"

windeployqt --qmldir "%QML_DIR%" "%BIN_DIR%\%EXE_NAME%"

if %ERRORLEVEL% NEQ 0 (
    echo Error: windeployqt failed. Make sure Qt bin directory is in your PATH.
    pause
    exit /b 1
)

echo [3/3] Deployment files gathered in %BIN_DIR%
echo You can now zip the 'bin' folder for distribution.
pause
