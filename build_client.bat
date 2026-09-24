@echo off
echo ========================================
echo Building the virtual HID client program
echo ========================================
echo.

REM Check whether it is running in the Visual Studio Developer Command Prompt
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: C++ compiler not found!
    echo Please run this script in the Visual Studio Developer Command Prompt
    echo or run the following command to set up the environment:
    echo   "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo.
    pause
    exit /b 1
)

echo Compiling the user-mode client program...
echo.

REM Compile the user-mode application
cl /EHsc /std:c++17 UserModeApp\VirtualHIDClient.cpp /Fe:VirtualHIDClient.exe

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo Build successful!
    echo ========================================
    echo Output file: VirtualHIDClient.exe
    echo.
    echo Usage:
    echo 1. Ensure the virtual HID driver is installed
    echo 2. Run as administrator: VirtualHIDClient.exe
    echo.
) else (
    echo.
    echo ========================================
    echo Build failed!
    echo ========================================
    echo Please check the source code and build environment settings
    echo.
)

pause 