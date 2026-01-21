#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <Arduino.h>
#include <vector>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h> 
#include "config.h"
#include "MqttHandler.h"
#include "TIME_T.h"


// 数据接收状态枚举
enum DataReceiveState {
    NORMAL_MODE,      // 正常模式
    UPLOAD_DATA_MODE  // 数据上传模式
};

// 键值对数据结构
struct KeyValueData {
    String key;
    String value;
    bool isValid;
};

class SerialHandler {
private:
    String serialBuffer;
    DataReceiveState currentState;
    std::vector<KeyValueData> dataBuffer;
    unsigned long uploadStartTime;
    MqttHandler* mqttHandler;  // MQTT处理器引用
    
    // 数据处理函数
    bool validateKeyValueFormat(const String& data, String& key, String& value, char& separator);
    void processUploadDataCommand();
    void processKeyValueData(const String& data);
    void processEndCommand();
    void processCancelCommand();
    void clearDataBuffer();
    String generateJsonPayload() const;

    bool isInteger(const String& value) const;
    bool isFloat(const String& value) const;

public:

    // 构造函数，初始化串口处理器
    SerialHandler();
    // 初始化串口处理器
    void init();
    // 读取串口数据
    void readSerialData();//在void loop()中调用
    // 处理串口命令
    void processSerialCommand(String command);
    // 检查WiFi连接状态
    bool isWiFiConnected();
    // 获取当前数据接收状态
    DataReceiveState getCurrentState() const { return currentState; }
    //获取数据缓冲区中的数据数量
    size_t getDataBufferCount() const { return dataBuffer.size(); }
    // 设置MQTT处理器引用
    void setMqttHandler(MqttHandler* handler) { mqttHandler = handler; }
    //检查是否有待上传的数据
    bool hasDataToUpload() const { return dataBuffer.size() > 0; }
    //获取JSON格式的数据负载
    String getJsonPayload() const { return generateJsonPayload(); }
    //清除已上传的数据
    void clearUploadedData() { clearDataBuffer(); }
};

#endif
