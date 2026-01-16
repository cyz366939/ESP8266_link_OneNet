#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "config.h"
#include "SerialHandler.h"
#include "MqttHandler.h"
#include "TIME_T.h"

// 全局对象
ESP8266WiFiMulti wifiMulti;
WiFiClient wifiClient;
SerialHandler serialHandler;
MqttHandler mqttHandler(&wifiClient);

// 定时器变量
unsigned long lastWiFiCheck = 0;
unsigned long lastMQTTCheck = 0;

//GPIO口初始化函数
void initGPIO() {
    pinMode(2, OUTPUT); // GPIO2用于指示灯
    digitalWrite(2, HIGH); // 初始状态关闭LED
}
// WiFi连接函数
void connectWiFi() {
    Serial.println("正在连接WiFi...");
    wifiMulti.addAP(WIFI_SSID1, WIFI_PASSWORD1);
    //wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
    
    int attempts = 0;
    while (wifiMulti.run() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (wifiMulti.run() == WL_CONNECTED) {
        Serial.println("\nWiFi连接成功!");
        Serial.print("连接到SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi连接失败!");
    }
}

void setup() {
    // 初始化GPIO
    initGPIO();
    // 初始化串口处理模块，波特率设置为115200
    serialHandler.init();
    // 设置MQTT处理器引用，在串口处理模块中使用MQTT的功能函数
    serialHandler.setMqttHandler(&mqttHandler);
    //打印启动信息
    Serial.println("MQTT连接程序启动...");
    // 连接WiFi
    connectWiFi();
    // 初始化MQTT处理模块
    mqttHandler.init();
    // 连接MQTT
    mqttHandler.connect();
    //初始化时间戳模块
    SimpleTime::begin();
    //打印初始化完成信息
    Serial.println("初始化完成");
    Serial.println("支持的串口指令:");
    Serial.println("  1.UPLOAD_DATA - 进入数据上报模式");
    Serial.println("  2.GET_TIME - 获取当前时间戳");
    Serial.println("  3.STATUS - 获取状态");
    Serial.println("  4.HELP - 显示帮助");
}

void loop() {
    unsigned long currentMillis = millis();
    
    // 检查WiFi连接
    if (currentMillis - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
        lastWiFiCheck = currentMillis;
        if (wifiMulti.run() != WL_CONNECTED) {
            Serial.println("WiFi连接断开，尝试重连...");
            connectWiFi();
        }
    }
    // 检查MQTT连接
    if (currentMillis - lastMQTTCheck >= MQTT_CHECK_INTERVAL) {
        lastMQTTCheck = currentMillis;
        if (!mqttHandler.isConnected()) {
            Serial.println("MQTT连接断开，尝试重连...");
            mqttHandler.connect();
        }
    }
    // 维持MQTT连接
    mqttHandler.loop();
    // 处理串口数据
    serialHandler.readSerialData();
    
    delay(100);
}


