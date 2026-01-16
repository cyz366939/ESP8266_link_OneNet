#include <MqttHandler.h>
#include <config.h>
//构造函数
MqttHandler::MqttHandler(WiFiClient* client) {
    wifiClient = client;
    mqttClient = new PubSubClient(*wifiClient);
    propertySetCallback = nullptr;
}
//初始化MQTT客户端
bool MqttHandler::init() {
    mqttClient->setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient->setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->mqttCallback(topic, payload, length);
    });
    Serial.println("MQTT客户端初始化完成");
    return true;
}
//设置自定义回调函数
void MqttHandler::setUserCallback(void (*callback)(const String&, const String&)) {
    propertySetCallback = callback;
}
//连接MQTT服务器
bool MqttHandler::connect() {
    if (mqttClient->connected()) {
        return true;
    }
    
    Serial.println("正在连接MQTT服务器...");
    
    String clientId = DEVICE_ID;
    if (mqttClient->connect(clientId.c_str(), USERNAME, PASSWORD)) {
        Serial.println("MQTT连接成功!");
        
        // 订阅主题
        if (mqttClient->subscribe(SUB_set_TOPIC)) {
            Serial.println("成功订阅主题: " + String(SUB_set_TOPIC));
        } else {
            Serial.println("订阅主题失败: " + String(SUB_set_TOPIC));
        }
        
        return true;
    } else {
        Serial.print("MQTT连接失败，状态码: ");
        Serial.println(mqttClient->state());
        return false;
    }
}
//保持MQTT连接
void MqttHandler::loop() {
    mqttClient->loop();
}
//发布MQTT消息
bool MqttHandler::publish(const char* topic, const char* payload) {
    if (!mqttClient->connected()) {
        return false;
    }
    
    bool result = mqttClient->publish(topic, payload);
    if (result) {
        Serial.println("MQTT发布成功 [" + String(topic) + "]: " + String(payload));
    } else {
        Serial.println("MQTT发布失败 [" + String(topic) + "]: " + String(payload));
    }
    return result;
}
//检查MQTT连接状态
bool MqttHandler::isConnected() {
    return mqttClient->connected();
}
//发送心跳消息
void MqttHandler::sendHeartbeat() {
    if (!mqttClient->connected()) {
        return;
    }
    
    DynamicJsonDocument doc(256);
    doc["device_id"] = DEVICE_ID;
    doc["status"] = "online";
    doc["timestamp"] = millis();  // 心跳消息可以使用ESP8266内部时间戳
    
    String message;
    serializeJson(doc, message);
    
    publish(PUB_post_TOPIC, message.c_str());
}

/*=======================OneNET回调处理========================*/

