#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置 
#define WIFI_SSID1 "cyz"
#define WIFI_PASSWORD1 "mzzx123321"
// #define WIFI_SSID2 "WIFI_SSID_2"
// #define WIFI_PASSWORD2 "WIFI_PASS_2"

// OneNET平台配置
#define MQTT_SERVER "mqtts.heclouds.com"
#define MQTT_PORT 1883
#define DEVICE_ID "Carrier"
#define USERNAME "wyAD40JBtZ"
#define PASSWORD "version=2018-10-31&res=products%2FwyAD40JBtZ%2Fdevices%2FCarrier&et=2712538324&method=md5&sign=i75wHA3H5zjivQX5RjdXSA%3D%3D"

// MQTT主题配置
#define PUB_post_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/post"
#define SUB_post_reply_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/post/reply"
#define SUB_set_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/set"
#define PUB_set_reply_TOPIC "$sys/wyAD40JBtZ/Carrier/thing/property/set_reply"

// 系统参数配置
#define SERIAL_BAUD 115200
#define WIFI_CHECK_INTERVAL 30000    // WiFi检查间隔(ms)
#define MQTT_CHECK_INTERVAL 5000     // MQTT检查间隔(ms)
#define HEARTBEAT_INTERVAL 60000     // 心跳间隔(ms)

#endif