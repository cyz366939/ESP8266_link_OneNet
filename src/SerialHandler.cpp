#include <SerialHandler.h>

// 前向声明
extern ESP8266WiFiMulti wifiMulti;
//构造函数
SerialHandler::SerialHandler() {
    serialBuffer = "";
    currentState = NORMAL_MODE;
    uploadStartTime = 0;
    mqttHandler = nullptr;
}
//初始化串口
void SerialHandler::init() {
    Serial.begin(SERIAL_BAUD);
    delay(300);
    Serial.println("串口初始化完成");
    clearDataBuffer();
}
//读取串口数据
void SerialHandler::readSerialData() {
    if (Serial.available()) {
        while (Serial.available()) {
            char c = Serial.read();
            serialBuffer += c;
            
            // 检查是否收到完整指令
            if (c == '\n' || c == '\r') {
                if (serialBuffer.length() > 1) {
                    // 根据当前状态处理数据
                    if (currentState == UPLOAD_DATA_MODE) {
                        String trimmedData = serialBuffer;
                        trimmedData.trim();
                        
                        // 检查是否是END或CANCEL命令
                        if (trimmedData.equalsIgnoreCase("END")) {
                            processEndCommand();
                        } else if (trimmedData.equalsIgnoreCase("CANCEL")) {
                            processCancelCommand();
                        } else {
                            // 处理键值对数据
                            processKeyValueData(trimmedData);
                        }
                    } else {
                        // 正常模式处理普通指令
                        processSerialCommand(serialBuffer);
                    }
                }
                serialBuffer = "";
            }
        }
    }
}
//处理串口命令
void SerialHandler::processSerialCommand(String command) {
    command.trim();// 去除前后空格
    
    if (command == "UPLOAD_DATA") {
        Serial.println("处理指令: " + command);
        processUploadDataCommand();
        
    } else if (command.equals("GET_TIME")) {
        // 返回当前时间戳
        Serial.println("time:" + String(SimpleTime::getTimestamp()));
    } else if (command == "STATUS") {
        Serial.println("处理指令: " + command);
        String status = "WiFi:" + String(isWiFiConnected() ? "OK" : "FAIL") + 
                       ",DataBuffer:" + String(dataBuffer.size());
        Serial.println(status);
    } else if (command == "HELP") {
        Serial.println("处理指令: " + command);
        Serial.println("支持的指令:");
        Serial.println("  UPLOAD_DATA - 进入数据上传模式");
        Serial.println("  GET_TIME - 获取当前时间戳");
        Serial.println("  STATUS - 获取状态");
        Serial.println("  HELP - 显示帮助");
        Serial.println("\n数据上传模式下:");
        Serial.println("  key=value 或 key:value - 添加键值对数据");
        Serial.println("  END - 结束数据上传并发送到OneNET");
        Serial.println("  CANCEL - 取消数据上传");
    }
    else {
        Serial.println("处理指令: " + command);
        Serial.println("未知指令: " + command);
    }
}
//处理数据上传命令
void SerialHandler::processUploadDataCommand() {
    Serial.println("进入数据上传模式...");
    Serial.println("请输入键值对数据 (格式: key=value 或 key:value)");
    Serial.println("输入 'END' 完成输入并自动上传数据，输入 'CANCEL' 取消上传");
    
    currentState = UPLOAD_DATA_MODE;
    uploadStartTime = millis();
    clearDataBuffer();
}
//格式验证函数
bool SerialHandler::validateKeyValueFormat(const String& data, String& key, String& value, char& separator) {
    // 查找分隔符位置
    int equalPos = data.indexOf('=');
    int colonPos = data.indexOf(':');
    
    // 确定使用哪个分隔符（优先使用'='）
    if (equalPos > 0) {
        separator = '=';
        key = data.substring(0, equalPos);
        value = data.substring(equalPos + 1);
    } else if (colonPos > 0) {
        separator = ':';
        key = data.substring(0, colonPos);
        value = data.substring(colonPos + 1);
    } else {
        return false; // 未找到有效分隔符
    }
    
    // 清理键值中的空白字符
    key.trim();
    value.trim();
    
    // 验证键和值的有效性
    if (key.length() == 0) {
        Serial.println("错误: 键不能为空");
        return false;
    }
    
    // 检查键中是否包含无效字符
    for (unsigned int i = 0; i < key.length(); i++) {
        char c = key.charAt(i);
        if (!(isalnum(c) || c == '_' || c == '-' || c == '.')) {
            Serial.println("错误: 键包含无效字符 '" + String(c) + "'");
            return false;
        }
    }
    
    return true;
}
//键值对数据处理
void SerialHandler::processKeyValueData(const String& data) {
    String key, value;
    char separator;
    
    // 验证格式
    if (!validateKeyValueFormat(data, key, value, separator)) {
        Serial.println("格式错误，请使用格式: key=value 或 key:value");
        return;
    }
    
    // 添加到数据缓冲区
    KeyValueData kvData;
    kvData.key = key;
    kvData.value = value;
    kvData.isValid = true;// 标记为有效数据
    dataBuffer.push_back(kvData);
    
    Serial.println("已添加: " + key + " " + String(separator) + " " + value + 
                  " (总计: " + String(dataBuffer.size()) + " 条数据)");
}
//结束命令处理并上传数据
void SerialHandler::processEndCommand() {
    Serial.println("\n结束数据上传模式");
    Serial.println("接收到 " + String(dataBuffer.size()) + " 条键值对数据");
    
    if (dataBuffer.empty()) {
        Serial.println("没有数据需要上传");
    } else {
        // 生成JSON格式数据
        String jsonPayload = generateJsonPayload();
        Serial.println("生成JSON数据: " + jsonPayload);

        /****************发布数据到OneNET************/
        // 首先检查指针是否有效
        if (mqttHandler == nullptr) {
            Serial.println("错误: MQTT处理器未初始化!");
            return;
        }
        // 检查数据是否为空
        if (jsonPayload.length() == 0) {
            Serial.println("警告: 要发布的数据为空!");
            return;
        }
        // 发布数据
        if(mqttHandler->publish(PUB_post_TOPIC, jsonPayload.c_str())) {
            Serial.println("成功上传到OneNET"); // 发布成功
        }else {
            Serial.println("上传到OneNET失败"); // 发布失败
        }           
    
    // 返回正常模式
    currentState = NORMAL_MODE;
    }//else结束
}
//取消命令处理
void SerialHandler::processCancelCommand() {
    Serial.println("\n取消数据上传模式");
    Serial.println("已清除 " + String(dataBuffer.size()) + " 条未上传的数据");
    
    clearDataBuffer();  // 清空数据缓冲区
    currentState = NORMAL_MODE;
    
    Serial.println("已返回正常模式");
}
// 生成符合OneNET格式的JSON数据
String SerialHandler::generateJsonPayload() const {
    JsonDocument doc;
    
    // OneNET标准格式
    doc["id"] = String(millis());                              // 消息ID
    doc["version"] = "1.0";                          // 固定版本号
    
    // 创建params对象
    JsonObject params = doc.createNestedObject("params");
    
    // 添加键值对数据到params，符合OneNET格式
    //遍历数据缓冲区，添加有效数据到params对象中
    for (const auto& kv : dataBuffer) {
        if (kv.isValid) {
            JsonObject paramValue = params[kv.key].to<JsonObject>();
        // 检查是否为整数
        if (isInteger(kv.value)) {
            paramValue["value"] = kv.value.toInt();
        }
        // 检查是否为浮点数
        else if (isFloat(kv.value)) {
            paramValue["value"] = kv.value.toFloat();
        }
        else {
            paramValue["value"] = kv.value;
        }
     }//if(kv.isValid)结束
  }//for循环结束
    String jsonStr;
    serializeJson(doc, jsonStr);// 序列化为JSON字符串
    return jsonStr;
}//generateJsonPayload结束
// 清空数据缓冲区
void SerialHandler::clearDataBuffer() {
    dataBuffer.clear();
    uploadStartTime = 0;
}
// 检查WiFi连接状态
bool SerialHandler::isWiFiConnected() {
    return (wifiMulti.run() == WL_CONNECTED);
}
// 判断字符串是否为整数
bool SerialHandler::isInteger(const String& value) const {
    for (unsigned int j = 0; j < value.length(); j++) {
        if (!isdigit(value.charAt(j)) && value.charAt(j) != '-') {
            return false;
        }
    }
    return value.length() > 0;
}
// 判断字符串是否为浮点数
bool SerialHandler::isFloat(const String& value) const {
    int dotCount = 0;
    for (unsigned int j = 0; j < value.length(); j++) {
        char c = value.charAt(j);
        if (c == '.') {
            dotCount++;
        } else if (!isdigit(c) && c != '-') {
            return false;
        }
    }
    return dotCount == 1 && value.length() > 1;
}

