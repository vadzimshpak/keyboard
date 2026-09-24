@echo off
echo ========================================
echo Virtual HID Driver Installation Script
echo ========================================
echo.

REM Check administrator privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Administrator privileges are required!
    echo Please run this script as an administrator.
    echo.
    pause
    exit /b 1
)

echo Checking driver files...

REM Check whether the driver files exist
if not exist "VirtualHIDDriver\VirtualHIDDriver.sys" (
    echo Error: Driver file VirtualHIDDriver.sys not found
    echo Please build the driver first.
    echo.
    pause
    exit /b 1
)

if not exist "VirtualHIDDriver\VirtualHIDDriver.inf" (
    echo Error: INF file VirtualHIDDriver.inf not found
    echo.
    pause
    exit /b 1
)

echo Driver files found.
echo.

echo Step 1: Enable test signing mode...
bcdedit /set testsigning on
if %errorlevel% neq 0 (
    echo Warning: Failed to enable test signing mode
    echo You may need to restart and rerun this script
)
echo.

echo Step 2: Install the driver...
cd VirtualHIDDriver
pnputil /add-driver VirtualHIDDriver.inf /install
if %errorlevel% neq 0 (
    echo Error: Driver installation failed
    cd ..
    pause
    exit /b 1
)
cd ..
echo.

echo Step 3: Create the device instance...
devcon install VirtualHIDDriver\VirtualHIDDriver.inf Root\VirtualHIDDriver
if %errorlevel% neq 0 (
    echo Warning: Failed to create the device instance with devcon
    echo If devcon is not installed, you can install it manually through Device Manager
)
echo.

echo ========================================
echo Installation complete!
echo ========================================
echo.
echo Next steps:
echo 1. Restart the computer to apply the test signing mode change (if enabled just now)
echo 2. Run VirtualHIDClient.exe to test the driver
echo.
echo Notes:
echo - This driver is intended for learning and testing only
echo - Production environments require a digital signature
echo - If you encounter issues, check the error information in Event Viewer
echo.
pause 