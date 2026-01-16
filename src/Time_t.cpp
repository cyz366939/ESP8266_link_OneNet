#include "TIME_T.h"


extern ESP8266WiFiMulti wifiMulti;
// 内部使用的变量
static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP);
static bool timeSynced = false;

// 初始化
void SimpleTime::begin() {
    // 设置东8区（北京时间）
    timeClient.setTimeOffset(0 * 3600);
    timeClient.begin();
    
    Serial.println("时间模块初始化完成");
}

// 获取当前时间戳（秒）
unsigned long SimpleTime::getTimestamp() {
    // 检查WiFi连接
    if (wifiMulti.run() != WL_CONNECTED) {
        return 0;
    }
    
    // 如果还没同步过时间，先同步
    if (!timeSynced) {
        if (timeClient.forceUpdate()) {
            timeSynced = true;
        } else {
            return 0;
        }
    }
    
    // 更新时间并获取时间戳
    timeClient.update();
    return timeClient.getEpochTime();
}

// 获取当前时间戳（毫秒）
unsigned long long SimpleTime::getTimestampMillis() {
    unsigned long timestamp = getTimestamp();
    if (timestamp == 0) {
        return 0;
    }
    return (unsigned long long)timestamp * 1000;
}

// 获取格式化的时间 HH:MM:SS
String SimpleTime::getTimeString() {
    if (getTimestamp() == 0) {
        return "00:00:00";
    }
    return timeClient.getFormattedTime();
}

// 获取格式化的日期时间
String SimpleTime::getDateTimeString() {
    unsigned long timestamp = getTimestamp();
    if (timestamp == 0) {
        return "1970-01-01 00:00:00";
    }
    
    // 计算年月日时分秒
    time_t rawtime = timestamp;
    struct tm* timeinfo = localtime(&rawtime);
    
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             timeinfo->tm_year + 1900,
             timeinfo->tm_mon + 1,
             timeinfo->tm_mday,
             timeinfo->tm_hour,
             timeinfo->tm_min,
             timeinfo->tm_sec);
    
    return String(buffer);
}

// 检查是否已同步时间
bool SimpleTime::isTimeSynced() {
    return timeSynced && (wifiMulti.run() == WL_CONNECTED);
}

