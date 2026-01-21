#include <MqttHandler.h>
#include <config.h>
//构造函数
MqttHandler::MqttHandler(WiFiClient* client) {
    wifiClient = client;
    mqttClient = new PubSubClient(*wifiClient);
    propertySetCallback = nullptr;
    lastPublishAttempt = 0;
    publishRetryCount = 0;
}
//析构函数
MqttHandler::~MqttHandler() {
    if (mqttClient != nullptr) {
        delete mqttClient;
        mqttClient = nullptr;
    }
}
//初始化MQTT连接
bool MqttHandler::init() {
    mqttClient->setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient->setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->mqttCallback(topic, payload, length);
    });
    Serial.println("MQTT客户端初始化完成");
    return true;
}
//设置自定义回调函数
void MqttHandler::setUserCallback(void (*callback)(const String&topic, const String&payload)) {
    propertySetCallback = callback;
}
//连接MQTT服务器并订阅主题
bool MqttHandler::connect(const char *topic) {
    if (mqttClient->connected()) {
        return true;
    }

    Serial.println("正在连接MQTT服务器...");

    String clientId = DEVICE_ID; // 使用设备ID作为客户端ID
    if (mqttClient->connect(clientId.c_str(), USERNAME, PASSWORD)) {
        Serial.println("MQTT连接成功!");

        if (mqttClient->subscribe(topic)) {
            Serial.println("成功订阅主题: " + String(topic));
        } else {
            Serial.println("订阅主题失败: " + String(topic));
        }

        return true;
    } else {
        Serial.print("MQTT连接失败，状态码: ");
        Serial.println(mqttClient->state());
        return false;
    }
}
//保持MQTT心跳
void MqttHandler::loop() {
    mqttClient->loop();
    processMessageQueue();
}
//处理消息队列
void MqttHandler::processMessageQueue() {
    if (messageQueue.empty()) {
        return;
    }

    unsigned long currentTime = millis();

    // 从队列尾部开始处理（FIFO）
    for (auto it = messageQueue.begin(); it != messageQueue.end(); ) {
        if (currentTime >= it->nextAttemptTime) {
            bool success = mqttClient->publish(it->topic.c_str(), it->payload.c_str());

            if (success) {
                Serial.println("队列消息发布成功: " + it->topic);
                it = messageQueue.erase(it);
            } else {
                it->retryCount++;
                if (it->retryCount >= MAX_RETRY_COUNT) {
                    Serial.println("队列消息发布失败（已达最大重试次数）: " + it->topic);
                    it = messageQueue.erase(it);
                } else {
                    it->nextAttemptTime = currentTime + 5000; // 5秒后重试
                    Serial.println("队列消息发布失败，将在5秒后重试: " + it->topic + " (重试: " + String(it->retryCount) + ")");
                    ++it;
                }
            }
        } else {
            ++it;
        }
    }
}
//发布消息到指定主题
bool MqttHandler::publish(const char* topic, const char* payload, bool queued) {
    if (!mqttClient->connected()) {
        // 如果未连接且允许队列模式，加入队列
        if (queued) {
            if (messageQueue.size() >= MAX_QUEUE_SIZE) {
                Serial.println("警告: 消息队列已满，丢弃消息: " + String(topic));
                return false;
            }
            PendingMessage msg;
            msg.topic = String(topic);
            msg.payload = String(payload);
            msg.retryCount = 0;
            msg.nextAttemptTime = millis() + 1000;
            messageQueue.push_back(msg);
            Serial.println("消息已加入队列: " + String(topic) + " (队列大小: " + String(messageQueue.size()) + ")");
            return true;
        }
        return false;
    }

    bool result = mqttClient->publish(topic, payload);
    if (result) {
        Serial.println("MQTT发布成功 [" + String(topic) + "]: " + String(payload));
        publishRetryCount = 0;
    } else {
        Serial.println("MQTT发布失败 [" + String(topic) + "]: " + String(payload));

        // 失败时加入队列
        if (messageQueue.size() < MAX_QUEUE_SIZE) {
            PendingMessage msg;
            msg.topic = String(topic);
            msg.payload = String(payload);
            msg.retryCount = 0;
            msg.nextAttemptTime = millis() + 2000;
            messageQueue.push_back(msg);
            Serial.println("消息已加入重传队列: " + String(topic));
        } else {
            Serial.println("警告: 消息队列已满，丢弃消息: " + String(topic));
        }

        if (publishRetryCount < 3) {
            publishRetryCount++;
            delay(1000);
            return publish(topic, payload, false);
        } else {
            publishRetryCount = 0;
        }
    }
    return result;
}
//订阅主题
bool MqttHandler::subscribe(const char* topic)
{
    if (!mqttClient->connected()) {
        return false;
    }
    if(mqttClient->subscribe(topic)){
        Serial.println("成功订阅主题: " + String(topic));
        return true;
    }
    else
    {
        Serial.println("主题订阅失败");
        return false;
    }
    
}
//检查MQTT连接状况
bool MqttHandler::isConnected() {
    return mqttClient->connected();
}
//发送心跳信息向主题
void MqttHandler::sendHeartbeat() {
    if (!mqttClient->connected()) {
        return;
    }

    StaticJsonDocument<256> doc;
    doc["device_id"] = DEVICE_ID;
    doc["status"] = "online";
    doc["timestamp"] = millis();

    String message;
    serializeJson(doc, message);

    publish(PUB_post_TOPIC, message.c_str());
}

