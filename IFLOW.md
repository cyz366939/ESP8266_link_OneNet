# ESP8266 OneNET MQTT 驱动项目

## 项目概述

这是一个基于 ESP8266 (ESP12E) 的 IoT 项目，实现了与 OneNET 云平台的 MQTT 通信功能。项目采用简洁模块化设计，支持通过串口接收数据并自动上传到 OneNET 平台，同时支持从 OneNET 平台接收控制指令。

### 主要功能

- **WiFi 连接管理**: 支持多 SSID 自动切换连接
- **MQTT 通信**: 与 OneNET 平台建立稳定连接，支持数据上报和指令接收
- **串口数据处理**: 通过串口接收键值对数据，自动转换为 JSON 格式上传
- **时间同步**: 集成 NTP 时间同步功能
- **模块化设计**: 将 MQTT 处理、串口处理、时间管理等功能分离为独立模块

### 技术栈

- **平台**: ESP8266 (ESP12E)
- **开发框架**: Arduino
- **构建工具**: PlatformIO
- **核心库**:
  - ESP8266WiFi / ESP8266WiFiMulti: WiFi 连接管理
  - PubSubClient: MQTT 协议实现
  - ArduinoJson: JSON 数据处理
  - NTPClient: 网络时间同步
  - ESP8266WebServer: Web 服务器（预留）
  - ESP8266HTTPClient: HTTP 客户端（预留）
  - ESP8266mDNS: mDNS 服务（预留）

## 项目结构

```
ESP8266_OneNET_Ariduno_简洁模块化_v2.0/
├── src/                    # 源代码目录
│   ├── main.cpp            # 主程序入口
│   ├── MqttHandler.cpp     # MQTT 处理实现
│   ├── SerialHandler.cpp   # 串口处理实现
│   └── Time_t.cpp          # 时间模块实现
├── include/                # 头文件目录
│   ├── config.h            # 配置文件（WiFi、MQTT 等参数）
│   ├── MqttHandler.h       # MQTT 处理头文件
│   ├── SerialHandler.h     # 串口处理头文件
│   └── Time_t.h            # 时间模块头文件
├── platformio.ini          # PlatformIO 项目配置
└── .pio/                   # PlatformIO 构建输出目录
```

## 核心模块说明

### 1. MqttHandler (MQTT 处理器)

**文件**: `include/MqttHandler.h`, `src/MqttHandler.cpp`

负责与 OneNET 平台的 MQTT 通信，包括：
- 连接管理（自动重连）
- 消息发布和订阅
- 属性设置指令处理
- 支持自定义回调函数

**关键方法**:
- `init()`: 初始化 MQTT 客户端
- `connect()`: 连接到 MQTT 服务器
- `publish()`: 发布消息到指定主题
- `loop()`: 维持 MQTT 连接（需在 loop 中调用）
- `setUserCallback()`: 设置自定义回调函数

**支持的属性类型处理**:
- 布尔值 (LED/Switch 控制)
- 字符串 (Command/Control/Upload_Data)
- 整数 (Set_Threshold)
- 浮点数

### 2. SerialHandler (串口处理器)

**文件**: `include/SerialHandler.h`, `src/SerialHandler.cpp`

负责串口数据的接收、解析和上传，支持两种工作模式：

**工作模式**:
- `NORMAL_MODE`: 正常模式，处理串口命令
- `UPLOAD_DATA_MODE`: 数据上传模式，接收键值对数据

**支持的串口命令**:
- `UPLOAD_DATA`: 进入数据上传模式
- `GET_TIME`: 获取当前时间戳
- `STATUS`: 获取系统状态
- `HELP`: 显示帮助信息

**数据上传模式**:
- 输入格式: `key=value` 或 `key:value`
- `END`: 结束输入并上传数据到 OneNET
- `CANCEL`: 取消上传

**数据格式**:
- 自动识别整数、浮点数、字符串类型
- 生成符合 OneNET 平台格式的 JSON 数据

### 3. SimpleTime (时间模块)

**文件**: `include/Time_t.h`, `src/Time_t.cpp`

提供网络时间同步和时间格式化功能：

**关键方法**:
- `begin()`: 初始化 NTP 客户端
- `getTimestamp()`: 获取当前时间戳（秒）
- `getTimestampMillis()`: 获取当前时间戳（毫秒）
- `getTimeString()`: 获取格式化时间 (HH:MM:SS)
- `getDateTimeString()`: 获取格式化日期时间 (YYYY-MM-DD HH:MM:SS)
- `isTimeSynced()`: 检查时间是否已同步

## 配置说明

### 配置文件: `include/config.h`

```cpp
// WiFi 配置
#define WIFI_SSID1 "你的WiFi名称"
#define WIFI_PASSWORD1 "你的WiFi密码"

// OneNET 平台配置
#define MQTT_SERVER "mqtts.heclouds.com"
#define MQTT_PORT 1883
#define DEVICE_ID "设备ID"
#define USERNAME "用户名"
#define PASSWORD "密码"

// MQTT 主题配置
#define PUB_post_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/post"
#define SUB_post_reply_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/post/reply"
#define SUB_set_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/set"
#define PUB_set_reply_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/set_reply"

// 系统参数
#define SERIAL_BAUD 115200
#define WIFI_CHECK_INTERVAL 30000    // WiFi 检查间隔
#define MQTT_CHECK_INTERVAL 5000     // MQTT 检查间隔
#define HEARTBEAT_INTERVAL 60000     // 心跳间隔
```

