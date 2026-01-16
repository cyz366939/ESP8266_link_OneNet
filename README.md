# ESP8266 OneNET MQTT 驱动

基于 ESP8266 (ESP12E) 的 IoT 项目，实现与 OneNET 云平台的 MQTT 通信功能。

## 功能特性

- ✅ WiFi 多 SSID 自动切换连接
- ✅ MQTT 稳定连接与自动重连
- ✅ 串口数据接收与自动上传
- ✅ OneNET 属性设置指令接收
- ✅ NTP 网络时间同步
- ✅ 模块化设计，易于扩展

## 硬件要求

- ESP8266 开发板 (ESP12E)
- USB 数据线

## 软件环境

- PlatformIO CLI 或 VS Code + PlatformIO 插件
- Arduino Framework

## 快速开始

### 1. 克隆项目

```bash
git clone https://github.com/cyz366939/ESP8266_link_OneNet.git
cd ESP8266_link_OneNet
```

### 2. 配置参数

编辑 `include/config.h` 文件，修改以下配置：

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
```

### 3. 编译上传

```bash
# 编译项目
pio run

# 上传到开发板
pio run -t upload

# 监控串口输出
pio device monitor
```

## 使用说明

### 串口命令

- `UPLOAD_DATA` - 进入数据上传模式
- `GET_TIME` - 获取当前时间戳
- `STATUS` - 获取系统状态
- `HELP` - 显示帮助信息

### 数据上传

1. 发送 `UPLOAD_DATA` 命令
2. 输入键值对数据（格式：`key=value` 或 `key:value`）
3. 发送 `END` 完成上传
4. 发送 `CANCEL` 取消上传

示例：
```
UPLOAD_DATA
temperature=25.5
humidity=60.2
END
```

### 接收控制指令

OneNET 平台可通过以下属性控制设备：

- `LED/Switch` - 控制 LED 开关（on/off, true/false, 1/0）
- `Upload_Data` - 触发数据上传（Upload_on）
- `Command/Control` - 发送控制命令
- `Set_Threshold` - 设置阈值参数

## 项目结构

```
.
├── src/                    # 源代码
│   ├── main.cpp            # 主程序
│   ├── MqttHandler.cpp     # MQTT 处理
│   ├── SerialHandler.cpp   # 串口处理
│   └── Time_t.cpp          # 时间模块
├── include/                # 头文件
│   ├── config.h            # 配置文件
│   ├── MqttHandler.h
│   ├── SerialHandler.h
│   └── Time_t.h
├── platformio.ini          # PlatformIO 配置
└── README.md               # 项目说明
```

## 核心模块

### MqttHandler

负责与 OneNET 平台的 MQTT 通信，包括连接管理、消息发布订阅和属性设置处理。

### SerialHandler

负责串口数据的接收、解析和上传，支持键值对数据格式和自动类型识别。

### SimpleTime

提供网络时间同步和时间格式化功能。

## 依赖库

- ESP8266WiFi
- ESP8266WiFiMulti
- PubSubClient
- ArduinoJson
- NTPClient

## 注意事项

1. 修改配置后需要重新编译
2. 串口波特率设置为 115200
3. OneNET 密码有时间限制，过期需重新生成
4. GPIO2 作为 LED 指示灯（低电平有效）

## 许可证

本项目采用 MIT 许可证。

## 作者

cyz366939

## 相关链接

- [OneNET 官方文档](https://open.iot.10086.cn/doc/)
- [PlatformIO 文档](https://docs.platformio.org/)
- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)