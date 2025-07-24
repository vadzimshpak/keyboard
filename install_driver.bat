@echo off
echo ========================================
echo 虚拟HID驱动程序安装脚本
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

echo 正在检查驱动程序文件...

REM 检查驱动程序文件是否存在
if not exist "VirtualHIDDriver\VirtualHIDDriver.sys" (
    echo 错误: 找不到驱动程序文件 VirtualHIDDriver.sys
    echo 请先编译驱动程序。
    echo.
    pause
    exit /b 1
)

if not exist "VirtualHIDDriver\VirtualHIDDriver.inf" (
    echo 错误: 找不到INF文件 VirtualHIDDriver.inf
    echo.
    pause
    exit /b 1
)

echo 找到驱动程序文件。
echo.

echo 步骤 1: 启用测试签名模式...
bcdedit /set testsigning on
if %errorlevel% neq 0 (
    echo 警告: 无法启用测试签名模式
    echo 可能需要重启后重新运行此脚本
)
echo.

echo 步骤 2: 安装驱动程序...
cd VirtualHIDDriver
pnputil /add-driver VirtualHIDDriver.inf /install
if %errorlevel% neq 0 (
    echo 错误: 驱动程序安装失败
    cd ..
    pause
    exit /b 1
)
cd ..
echo.

echo 步骤 3: 创建设备实例...
devcon install VirtualHIDDriver\VirtualHIDDriver.inf Root\VirtualHIDDriver
if %errorlevel% neq 0 (
    echo 警告: 无法使用devcon创建设备实例
    echo 如果没有安装devcon，可以手动通过设备管理器安装
)
echo.

echo ========================================
echo 安装完成!
echo ========================================
echo.
echo 下一步:
echo 1. 重启计算机以使测试签名模式生效（如果刚启用）
echo 2. 运行 VirtualHIDClient.exe 测试驱动程序
echo.
echo 注意事项:
echo - 此驱动程序仅用于学习和测试目的
echo - 生产环境需要数字签名
echo - 如遇问题请查看事件查看器中的错误信息
echo.
pause 