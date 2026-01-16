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
};

#endif