#ifndef _TIME_T_H
#define _TIME_T_H

#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class SimpleTime {
public:
    // 初始化（在setup中调用）
    static void begin();

    // 非阻塞更新时间（需在loop中调用）
    static void update();

    // 获取当前时间戳（秒）
    static unsigned long getTimestamp();

    // 获取当前时间戳（毫秒）
    static unsigned long long getTimestampMillis();

    // 获取格式化的时间 HH:MM:SS
    static String getTimeString();

    // 获取格式化的日期 YYYY-MM-DD HH:MM:SS
    static String getDateTimeString();

    // 检查是否已同步时间
    static bool isTimeSynced();

    // 获取时间同步状态
    static bool getSyncSuccess() { return timeSynced; }

private:
    static unsigned long lastNTPUpdate;  // 上次NTP更新时间
    static bool timeSynced;               // 时间同步状态
    static const unsigned long NTP_UPDATE_INTERVAL; // 每小时更新一次
};

#endif