//发送设备属性设置响应
void MqttHandler::sendPropertySetResponse(const String& requestId, int code, const String& message) {
    // 默认响应处理逻辑
    DynamicJsonDocument responseDoc(512);
    
    // 设置响应ID
    if (requestId.length() > 0) {
        responseDoc["id"] = requestId;
    } else {
        responseDoc["id"] = String(millis());
    }
    
    // 设置响应码
    responseDoc["code"] = code;
    
    // 设置响应消息
    if (code == 200) {
        responseDoc["message"] = "success";
        
        // 添加成功时的数据
        JsonObject data = responseDoc.createNestedObject("data");
        data["result"] = message;
        
        // 可以添加当前设备状态信息
        JsonObject deviceStatus = data.createNestedObject("deviceStatus");
        //deviceStatus["Switch"] = digitalRead(2) == LOW ? "P2-on" : "P2-off"; // 开关状态
        //deviceStatus["Upload_Data"] = digitalRead(5) == LOW ? "P5-on" : "P5-off";
        deviceStatus["设备名称"] = "我的ESP8266设备";
        deviceStatus["uptime"] = millis() / 1000; // 运行时间（秒）
        deviceStatus["freeHeap"] = ESP.getFreeHeap(); // 可用内存
    } else {
        responseDoc["message"] = message;
    }
    
    // 序列化响应
    String responsePayload;
    serializeJson(responseDoc, responsePayload);
    
    Serial.println("\r\n");
    Serial.println("开始发送设备属性设置响应...");
    //Serial.println(responsePayload);
    
    // 发布响应
    publish(PUB_set_reply_TOPIC, responsePayload.c_str()); // 发布响应
}
//MQTT消息回调函数
void MqttHandler::mqttCallback(char* topic, byte* payload, unsigned int length) {
    
    String topicStr = String(topic);
    String payloadStr = "";
    
    for (unsigned int i = 0; i < length && i < 100; i++) {
        payloadStr += (char)payload[i];
    }
    if (length > 100) {
        payloadStr += "...(消息过长，已截断)";
    }
    Serial.print("收到消息【");
    Serial.print(topicStr);
    Serial.print("】: ");
    Serial.println(payloadStr);
    /*根据不同的主题处理消息*/
    // 1.解析OneNET平台属性设置指令
    if (strcmp(topic, SUB_set_TOPIC) == 0) {
        handlePropertySetCommand(topicStr,payloadStr);
    }
    // 2.自定义的属性设置回调处理
    if (propertySetCallback) {
    propertySetCallback(topicStr,payloadStr);
}
}
//处理设备属性设置指令
void MqttHandler::handlePropertySetCommand(const String& topic, const String& payload) {

    Serial.println("\r\n");
    Serial.println("开始处理设备属性设置响应...");
    
    // 解析JSON请求
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    // 检查JSON解析错误
    if (error) {
        Serial.println("JSON解析失败: " + String(error.c_str()));
        // 发送错误响应
        sendPropertySetResponse("", 400, "JSON解析失败");
        return;
    }
    
    // 获取请求ID
    String requestId = doc["id"] | "";
    //先发送属性设置响应，表示已收到请求，防止超时    

    //检测参数错误
    if (!doc.containsKey("params")) {
        Serial.println("请求中缺少params参数");
        sendPropertySetResponse(requestId, 400, "缺少params参数");
        return;
    }
    // 发送标准响应
    sendPropertySetResponse(requestId, 200, "success");  

    // 处理每个属性设置
    JsonObject params = doc["params"];
    Serial.println("\r\n");
    Serial.println("本次设置了以下属性:");
    
    // 遍历并处理每个属性
    for (JsonPair kv : params) {
        String propertyName = String(kv.key().c_str());
        JsonVariant propertyValue = kv.value();
        
        Serial.print("  ");
        Serial.print(propertyName);
        Serial.print(" = ");
        // 使用通用的属性处理函数
        processPropertySetValue(propertyName, propertyValue);
    }//for循环结束
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
    // 默认的设备属性处理逻辑
    if (propertyName == "LED" || propertyName == "Switch") {
        // LED或开关控制
        bool state = (value == "on" || value == "true" || value == "1" || value == "HIGH");
        digitalWrite(2, state ? LOW:HIGH); // 
        Serial.print("标识符:"+propertyName+"，其设置值为: ");
        Serial.println(value);
    }
    // 添加其他设备属性的处理逻辑
    //else if {...}
    else {
        // 其他属性，
        Serial.print("未知属性: ");
        Serial.print(propertyName);
        Serial.print(" = ");
        Serial.println(value);
    }
}
//2.处理字符串类型属性
void MqttHandler::handleDeviceProperty_String(const String& propertyName, const String& value) {
    // 处理字符串类型属性（向STM32发送命令，自定义的数据包<CYZ:XXX:CYZ>）
    
    Serial.print("标识符:"+propertyName+"，其设置值为: ");
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
    // 处理浮点数类型属性（向STM32发送命令，自定义的数据包<CYZ:XXX:CYZ>）

    Serial.print("标识符:"+propertyName+"，其设置值为: ");
    Serial.println(value);
    //根据不同标识符进行不同处理
}