/*=======================OneNET回调处理========================*/

//发送设备属性设置响应
void MqttHandler::sendPropertySetResponse(const String& requestId, int code, const String& message) {
    StaticJsonDocument<512> responseDoc;

    if (requestId.length() > 0) {
        responseDoc["id"] = requestId;
    } else {
        responseDoc["id"] = String(millis());
    }

    responseDoc["code"] = code;

    if (code == 200) {
        responseDoc["message"] = "success";

        JsonObject data = responseDoc.createNestedObject("data");
        data["result"] = message;

        JsonObject deviceStatus = data.createNestedObject("deviceStatus");
        deviceStatus["设备名称"] = "我的ESP8266设备";
        deviceStatus["uptime"] = millis() / 1000;
        deviceStatus["freeHeap"] = ESP.getFreeHeap();
    } else {
        responseDoc["message"] = message;
    }

    String responsePayload;
    serializeJson(responseDoc, responsePayload);

    Serial.println("\r\n");
    Serial.println("开始发送设备属性设置响应...");

    publish(PUB_set_reply_TOPIC, responsePayload.c_str());
}
//MQTT消息回调函数
void MqttHandler::mqttCallback(char* topic, byte* payload, unsigned int length) {

    String topicStr = String(topic);
    String payloadStr = "";

    for (unsigned int i = 0; i < length && i < MAX_MESSAGE_LENGTH; i++) {
        payloadStr += (char)payload[i];
    }
    if (length > MAX_MESSAGE_LENGTH) {
        payloadStr += "...(消息过长，已截断)";
    }
    Serial.print("收到消息【");
    Serial.print(topicStr);
    Serial.print("】: ");
    Serial.println(payloadStr);

    //检查用户自定义回调函数，优先调用用户自定义回调函数
    if (propertySetCallback) {
    propertySetCallback(topicStr,payloadStr);
    }
    //是否是属性设置回调
    if (strcmp(topic, SUB_set_TOPIC) == 0) {
    handlePropertySetCommand(topicStr,payloadStr);
    }
}
//处理设备属性设置指令
void MqttHandler::handlePropertySetCommand(const String& topic, const String& payload) {

    Serial.println("\r\n");
    Serial.println("开始处理设备属性设置响应...");

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.println("JSON解析失败: " + String(error.c_str()));
        sendPropertySetResponse("", 400, "JSON解析失败");
        return;
    }

    String requestId = doc["id"] | "";

    if (!doc.containsKey("params")) {
        Serial.println("请求中缺少params参数");
        sendPropertySetResponse(requestId, 400, "缺少params参数");
        return;
    }
    sendPropertySetResponse(requestId, 200, "success");

    JsonObject params = doc["params"];
    Serial.println("\r\n");
    Serial.println("本次设置了以下属性:");

    for (JsonPair kv : params) {
        String propertyName = String(kv.key().c_str());
        JsonVariant propertyValue = kv.value();

        Serial.print("  ");
        Serial.print(propertyName);
        Serial.print(" = ");
        processPropertySetValue(propertyName, propertyValue);
    }
}
//根据设备属性值的类型进行不同的处理选择
void MqttHandler::processPropertySetValue(const String& propertyName, const JsonVariant& propertyValue) {
    /*根据值的类型进行不同的处理*/
    if (propertyValue.is<const char*>()) {
        // 处理字符串类型属性
        String value = propertyValue.as<String>();
        Serial.println(value);
        handleDeviceProperty_String(propertyName, value);//调用处理设备属性的函数，传入属性名和属性值
    }
    else if (propertyValue.is<int>()) {
        // 处理整数类型属性
        int value = propertyValue.as<int>();
        Serial.println(value);
        handleDeviceProperty(propertyName, String(value));
    }
    else if (propertyValue.is<float>()) {
        // 处理浮点数类型属性
        float value = propertyValue.as<float>();
        Serial.println(value, 2);
        handleDeviceProperty(propertyName, String(value));
    }
    else if (propertyValue.is<bool>()) {
        // 处理布尔类型属性
        bool value = propertyValue.as<bool>();
        Serial.println(value ? "true" : "false");
        handleDeviceProperty(propertyName, String(value ? "true" : "false"));
    }
}

