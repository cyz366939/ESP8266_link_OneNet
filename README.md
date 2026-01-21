# ESP8266 MQTT Device

基于ESP8266的MQTT物联网设备项目，支持与OneNET云平台通信。

## 功能特性

- WiFi自动连接与重连
- MQTT协议通信
- OneNET平台集成
- 串口命令控制
- 时间同步（NTP）
- 数据上报功能
- 设备属性控制

## 硬件要求

- ESP8266开发板（ESP-12E）
- USB转串口模块
- LED指示灯（可选）

## 软件依赖

- PlatformIO
- Arduino Framework
- ESP8266WiFi @ 1.0
- ArduinoJson @ 6.21.3
- PubSubClient @ 2.8
- arduino-libraries/NTPClient @ 3.2.1

## 安装配置

### 1. 克隆项目

```bash
git clone <repository-url>
cd Trae_2_ESP
```

### 2. 配置WiFi和MQTT

复制配置模板并填入实际信息：

```bash
cp include/config_template.h include/config.h
```

编辑 `include/config.h`，填入以下信息：

```cpp
// WiFi配置
#define WIFI_SSID1 "你的WiFi名称"
#define WIFI_PASSWORD1 "你的WiFi密码"

// OneNET平台配置
#define DEVICE_ID "你的设备ID"
#define USERNAME "你的产品ID"
#define PASSWORD "你的加密密码"
```

### 3. 编译上传

使用PlatformIO编译并上传：

```bash
pio run --target upload
```

或使用VS Code PlatformIO插件上传。

## 使用说明

### 串口命令

设备启动后，通过串口（115200波特率）发送以下命令：

#### 基本命令

- `UPLOAD_DATA` - 进入数据上传模式
- `GET_TIME` - 获取当前时间戳
- `STATUS` - 获取设备状态
- `HELP` - 显示帮助信息

#### 数据上传模式

进入数据上传模式后，可以输入键值对数据：

```
temperature=25.5
humidity=60
END
```

支持的格式：
- `key=value`
- `key:value`

结束命令：
- `END` - 完成上传并发送到OneNET
- `CANCEL` - 取消上传

### MQTT主题

#### 发布主题

- `$sys/{产品ID}/{设备ID}/thing/property/post` - 上报设备属性
- `$sys/{产品ID}/{设备ID}/thing/property/set_reply` - 响应属性设置

#### 订阅主题

- `$sys/{产品ID}/{设备ID}/thing/property/set` - 接收属性设置指令

## 项目结构

```
Trae_2_ESP/
├── include/           # 头文件
│   ├── config.h      # 配置文件（需自行创建）
│   ├── config_template.h  # 配置模板
│   ├── MqttHandler.h # MQTT处理器
│   ├── SerialHandler.h # 串口处理器
│   └── Time_t.h      # 时间处理
├── src/              # 源文件
│   ├── main.cpp      # 主程序
│   ├── MqttHandler.cpp
│   ├── SerialHandler.cpp
│   └── Time_t.cpp
├── platformio.ini    # PlatformIO配置
└── README.md        # 项目说明
```

## 配置说明

### WiFi配置

支持多WiFi配置，优先连接第一个可用的网络：

```cpp
#define WIFI_SSID1 "WiFi名称1"
#define WIFI_PASSWORD1 "密码1"
#define WIFI_SSID2 "WiFi名称2"
#define WIFI_PASSWORD2 "密码2"
```

### 系统参数

```cpp
#define SERIAL_BAUD 115200           // 串口波特率
#define WIFI_CHECK_INTERVAL 60000    // WiFi检查间隔(ms)
#define MQTT_CHECK_INTERVAL 60000    // MQTT检查间隔(ms)
#define HEARTBEAT_INTERVAL 30000     // 心跳间隔(ms)
```

## 故障排除

### WiFi连接失败

1. 检查WiFi名称和密码是否正确
2. 确认ESP8266在WiFi覆盖范围内
3. 查看串口输出的错误信息

### MQTT连接失败

1. 确认OneNET平台配置正确
2. 检查设备ID、用户名和密码
3. 验证网络连接状态

### 编译错误

1. 确保已安装PlatformIO
2. 检查依赖库是否正确安装
3. 清理构建缓存：`pio run --target clean`

## 开发指南

### 添加新的MQTT属性处理

在 `MqttHandler.cpp` 中添加处理逻辑：

```cpp
void MqttHandler::handleDeviceProperty(const String& propertyName, const String& value) {
    if (propertyName == "YourProperty") {
        // 处理逻辑
    }
}
```

### 扩展串口命令

在 `SerialHandler.cpp` 的 `processSerialCommand` 函数中添加：

```cpp
if (command == "YOUR_COMMAND") {
    // 命令处理逻辑
}
```


