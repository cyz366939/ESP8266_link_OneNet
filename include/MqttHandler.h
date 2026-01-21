#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// 待发送消息结构
struct PendingMessage {
    String topic;
    String payload;
    int retryCount;
    unsigned long nextAttemptTime;
};

class MqttHandler {
private:
    WiFiClient* wifiClient;
    PubSubClient *mqttClient;
    void (*propertySetCallback)(const String&topic, const String&payload);

    unsigned long lastPublishAttempt;// 上一次发布尝试的时间戳（以毫秒为单位）
    int publishRetryCount;//

    std::vector<PendingMessage> messageQueue; // 消息重传队列
    const int MAX_QUEUE_SIZE = 10; // 最大队列大小
    const int MAX_RETRY_COUNT = 3; // 最大重试次数

    void processMessageQueue(); // 处理消息队列

    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void handlePropertySetCommand(const String& topic, const String& payload);
    void sendPropertySetResponse(const String& requestId, int code, const String& message);
    void processPropertySetValue(const String& propertyName, const JsonVariant& propertyValue);
    void handleDeviceProperty(const String& propertyName, const String& value);
    void handleDeviceProperty_String(const String& propertyName, const String& value);
    void handleDeviceProperty_Int(const String& propertyName, int value);
    void handleDeviceProperty_Float(const String& propertyName, float value);

public:

    MqttHandler(WiFiClient* client);
    ~MqttHandler();
    bool init();
    void setUserCallback(void (*callback)(const String&topic, const String&payload));
    bool connect(const char *topic);
    void loop();
    bool publish(const char* topic, const char* payload, bool queued = false);
    bool subscribe(const char* topic);
    bool isConnected();
    void sendHeartbeat();
    size_t getQueueSize() const { return messageQueue.size(); }
};

#endif