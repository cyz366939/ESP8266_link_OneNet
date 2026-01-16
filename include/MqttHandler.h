#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

class MqttHandler {
private:
    WiFiClient* wifiClient;
    PubSubClient* mqttClient;
    void (*propertySetCallback)(const String&, const String&);
    
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
    bool init();
    void setUserCallback(void (*callback)(const String&, const String&));
    bool connect();
    void loop();
    bool publish(const char* topic, const char* payload);
    bool isConnected();
    void sendHeartbeat();
};

#endif