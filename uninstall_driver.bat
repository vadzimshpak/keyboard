@echo off
echo ========================================
echo Virtual HID Driver Uninstall Script
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

echo Uninstalling the virtual HID driver...
echo.

echo Step 1: Stop and remove the device instance...
devcon remove Root\VirtualHIDDriver
if %errorlevel% neq 0 (
    echo Warning: Failed to remove the device instance using devcon
)
echo.

echo Step 2: Uninstall the driver package...
for /f "tokens=1" %%i in ('pnputil /enum-drivers ^| findstr /i "virtualhid"') do (
    if not "%%i"=="" (
        echo Found driver package: %%i
        pnputil /delete-driver %%i /uninstall /force
        if %errorlevel% equ 0 (
            echo Driver package removed successfully: %%i
        ) else (
            echo Warning: Failed to remove driver package: %%i
        )
    )
)
echo.

echo Step 3: Clean up registry entries (optional)...
echo Note: Manual cleanup may require a system restart to take full effect
echo.

echo ========================================
echo Uninstall complete!
echo ========================================
echo.
echo Next steps:
echo 1. Restart the computer to fully remove the driver
echo 2. Optionally disable test signing mode: bcdedit /set testsigning off
echo.
echo Note: If you need to disable test signing mode, run:
echo bcdedit /set testsigning off
echo then restart the system.
echo.
pause 