/*=====================具体的回调逻辑处理函数========================*/

//1.布尔类型属性处理
void MqttHandler::handleDeviceProperty(const String& propertyName, const String& value) {
    if (propertyName == "LED" || propertyName == "Switch") {
        bool state = (value == "on" || value == "true" || value == "1" || value == "HIGH");
        digitalWrite(LED_GPIO_PIN, state ? LOW:HIGH);
        Serial.print("标识符:"+propertyName+"，其设置值为: ");
        Serial.println(value);
    }
    else {
        Serial.print("未知属性: ");
        Serial.print(propertyName);
        Serial.print(" = ");
        Serial.println(value);
    }
}
//2.处理字符串类型属性
void MqttHandler::handleDeviceProperty_String(const String& propertyName, const String& value) {
    // 处理字符串类型属性（向STM32发送命令，自定义的数据包<CYZ:XXX:CYZ>）
    
    Serial.print("标识符:"+propertyName+"，其值设置为: ");
    Serial.println(value);
    //根据不同标识符进行不同处理
    if(propertyName == "Upload_Data") { 
        // 这里可以添加实际的逻辑
        if (value == "Upload_on") {
            Serial.println("控制开始一次数据上传...");
            Serial.println("发送数据上传数据包：<CYZ:" + value + ":CYZ>");
        } else {
            Serial.println("不属于当前标识符的有效命令：<CYZ:unknown_command:CYZ>");
        }
        
    }
    // 可以添加其他标识符属性的处理逻辑
    //else if {...}
    else if(propertyName == "Command" || propertyName == "Control") {
        //发送指令给STM32控制
        Serial.println("发送命令数据包：<CYZ:" + value + ":CYZ>");
    }
    else {
        // 其他字符串属性
        Serial.print("未知字符串属性: ");
        Serial.print(propertyName);
        Serial.print(" = ");
        Serial.println(value);
    }

}
//3.处理整数类型属性
void MqttHandler::handleDeviceProperty_Int(const String& propertyName, int value) {
    // 处理整数类型属性（向STM32发送命令，自定义的数据包<CYZ:XXX:CYZ>）
    
    Serial.print("标识符:"+propertyName+"，其设置值为: ");
    Serial.println(value);
    //根据不同标识符进行不同处理
    if(propertyName == "Set_Threshold") { 
        // 这里可以添加实际的逻辑
        Serial.println("发送设置阈值数据包：<CYZ:" + String(value) + ":CYZ>");
    }
    // 可以添加其他整数属性的处理逻辑
    //else if {...}
    else {
        // 其他整数属性
        Serial.print("未知整数属性: ");
        Serial.print(propertyName);
        Serial.print(" = ");
        Serial.println(value);
    }

}
//4.处理浮点数类型属性
void MqttHandler::handleDeviceProperty_Float(const String& propertyName, float value) {
    Serial.print("标识符:" + propertyName + "，其设置值为: ");
    Serial.println(value, 2);

    if (propertyName == "Set_Threshold_Float") {
        Serial.println("发送设置浮点阈值数据包：<CYZ:" + String(value, 2) + ":CYZ>");
    } else if (propertyName == "Set_Temperature") {
        Serial.println("设置温度阈值: " + String(value, 2));
    } else if (propertyName == "Set_Humidity") {
        Serial.println("设置湿度阈值: " + String(value, 2));
    } else {
        Serial.print("未知浮点数属性: ");
        Serial.print(propertyName);
        Serial.print(" = ");
        Serial.println(value, 2);
    }
}

