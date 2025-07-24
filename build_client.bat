@echo off
echo ========================================
echo 编译虚拟HID客户端程序
echo ========================================
echo.

REM 检查是否在Visual Studio命令提示符中运行
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到C++编译器!
    echo 请在Visual Studio开发人员命令提示符中运行此脚本
    echo 或者运行以下命令设置环境:
    echo   "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo.
    pause
    exit /b 1
)

echo 正在编译用户态客户端程序...
echo.

REM 编译用户态应用程序
cl /EHsc /std:c++17 UserModeApp\VirtualHIDClient.cpp /Fe:VirtualHIDClient.exe

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo 编译成功!
    echo ========================================
    echo 输出文件: VirtualHIDClient.exe
    echo.
    echo 使用说明:
    echo 1. 确保虚拟HID驱动程序已安装
    echo 2. 以管理员权限运行: VirtualHIDClient.exe
    echo.
) else (
    echo.
    echo ========================================
    echo 编译失败!
    echo ========================================
    echo 请检查源代码和编译环境设置
    echo.
)

pause 