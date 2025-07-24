# 虚拟HID驱动程序项目

这是一个基于 Windows KMDF (Kernel Mode Driver Framework) 和 VHF (Virtual HID Framework) 的虚拟键盘鼠标驱动程序实现。

## 🆕 最新更新

**版本 1.1 - Bug修复版本**
- ✅ 修复了WPP跟踪宏重复定义的编译错误
- ✅ 改进了VHF设备初始化和错误处理
- ✅ 扩展了键盘映射，支持A-Z字母、0-9数字和常用按键
- ✅ 添加了完整的设备生命周期管理和资源清理
- ✅ 改进了用户态客户端的错误处理和界面体验
- ✅ 修复了INF文件配置问题
- ✅ 添加了自动化安装/卸载脚本

## 项目结构

```
keyboard/
├── VirtualHIDDriver/          # 内核模式驱动程序
│   ├── Driver.cpp            # 驱动主入口点
│   ├── Driver.h              # 驱动头文件
│   ├── Device.cpp            # 设备创建和VHF初始化
│   ├── Device.h              # 设备相关定义
│   ├── Trace.h               # WPP跟踪定义
│   ├── VirtualHIDDriver.inf  # 驱动安装文件
│   └── VirtualHIDDriver.vcxproj # Visual Studio项目文件
├── UserModeApp/              # 用户态测试应用程序
│   └── VirtualHIDClient.cpp  # 测试客户端程序
├── install_driver.bat        # 自动安装脚本
├── uninstall_driver.bat      # 自动卸载脚本
├── build_client.bat          # 客户端编译脚本
├── VirtualHIDSolution.sln    # Visual Studio解决方案
└── README.md                 # 项目说明文档
```

## 功能特性

- **虚拟键盘设备**: 支持A-Z字母、0-9数字、空格、回车、ESC等常用按键
- **虚拟鼠标设备**: 支持鼠标移动、左中右键点击和滚轮操作
- **KMDF框架**: 使用现代Windows驱动框架，提供稳定性和可维护性
- **VHF集成**: 利用微软虚拟HID框架，确保与系统的兼容性
- **用户态通信**: 通过IOCTL接口与用户应用程序通信
- **标准HID报告**: 完全符合USB HID规范的报告描述符
- **资源管理**: 完整的设备生命周期管理和自动资源清理
- **错误处理**: 全面的错误检查和状态报告

## 开发环境要求

### 必需工具
- **Microsoft Visual Studio 2019/2022**
- **Windows Driver Kit (WDK) 10**
- **Windows 10 SDK**

### 系统要求
- **开发系统**: Windows 10/11 (64位)
- **目标系统**: Windows 10/11 (支持VHF框架)
- **测试环境**: 推荐使用虚拟机进行驱动程序测试

## 快速开始

### 🚀 自动化安装（推荐）

1. **编译驱动程序**：
   - 打开 `VirtualHIDSolution.sln`
   - 选择 x64 平台和 Debug/Release 配置
   - 按 F7 构建解决方案

2. **编译客户端程序**：
   ```cmd
   # 在项目根目录运行
   build_client.bat
   ```

3. **安装驱动程序**：
   ```cmd
   # 以管理员权限运行
   install_driver.bat
   ```

4. **测试功能**：
   ```cmd
   # 以管理员权限运行
   VirtualHIDClient.exe
   ```

5. **卸载驱动程序**（可选）：
   ```cmd
   # 以管理员权限运行
   uninstall_driver.bat
   ```

### 📋 手动安装

1. **准备测试环境**：
   ```cmd
   # 以管理员权限运行
   bcdedit /set testsigning on
   # 重启系统
   shutdown /r /t 0
   ```

2. **安装驱动程序**：
   ```cmd
   # 方法1: 使用pnputil (推荐)
   pnputil /add-driver VirtualHIDDriver\VirtualHIDDriver.inf /install
   
   # 方法2: 使用设备管理器
   # 1. 打开设备管理器
   # 2. 选择"操作" > "添加传统硬件"
   # 3. 选择"安装我手动从列表选择的硬件"
   # 4. 选择"从磁盘安装"，指向 VirtualHIDDriver.inf
   ```

