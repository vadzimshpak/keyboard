# Virtual HID Driver Project

This is an implementation of a virtual keyboard and mouse driver based on Windows KMDF (Kernel Mode Driver Framework) and VHF (Virtual HID Framework).

## 🆕 Latest updates

**Version 1.1 - Bug fix release**
- ✅ Fixed the WPP trace macro redefinition compile error
- ✅ Improved VHF device initialization and error handling
- ✅ Expanded keyboard mapping to support A-Z letters, 0-9 digits, and common keys
- ✅ Added complete device lifecycle management and resource cleanup
- ✅ Improved user-mode client error handling and interface experience
- ✅ Fixed INF file configuration issues
- ✅ Added automated install/uninstall scripts

## Project structure

```
keyboard/
├── VirtualHIDDriver/          # Kernel-mode driver
│   ├── Driver.cpp            # Driver entry point
│   ├── Driver.h              # Driver header file
│   ├── Device.cpp            # Device creation and VHF initialization
│   ├── Device.h              # Device-related definitions
│   ├── Trace.h               # WPP trace definitions
│   ├── VirtualHIDDriver.inf  # Driver installation file
│   └── VirtualHIDDriver.vcxproj # Visual Studio project file
├── UserModeApp/              # User-mode test application
│   └── VirtualHIDClient.cpp  # Test client program
├── install_driver.bat        # Automated installation script
├── uninstall_driver.bat      # Automated uninstall script
├── build_client.bat          # Client build script
├── VirtualHIDSolution.sln    # Visual Studio solution
└── README.md                 # Project documentation
```

## Feature set

- **Virtual keyboard device**: Supports A-Z letters, 0-9 digits, space, Enter, Esc, and other common keys
- **Virtual mouse device**: Supports mouse movement, left/middle/right button clicks, and wheel actions
- **KMDF framework**: Uses the modern Windows driver framework for stability and maintainability
- **VHF integration**: Uses Microsoft’s Virtual HID framework to ensure compatibility with the system
- **User-mode communication**: Communicates with user applications through the IOCTL interface
- **Standard HID reports**: Fully compliant HID report descriptor for USB HID specification
- **Resource management**: Complete device lifecycle management and automatic resource cleanup
- **Error handling**: Comprehensive error checks and status reporting

## Development environment requirements

### Required tools
- **Microsoft Visual Studio 2019/2022**
- **Windows Driver Kit (WDK) 10**
- **Windows 10 SDK**

### System requirements
- **Development system**: Windows 10/11 (64-bit)
- **Target system**: Windows 10/11 (supports the VHF framework)
- **Test environment**: A virtual machine is recommended for driver testing

## Quick start

### 🚀 Automated installation (recommended)

1. **Build the driver**:
   - Open `VirtualHIDSolution.sln`
   - Select the x64 platform and Debug/Release configuration
   - Press F7 to build the solution

2. **Build the client program**:
   ```cmd
   # Run from the project root
   build_client.bat
   ```

3. **Install the driver**:
   ```cmd
   # Run as administrator
   install_driver.bat
   ```

4. **Test the functionality**:
   ```cmd
   # Run as administrator
   VirtualHIDClient.exe
   ```

5. **Uninstall the driver** (optional):
   ```cmd
   # Run as administrator
   uninstall_driver.bat
   ```

### 📋 Manual installation

1. **Prepare the test environment**:
   ```cmd
   # Run as administrator
   bcdedit /set testsigning on
   # Restart the system
   shutdown /r /t 0
   ```

2. **Install the driver**:
   ```cmd
   # Method 1: Use pnputil (recommended)
   pnputil /add-driver VirtualHIDDriver\VirtualHIDDriver.inf /install
   
   # Method 2: Use Device Manager
   # 1. Open Device Manager
   # 2. Select "Action" > "Add legacy hardware"
   # 3. Select "Install the hardware that I manually select from a list"
   # 4. Select "Install from disk" and point to VirtualHIDDriver.inf
   ```

3. **Create a device instance**:
   ```cmd
   # Use the devcon tool to create a root-enumerated device
   devcon install VirtualHIDDriver\VirtualHIDDriver.inf Root\VirtualHIDDriver
   ```

## Usage instructions

### Test program features

The test program `VirtualHIDClient.exe` provides the following features:

1. **Keyboard input test**: Sends a single key press (for example, A)
2. **Mouse movement test**: Simulates mouse movement
3. **Mouse click test**: Simulates mouse button clicks
4. **Text input test**: Simulates entering the string "HELLO"
5. **Wheel test**: Simulates mouse wheel actions
6. **Number key test**: Tests 0-9 keys
7. **Special key test**: Tests Space, Enter, and Esc keys

### API usage example

```cpp
// Connect to the driver
VirtualHIDClient client;
if (!client.Initialize()) {
    // Handle connection failure
    return;
}

// Send keyboard input
client.SendKeyboardInput('A', true);   // Press A
client.SendKeyboardInput('A', false);  // Release A

// Send mouse input
client.SendMouseInput(10, 10, 0, 0);   // Move the mouse
client.SendMouseInput(0, 0, 0x01, 0);  // Press left button
client.SendMouseInput(0, 0, 0, 0);     // Release all buttons

// Check connection status
if (client.IsConnected()) {
    // Device connected
}
```

