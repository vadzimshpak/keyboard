@echo off
echo ========================================
echo 虚拟HID驱动程序卸载脚本
echo ========================================
echo.

REM 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 需要管理员权限!
    echo 请以管理员身份运行此脚本。
    echo.
    pause
    exit /b 1
)

echo 正在卸载虚拟HID驱动程序...
echo.

echo 步骤 1: 停止并删除设备实例...
devcon remove Root\VirtualHIDDriver
if %errorlevel% neq 0 (
    echo 警告: 无法使用devcon删除设备实例
)
echo.

echo 步骤 2: 卸载驱动程序包...
for /f "tokens=1" %%i in ('pnputil /enum-drivers ^| findstr /i "virtualhid"') do (
    if not "%%i"=="" (
        echo 找到驱动程序包: %%i
        pnputil /delete-driver %%i /uninstall /force
        if %errorlevel% equ 0 (
            echo 成功卸载驱动程序包: %%i
        ) else (
            echo 警告: 无法卸载驱动程序包: %%i
        )
    )
)
echo.

echo 步骤 3: 清理注册表项（可选）...
echo 注意: 手动清理可能需要重启系统才能完全生效
echo.

echo ========================================
echo 卸载完成!
echo ========================================
echo.
echo 后续步骤:
echo 1. 重启计算机以完全清理驱动程序
echo 2. 可以选择禁用测试签名模式: bcdedit /set testsigning off
echo.
echo 注意: 如果需要禁用测试签名模式，请运行:
echo bcdedit /set testsigning off
echo 然后重启系统。
echo.
pause 