3. **创建设备实例**：
   ```cmd
   # 使用devcon工具创建根枚举设备
   devcon install VirtualHIDDriver\VirtualHIDDriver.inf Root\VirtualHIDDriver
   ```

## 使用说明

### 测试程序功能

测试程序 `VirtualHIDClient.exe` 提供以下功能：

1. **键盘输入测试**: 发送单个按键 (如A键)
2. **鼠标移动测试**: 模拟鼠标移动
3. **鼠标点击测试**: 模拟鼠标按钮点击
4. **文本输入测试**: 模拟输入字符串 "HELLO"
5. **滚轮测试**: 模拟鼠标滚轮操作
6. **数字键测试**: 测试0-9数字键
7. **特殊键测试**: 测试空格、回车、ESC键

### API使用示例

```cpp
// 连接到驱动程序
VirtualHIDClient client;
if (!client.Initialize()) {
    // 处理连接失败
    return;
}

// 发送键盘输入
client.SendKeyboardInput('A', true);   // 按下A键
client.SendKeyboardInput('A', false);  // 释放A键

// 发送鼠标输入
client.SendMouseInput(10, 10, 0, 0);   // 移动鼠标
client.SendMouseInput(0, 0, 0x01, 0);  // 按下左键
client.SendMouseInput(0, 0, 0, 0);     // 释放所有按键

// 检查连接状态
if (client.IsConnected()) {
    // 设备已连接
}
```

## 架构设计

### 驱动程序架构

```
┌─────────────────┐
│  用户态应用程序  │
└─────────┬───────┘
          │ IOCTL
┌─────────▼───────┐
│  虚拟HID驱动    │
├─────────────────┤
│ • IOCTL处理     │
│ • 输入转换      │
│ • VHF接口       │
│ • 资源管理      │
└─────────┬───────┘
          │ VHF API
┌─────────▼───────┐
│ Windows VHF     │
│ (Vhf.sys)       │
└─────────┬───────┘
          │ HID协议
┌─────────▼───────┐
│ HID类驱动程序   │
│ (HidClass.sys)  │
└─────────┬───────┘
          │ 标准输入
┌─────────▼───────┐
│  Windows系统    │
│  & 应用程序     │
└─────────────────┘
```

### 核心组件

1. **Driver.cpp**: 驱动程序入口点和生命周期管理
2. **Device.cpp**: 设备创建、VHF初始化和IOCTL处理
3. **HID报告描述符**: 标准键盘和鼠标的HID规范实现
4. **用户态客户端**: IOCTL通信和输入模拟的示例实现
5. **资源管理**: 自动设备清理和错误恢复

### 通信机制

- **IOCTL接口**: 用户态与内核态通信
- **VHF框架**: 内核态与Windows HID子系统集成
- **符号链接**: 提供用户态访问设备的路径 (`\\.\VirtualHIDDevice`)

## 调试和故障排除

### 1. 启用驱动程序验证器

```cmd
verifier /flags 0x209BB /driver VirtualHIDDriver.sys
```

### 2. WPP跟踪

驱动程序包含WPP跟踪支持，可以使用以下工具查看日志：
- WPA (Windows Performance Analyzer)
- TraceView
- 自定义跟踪会话

### 3. 常见问题

**问题**: 用户态程序无法连接到驱动程序
**解决**: 
- 确认驱动程序已正确安装
- 检查符号链接是否创建成功
- 确认以管理员权限运行
- 运行 `install_driver.bat` 重新安装

**问题**: 驱动程序加载失败
**解决**: 
- 检查系统是否启用了测试签名模式
- 确认WDK版本兼容性
- 查看事件查看器中的错误信息
- 使用 `bcdedit /set testsigning on` 启用测试签名

**问题**: 输入没有被系统识别
**解决**: 
- 确认VHF设备已成功创建
- 检查HID报告描述符格式
- 验证输入报告的数据格式
- 查看驱动程序的跟踪日志

**问题**: 编译错误
**解决**:
- 确保安装了正确版本的WDK和SDK
- 检查项目配置和平台设置
- 验证所有依赖项是否正确链接

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