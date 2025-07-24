#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>

// 从驱动程序头文件复制的定义
#define IOCTL_KEYBOARD_INPUT CTL_CODE(FILE_DEVICE_KEYBOARD, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUSE_INPUT    CTL_CODE(FILE_DEVICE_MOUSE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _KEYBOARD_INPUT_DATA
{
    BYTE VirtualKey;
    BOOL KeyDown;
} KEYBOARD_INPUT_DATA, *PKEYBOARD_INPUT_DATA;

typedef struct _MOUSE_INPUT_DATA
{
    LONG DeltaX;
    LONG DeltaY;
    BYTE ButtonFlags;
    SHORT WheelDelta;
} MOUSE_INPUT_DATA, *PMOUSE_INPUT_DATA;

class VirtualHIDClient
{
private:
    HANDLE m_deviceHandle;
    std::wstring m_devicePath;

public:
    VirtualHIDClient() : m_deviceHandle(INVALID_HANDLE_VALUE)
    {
        // 驱动程序的符号链接路径（需要在驱动中创建）
        m_devicePath = L"\\\\.\\VirtualHIDDevice";
    }

    ~VirtualHIDClient()
    {
        if (m_deviceHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_deviceHandle);
            m_deviceHandle = INVALID_HANDLE_VALUE;
        }
    }

    bool Initialize()
    {
        std::wcout << L"正在连接到虚拟HID驱动程序..." << std::endl;

        m_deviceHandle = CreateFileW(
            m_devicePath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (m_deviceHandle == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            std::wcout << L"无法打开设备！错误代码: " << error << std::endl;
            
            switch (error)
            {
            case ERROR_FILE_NOT_FOUND:
                std::wcout << L"错误: 找不到设备文件。" << std::endl;
                std::wcout << L"请确保驱动程序已正确安装并创建了符号链接。" << std::endl;
                break;
            case ERROR_ACCESS_DENIED:
                std::wcout << L"错误: 访问被拒绝。" << std::endl;
                std::wcout << L"请以管理员权限运行此程序。" << std::endl;
                break;
            default:
                std::wcout << L"请确保驱动程序已正确安装并且设备已创建符号链接。" << std::endl;
                break;
            }
            return false;
        }

        std::wcout << L"成功连接到虚拟HID设备！" << std::endl;
        return true;
    }

    bool SendKeyboardInput(BYTE virtualKey, bool keyDown)
    {
        if (m_deviceHandle == INVALID_HANDLE_VALUE)
        {
            std::wcout << L"设备未初始化！" << std::endl;
            return false;
        }

        KEYBOARD_INPUT_DATA keyboardData = { 0 };
        keyboardData.VirtualKey = virtualKey;
        keyboardData.KeyDown = keyDown ? TRUE : FALSE;

        DWORD bytesReturned = 0;
        BOOL result = DeviceIoControl(
            m_deviceHandle,
            IOCTL_KEYBOARD_INPUT,
            &keyboardData,
            sizeof(keyboardData),
            NULL,
            0,
            &bytesReturned,
            NULL
        );

        if (!result)
        {
            DWORD error = GetLastError();
            std::wcout << L"发送键盘输入失败！错误代码: " << error << std::endl;
            return false;
        }

        std::wcout << L"键盘输入已发送: VK=" << (int)virtualKey 
                   << L", KeyDown=" << (keyDown ? L"True" : L"False") << std::endl;
        return true;
    }

    bool SendMouseInput(LONG deltaX, LONG deltaY, BYTE buttonFlags, SHORT wheelDelta)
    {
        if (m_deviceHandle == INVALID_HANDLE_VALUE)
        {
            std::wcout << L"设备未初始化！" << std::endl;
            return false;
        }

        MOUSE_INPUT_DATA mouseData = { 0 };
        mouseData.DeltaX = deltaX;
        mouseData.DeltaY = deltaY;
        mouseData.ButtonFlags = buttonFlags;
        mouseData.WheelDelta = wheelDelta;

        DWORD bytesReturned = 0;
        BOOL result = DeviceIoControl(
            m_deviceHandle,
            IOCTL_MOUSE_INPUT,
            &mouseData,
            sizeof(mouseData),
            NULL,
            0,
            &bytesReturned,
            NULL
        );

        if (!result)
        {
            DWORD error = GetLastError();
            std::wcout << L"发送鼠标输入失败！错误代码: " << error << std::endl;
            return false;
        }

        std::wcout << L"鼠标输入已发送: DX=" << deltaX << L", DY=" << deltaY 
                   << L", Buttons=0x" << std::hex << (int)buttonFlags 
                   << L", Wheel=" << std::dec << wheelDelta << std::endl;
        return true;
    }

    void SimulateKeyPress(BYTE virtualKey, DWORD holdTime = 100)
    {
        // 按下键
        if (SendKeyboardInput(virtualKey, true))
        {
            // 等待指定时间
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
            
            // 释放键
            SendKeyboardInput(virtualKey, false);
        }
    }

    void SimulateMouseClick(BYTE button, DWORD holdTime = 100)
    {
        // 按下鼠标按钮
        if (SendMouseInput(0, 0, button, 0))
        {
            // 等待指定时间
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
            
            // 释放鼠标按钮
            SendMouseInput(0, 0, 0, 0);
        }
    }

    void SimulateMouseMove(LONG deltaX, LONG deltaY)
    {
        SendMouseInput(deltaX, deltaY, 0, 0);
    }

    bool IsConnected() const
    {
        return m_deviceHandle != INVALID_HANDLE_VALUE;
    }
};

