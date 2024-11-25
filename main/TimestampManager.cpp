#include "TimestampManager.h"

void TimestampManager::setConfigTime(long gmtOffset_sec = 0, int daylightOffset_sec = 0, const char* server1 = "pool.ntp.org") {
    configTime(gmtOffset_sec, daylightOffset_sec, server1);
}

String TimestampManager::timestampGenerator() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return "";
    }
    char timestampmonth[10];
    strftime(timestampmonth,10,"%B",&timeinfo);
    return String(timeinfo.tm_hour) +":"
            + String(timeinfo.tm_min) +":"
            + String(timeinfo.tm_sec)+"|"
            + String(timeinfo.tm_mday)+"/"
            + String(timestampmonth)+"/"
            + String(timeinfo.tm_year + 1900) ;
}




