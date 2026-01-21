#ifndef CONFIG_H
#define CONFIG_H

// ==================== WiFi配置 ====================
#define WIFI_SSID1 "cyz"
#define WIFI_PASSWORD1 "mzzx123321"
// #define WIFI_SSID2 "WIFI_SSID_2"
// #define WIFI_PASSWORD2 "WIFI_PASS_2"

// ==================== OneNET平台配置 ====================
#define MQTT_SERVER "mqtts.heclouds.com"
#define MQTT_PORT 1883
#define DEVICE_ID "Carrier"
#define USERNAME "wyAD40JBtZ"
#define PASSWORD "version=2018-10-31&res=products%2FwyAD40JBtZ%2Fdevices%2FCarrier&et=2712538324&method=md5&sign=i75wHA3H5zjivQX5RjdXSA%3D%3D"

// ==================== MQTT主题配置 ====================
#define PUB_post_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/post"
#define SUB_post_reply_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/post/reply"
#define SUB_set_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/set"
#define PUB_set_reply_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/set_reply"

// ==================== 系统参数配置 ====================
#define SERIAL_BAUD 115200//串口波特率
#define WIFI_CHECK_INTERVAL 30000//WiFi重连间隔（优化：60秒→30秒）
#define MQTT_CHECK_INTERVAL 30000//MQTT重连间隔（优化：60秒→30秒）
#define HEARTBEAT_INTERVAL 30000//心跳包发送间隔
#define MAX_MESSAGE_LENGTH 100//最大消息长度
#define MAX_DATA_BUFFER_SIZE 50//最大数据缓冲区条目数（防止内存溢出）

// ==================== 日志级别配置 ====================
// 0=关闭, 1=错误, 2=警告, 3=信息, 4=调试
#define LOG_LEVEL 3

// 日志宏定义
#define LOG_ERROR(msg)   if(LOG_LEVEL >= 1) Serial.println("[ERROR] " + String(msg))
#define LOG_WARNING(msg) if(LOG_LEVEL >= 2) Serial.println("[WARN] " + String(msg))
#define LOG_INFO(msg)    if(LOG_LEVEL >= 3) Serial.println("[INFO] " + String(msg))
#define LOG_DEBUG(msg)   if(LOG_LEVEL >= 4) Serial.println("[DEBUG] " + String(msg))

// ==================== GPIO配置 ====================
#define LED_GPIO_PIN 2

#endif