void PrintMenu()
{
    std::wcout << L"\n========== 虚拟HID驱动测试程序 ==========\n";
    std::wcout << L"1. 发送键盘输入 (A键)\n";
    std::wcout << L"2. 发送鼠标移动\n";
    std::wcout << L"3. 发送鼠标点击\n";
    std::wcout << L"4. 模拟输入 'Hello'\n";
    std::wcout << L"5. 鼠标滚轮测试\n";
    std::wcout << L"6. 数字键测试 (0-9)\n";
    std::wcout << L"7. 特殊键测试 (Space, Enter, Escape)\n";
    std::wcout << L"0. 退出\n";
    std::wcout << L"========================================\n";
    std::wcout << L"请选择操作: ";
}

void PrintStatus(const VirtualHIDClient& client)
{
    std::wcout << L"状态: " << (client.IsConnected() ? L"已连接" : L"未连接") << std::endl;
}

int main()
{
    // 设置控制台支持中文
    SetConsoleOutputCP(CP_UTF8);
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale(""));
    
    std::wcout << L"虚拟HID驱动客户端程序启动\n" << std::endl;

    VirtualHIDClient client;

    if (!client.Initialize())
    {
        std::wcout << L"\n初始化失败，程序退出。" << std::endl;
        std::wcout << L"请按任意键继续..." << std::endl;
        std::wcin.get();
        return 1;
    }

    int choice = -1;
    while (choice != 0)
    {
        PrintStatus(client);
        PrintMenu();
        
        if (!(std::wcin >> choice))
        {
            // 清除错误状态
            std::wcin.clear();
            std::wcin.ignore(10000, L'\n');
            std::wcout << L"无效输入，请输入数字。" << std::endl;
            continue;
        }

        switch (choice)
        {
        case 1:
            {
                std::wcout << L"发送A键按下和释放..." << std::endl;
                client.SimulateKeyPress('A', 200);
            }
            break;

        case 2:
            {
                std::wcout << L"发送鼠标移动 (10, 10)..." << std::endl;
                client.SimulateMouseMove(10, 10);
            }
            break;

        case 3:
            {
                std::wcout << L"发送鼠标左键点击..." << std::endl;
                client.SimulateMouseClick(0x01, 100); // 左键
            }
            break;

        case 4:
            {
                std::wcout << L"模拟输入 'Hello'..." << std::endl;
                // 简单的字符输入模拟
                char text[] = "HELLO";
                for (int i = 0; text[i] != '\0'; i++)
                {
                    client.SimulateKeyPress(text[i], 100);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            break;

        case 5:
            {
                std::wcout << L"发送鼠标滚轮向上..." << std::endl;
                client.SendMouseInput(0, 0, 0, 120); // 向上滚动
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::wcout << L"发送鼠标滚轮向下..." << std::endl;
                client.SendMouseInput(0, 0, 0, -120); // 向下滚动
            }
            break;

        case 6:
            {
                std::wcout << L"测试数字键 0-9..." << std::endl;
                char digits[] = "0123456789";
                for (int i = 0; digits[i] != '\0'; i++)
                {
                    std::wcout << L"按下数字键: " << (char)digits[i] << std::endl;
                    client.SimulateKeyPress(digits[i], 100);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }
            break;

        case 7:
            {
                std::wcout << L"测试特殊键..." << std::endl;
                
                std::wcout << L"按下空格键..." << std::endl;
                client.SimulateKeyPress(VK_SPACE, 200);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                
                std::wcout << L"按下回车键..." << std::endl;
                client.SimulateKeyPress(VK_RETURN, 200);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                
                std::wcout << L"按下ESC键..." << std::endl;
                client.SimulateKeyPress(VK_ESCAPE, 200);
            }
            break;

        case 0:
            std::wcout << L"退出程序..." << std::endl;
            break;

        default:
            std::wcout << L"无效的选择，请重新输入。" << std::endl;
            break;
        }

        if (choice != 0)
        {
            std::wcout << L"\n操作完成。按Enter继续..." << std::endl;
            std::wcin.ignore();
            std::wcin.get();
        }
    }

    return 0;
} 