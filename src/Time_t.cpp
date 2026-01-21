#include "TIME_T.h"


extern ESP8266WiFiMulti wifiMulti;
// 内部使用的变量
static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP);
bool SimpleTime::timeSynced = false;
unsigned long SimpleTime::lastNTPUpdate = 0;
const unsigned long SimpleTime::NTP_UPDATE_INTERVAL = 3600000;

// 初始化
void SimpleTime::begin() {
    timeClient.setTimeOffset(8 * 3600);
    timeClient.begin();

    Serial.println("时间模块初始化完成");
}

// 非阻塞更新时间（需在loop中调用）
void SimpleTime::update() {
    unsigned long currentTime = millis();

    // 每小时更新一次NTP时间
    if (currentTime - lastNTPUpdate >= NTP_UPDATE_INTERVAL) {
        lastNTPUpdate = currentTime;
        if (wifiMulti.run() == WL_CONNECTED) {
            timeClient.update();
            if (!timeSynced) {
                // 首次同步时尝试更新
                timeClient.forceUpdate();
                timeSynced = true;
            }
        }
    }

    // 常规时间更新（非阻塞）
    timeClient.update();
}

// 获取当前时间戳（秒）
unsigned long SimpleTime::getTimestamp() {
    // 检查WiFi连接
    if (wifiMulti.run() != WL_CONNECTED) {
        return 0;
    }

    // 返回当前时间戳
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