## 构建和运行

### 环境要求

- PlatformIO CLI 或 VS Code + PlatformIO 插件
- ESP8266 开发板 (ESP12E)
- USB 数据线

### 构建命令

```bash
# 清理构建文件
pio run -t clean

# 编译项目
pio run

# 上传固件到开发板
pio run -t upload

# 监控串口输出
pio device monitor

# 一键编译、上传并监控
pio run -t upload && pio device monitor
```

### PlatformIO 配置

项目使用 `platformio.ini` 配置文件，主要配置项：

```ini
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino
build_flags = -I include
monitor_speed = 115200
upload_speed = 921600
```

## 使用说明

### 启动流程

1. 上电后，设备自动初始化 GPIO（GPIO2 作为 LED 指示灯）
2. 初始化串口（波特率 115200）
3. 连接 WiFi（支持多 SSID 自动切换）
4. 初始化 MQTT 客户端并连接到 OneNET 平台
5. 订阅设备属性设置主题
6. 进入主循环，维持连接并处理数据

### 数据上报流程

1. 通过串口发送 `UPLOAD_DATA` 命令进入数据上传模式
2. 按照格式输入键值对数据（如 `temperature=25.5`）
3. 发送 `END` 命令完成输入
4. 系统自动将数据转换为 JSON 格式并上传到 OneNET 平台

### 接收控制指令

1. OneNET 平台向设备的属性设置主题发送指令
2. MQTT 处理器自动接收并解析 JSON 数据
3. 根据属性名和值类型调用相应的处理函数
4. 通过串口发送自定义数据包（格式: `<CYZ:命令:CYZ>`）给外部设备（如 STM32）

### 支持的设备属性

- **LED/Switch**: 控制 GPIO2 LED 开关（值: on/off, true/false, 1/0）
- **Upload_Data**: 触发数据上传（值: Upload_on）
- **Command/Control**: 发送控制命令到串口
- **Set_Threshold**: 设置阈值参数（整数）

## 开发规范

### 代码风格

- 使用 C++ 面向对象编程
- 类名使用大驼峰命名（如 `MqttHandler`）
- 成员变量使用小驼峰命名（如 `serialBuffer`）
- 常量使用全大写下划线命名（如 `WIFI_CHECK_INTERVAL`）
- 函数名使用小驼峰命名（如 `processSerialCommand`）

### 模块化设计

- 每个功能模块封装为独立的类
- 头文件包含必要的注释说明
- 使用前向声明减少头文件依赖
- 公共接口清晰，私有实现细节隐藏

### 错误处理

- MQTT 连接失败自动重连
- WiFi 断开自动重连
- JSON 解析错误返回错误响应
- 串口数据格式验证

### 调试输出

- 使用 `Serial.println()` 输出调试信息
- 关键操作都有日志输出
- 错误信息包含详细描述

## 依赖库

项目依赖以下库（已通过 PlatformIO 自动管理）：

- ESP8266WiFi
- ESP8266WiFiMulti
- ESP8266HTTPClient
- ArduinoJson
- PubSubClient
- ESP8266mDNS
- ESP8266WebServer
- arduino-libraries/NTPClient@^3.2.1

## 注意事项

1. **配置修改**: 修改 `include/config.h` 中的 WiFi 和 OneNET 配置后需要重新编译
2. **串口波特率**: 确保串口监视器波特率设置为 115200
3. **MQTT 连接**: OneNET 平台的密码有时间限制，过期后需要重新生成
4. **GPIO 引脚**: GPIO2 作为 LED 指示灯，连接低电平有效
5. **内存管理**: ESP8266 内存有限，注意 JSON 文档大小不要过大

## 扩展开发

### 添加新的设备属性

1. 在 `MqttHandler.cpp` 的 `handleDeviceProperty` 或相关函数中添加处理逻辑
2. 根据需要通过串口发送数据包到外部设备

### 修改数据上传格式

1. 修改 `SerialHandler.cpp` 中的 `generateJsonPayload()` 函数
2. 调整 JSON 结构以符合新的数据格式要求

### 添加新的串口命令

1. 在 `SerialHandler.cpp` 的 `processSerialCommand()` 函数中添加命令处理逻辑
2. 更新 `HELP` 命令的输出信息

## 常见问题

**Q: WiFi 连接失败怎么办？**
A: 检查 `config.h` 中的 WiFi SSID 和密码是否正确，确保 WiFi 信号强度足够。

**Q: MQTT 连接失败怎么办？**
A: 检查 OneNET 平台的设备 ID、用户名和密码是否正确，确认网络连接正常。

**Q: 数据上传失败怎么办？**
A: 检查串口数据格式是否正确（key=value 或 key:value），确保 MQTT 连接正常。

**Q: 如何查看调试信息？**
A: 使用串口监视器（波特率 115200）查看设备输出的调试信息。

## 版本信息

- **版本**: v2.0
- **平台**: ESP8266 (ESP12E)
- **框架**: Arduino
- **构建工具**: PlatformIO