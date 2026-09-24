#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>

// Definitions copied from the driver header file
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
        // Symbolic link path for the driver (must be created in the driver)
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
        std::wcout << L"Connecting to the virtual HID driver..." << std::endl;

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
            std::wcout << L"Unable to open the device! Error code: " << error << std::endl;
            
            switch (error)
            {
            case ERROR_FILE_NOT_FOUND:
                std::wcout << L"Error: Device file not found." << std::endl;
                std::wcout << L"Please make sure the driver is installed correctly and the symbolic link has been created." << std::endl;
                break;
            case ERROR_ACCESS_DENIED:
                std::wcout << L"Error: Access denied." << std::endl;
                std::wcout << L"Please run this program with administrator privileges." << std::endl;
                break;
            default:
                std::wcout << L"Please make sure the driver is installed correctly and the device symbolic link has been created." << std::endl;
                break;
            }
            return false;
        }

        std::wcout << L"Successfully connected to the virtual HID device!" << std::endl;
        return true;
    }

    bool SendKeyboardInput(BYTE virtualKey, bool keyDown)
    {
        if (m_deviceHandle == INVALID_HANDLE_VALUE)
        {
            std::wcout << L"Device is not initialized!" << std::endl;
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
            std::wcout << L"Failed to send keyboard input! Error code: " << error << std::endl;
            return false;
        }

        std::wcout << L"Keyboard input sent: VK=" << (int)virtualKey 
                   << L", KeyDown=" << (keyDown ? L"True" : L"False") << std::endl;
        return true;
    }

    bool SendMouseInput(LONG deltaX, LONG deltaY, BYTE buttonFlags, SHORT wheelDelta)
    {
        if (m_deviceHandle == INVALID_HANDLE_VALUE)
        {
            std::wcout << L"Device is not initialized!" << std::endl;
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
            std::wcout << L"Failed to send mouse input! Error code: " << error << std::endl;
            return false;
        }

        std::wcout << L"Mouse input sent: DX=" << deltaX << L", DY=" << deltaY 
                   << L", Buttons=0x" << std::hex << (int)buttonFlags 
                   << L", Wheel=" << std::dec << wheelDelta << std::endl;
        return true;
    }

    void SimulateKeyPress(BYTE virtualKey, DWORD holdTime = 100)
    {
        // Press the key
        if (SendKeyboardInput(virtualKey, true))
        {
            // Wait for the specified time
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
            
            // Release the key
            SendKeyboardInput(virtualKey, false);
        }
    }

    void SimulateMouseClick(BYTE button, DWORD holdTime = 100)
    {
        // Press the mouse button
        if (SendMouseInput(0, 0, button, 0))
        {
            // Wait for the specified time
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
            
            // Release the mouse button
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
    std::wcout << L"\n========== Virtual HID Driver Test Program ==========" << std::endl;
    std::wcout << L"1. Send keyboard input (A key)\n";
    std::wcout << L"2. Send mouse movement\n";
    std::wcout << L"3. Send mouse click\n";
    std::wcout << L"4. Simulate input 'Hello'\n";
    std::wcout << L"5. Mouse wheel test\n";
    std::wcout << L"6. Number key test (0-9)\n";
    std::wcout << L"7. Special key test (Space, Enter, Escape)\n";
    std::wcout << L"0. Exit\n";
    std::wcout << L"========================================\n";
    std::wcout << L"Select an operation: ";
}

void PrintStatus(const VirtualHIDClient& client)
{
    std::wcout << L"Status: " << (client.IsConnected() ? L"Connected" : L"Disconnected") << std::endl;
}

int main()
{
    // Enable console support for Unicode/UTF-8
    SetConsoleOutputCP(CP_UTF8);
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale(""));
    
    std::wcout << L"Virtual HID driver client program started\n" << std::endl;

    VirtualHIDClient client;

    if (!client.Initialize())
    {
        std::wcout << L"\nInitialization failed, exiting the program." << std::endl;
        std::wcout << L"Press any key to continue..." << std::endl;
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
            // Clear the error state
            std::wcin.clear();
            std::wcin.ignore(10000, L'\n');
            std::wcout << L"Invalid input. Please enter a number." << std::endl;
            continue;
        }

        switch (choice)
        {
        case 1:
            {
                std::wcout << L"Press and release the A key..." << std::endl;
                client.SimulateKeyPress('A', 200);
            }
            break;

        case 2:
            {
                std::wcout << L"Send mouse movement (10, 10)..." << std::endl;
                client.SimulateMouseMove(10, 10);
            }
            break;

        case 3:
            {
                std::wcout << L"Send a left mouse click..." << std::endl;
                client.SimulateMouseClick(0x01, 100); // Left button
            }
            break;

        case 4:
            {
                std::wcout << L"Simulate input 'Hello'..." << std::endl;
                // Simple character input simulation
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
                std::wcout << L"Scroll the mouse wheel up..." << std::endl;
                client.SendMouseInput(0, 0, 0, 120); // Scroll up
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::wcout << L"Scroll the mouse wheel down..." << std::endl;
                client.SendMouseInput(0, 0, 0, -120); // Scroll down
            }
            break;

        case 6:
            {
                std::wcout << L"Testing number keys 0-9..." << std::endl;
                char digits[] = "0123456789";
                for (int i = 0; digits[i] != '\0'; i++)
                {
                    std::wcout << L"Press number key: " << (char)digits[i] << std::endl;
                    client.SimulateKeyPress(digits[i], 100);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }
            break;

        case 7:
            {
                std::wcout << L"Testing special keys..." << std::endl;
                
                std::wcout << L"Press the Space key..." << std::endl;
                client.SimulateKeyPress(VK_SPACE, 200);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                
                std::wcout << L"Press the Enter key..." << std::endl;
                client.SimulateKeyPress(VK_RETURN, 200);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                
                std::wcout << L"Press the ESC key..." << std::endl;
                client.SimulateKeyPress(VK_ESCAPE, 200);
            }
            break;

        case 0:
            std::wcout << L"Exiting the program..." << std::endl;
            break;

        default:
            std::wcout << L"Invalid selection, please enter again." << std::endl;
            break;
        }

        if (choice != 0)
        {
            std::wcout << L"\nOperation complete. Press Enter to continue..." << std::endl;
            std::wcin.ignore();
            std::wcin.get();
        }
    }

    return 0;
} 