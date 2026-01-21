#ifndef CONFIG_H
#define CONFIG_H

// ==================== WiFi配置 ====================
#define WIFI_SSID1 "YOUR_WIFI_SSID"
#define WIFI_PASSWORD1 "YOUR_WIFI_PASSWORD"
// #define WIFI_SSID2 "YOUR_SECOND_WIFI_SSID"
// #define WIFI_PASSWORD2 "YOUR_SECOND_WIFI_PASSWORD"

// ==================== OneNET平台配置 ====================
#define MQTT_SERVER "mqtts.heclouds.com"
#define MQTT_PORT 1883
#define DEVICE_ID "YOUR_DEVICE_ID"
#define USERNAME "YOUR_PRODUCT_ID"
#define PASSWORD "YOUR_ENCRYPTED_PASSWORD"

// ==================== MQTT主题配置 ====================
#define PUB_post_TOPIC "$sys/YOUR_PRODUCT_ID/YOUR_DEVICE_ID/thing/property/post"
#define SUB_post_reply_TOPIC "$sys/YOUR_PRODUCT_ID/YOUR_DEVICE_ID/thing/property/post/reply"
#define SUB_set_TOPIC "$sys/YOUR_PRODUCT_ID/YOUR_DEVICE_ID/thing/property/set"
#define PUB_set_reply_TOPIC "$sys/YOUR_PRODUCT_ID/YOUR_DEVICE_ID/thing/property/set_reply"

// ==================== 系统参数配置 ====================
#define SERIAL_BAUD 115200
#define WIFI_CHECK_INTERVAL 30000
#define MQTT_CHECK_INTERVAL 30000
#define HEARTBEAT_INTERVAL 30000
#define MAX_MESSAGE_LENGTH 100
#define MAX_DATA_BUFFER_SIZE 50

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
