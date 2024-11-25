#ifndef TIMESTAMPMANAGER_H
#define TIMESTAMPMANAGER_H
#include <Arduino.h>
#include "time.h"

class TimestampManager {
    public :
        void setConfigTime(long gmtOffset_sec, int daylightOffset_sec, const char* server1);
        String timestampGenerator();
};
#endif