## Architecture design

### Driver architecture

```
┌─────────────────┐
│ User-mode app   │
└─────────┬───────┘
          │ IOCTL
┌─────────▼───────┐
│ Virtual HID      │
│ driver          │
├─────────────────┤
│ • IOCTL handling│
│ • Input convert │
│ • VHF interface │
│ • Resource mgmt │
└─────────┬───────┘
          │ VHF API
┌─────────▼───────┐
│ Windows VHF     │
│ (Vhf.sys)       │
└─────────┬───────┘
          │ HID protocol
┌─────────▼───────┐
│ HID class       │
│ driver          │
│ (HidClass.sys)  │
└─────────┬───────┘
          │ Standard input
┌─────────▼───────┐
│ Windows system  │
│ & applications  │
└─────────────────┘
```

### Core components

1. **Driver.cpp**: Driver entry point and lifecycle management
2. **Device.cpp**: Device creation, VHF initialization, and IOCTL handling
3. **HID report descriptor**: Standard keyboard and mouse HID implementation
4. **User-mode client**: Example implementation of IOCTL communication and input simulation
5. **Resource management**: Automatic device cleanup and error recovery

### Communication mechanism

- **IOCTL interface**: Communication between user mode and kernel mode
- **VHF framework**: Integration between kernel mode and the Windows HID subsystem
- **Symbolic link**: Provides the device path for user-mode access (`\\.\VirtualHIDDevice`)

## Debugging and troubleshooting

### 1. Enable the driver verifier

```cmd
verifier /flags 0x209BB /driver VirtualHIDDriver.sys
```

### 2. WPP tracing

The driver includes WPP tracing support, and the log can be viewed with the following tools:
- WPA (Windows Performance Analyzer)
- TraceView
- Custom trace sessions

### 3. Common issues

**Issue**: The user-mode program cannot connect to the driver
**Solution**:
- Confirm the driver is installed correctly
- Check whether the symbolic link was created successfully
- Confirm that it is running with administrator privileges
- Reinstall using `install_driver.bat`

**Issue**: The driver fails to load
**Solution**:
- Check whether test signing mode is enabled on the system
- Confirm WDK version compatibility
- Check the error information in Event Viewer
- Enable test signing with `bcdedit /set testsigning on`

**Issue**: Input is not recognized by the system
**Solution**:
- Confirm that the VHF device was created successfully
- Check the HID report descriptor format
- Verify the input report data format
- Review the driver trace logs

**Issue**: Build errors
**Solution**:
- Ensure the correct WDK and SDK versions are installed
- Check the project configuration and platform settings
- Verify that all dependencies are linked correctly


### 4. 日志查看

- **事件查看器**: Windows日志 → 系统
- **设备管理器**: 查看设备状态和错误代码
- **DebugView**: 查看内核调试输出

## 🔧 已修复的Bug

### 编译问题
- ✅ **WPP跟踪宏重复定义**: 移除了Driver.h中的重复跟踪宏定义
- ✅ **头文件包含错误**: 修复了头文件依赖关系

### 运行时问题
- ✅ **VHF设备初始化失败**: 改进了错误处理和资源清理逻辑
- ✅ **内存泄漏**: 添加了完整的设备上下文清理函数
- ✅ **键盘映射不完整**: 扩展支持A-Z、0-9和常用特殊按键
- ✅ **鼠标数据范围错误**: 添加了输入值范围检查和限制

### 配置问题
- ✅ **INF文件配置错误**: 修正了设备类别和VHF过滤器配置
- ✅ **符号链接创建失败**: 改进了符号链接创建的错误处理

### 用户体验问题
- ✅ **错误信息不明确**: 添加了详细的错误代码解释
- ✅ **中文显示问题**: 修复了控制台中文编码问题
- ✅ **输入验证缺失**: 添加了用户输入验证和错误恢复

## 安全考虑

### 开发和测试
- 仅在隔离的测试环境中使用
- 定期备份测试系统
- 使用虚拟机进行开发和调试

### 生产部署
- 必须获得微软数字签名
- 遵循最小权限原则
- 实施适当的访问控制

## 合规和法律

本项目仅用于**教育和合法研究目的**：
- 学习Windows驱动程序开发
- 理解HID设备工作原理
- 开发辅助功能工具
- 研究输入设备技术

**禁止用于**：
- 游戏作弊或自动化
- 规避安全系统
- 恶意软件开发
- 其他非法用途

## 技术参考

- [Windows Driver Kit (WDK)](https://docs.microsoft.com/en-us/windows-hardware/drivers/)
- [Virtual HID Framework (VHF)](https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/virtual-hid-framework--vhf-)
- [USB HID Usage Tables](https://www.usb.org/document-library/hid-usage-tables-122)
- [KMDF Documentation](https://docs.microsoft.com/en-us/windows-hardware/drivers/wdf/)

## 许可证

本项目仅供学习和研究使用。使用者需要遵守当地法律法规。

## 贡献

欢迎提交问题报告和改进建议。请确保所有贡献都符合教育和合法使用的目的。

---

**版本历史**:
- v1.1: Bug修复版本，改进稳定性和易用性
- v1.0: 初始版本，基本功能实现 