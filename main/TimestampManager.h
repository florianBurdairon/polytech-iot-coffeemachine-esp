#ifndef TIMESTAMPMANAGER_H
#define TIMESTAMPMANAGER_H
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class TimestampManager {
  public :
    static TimestampManager& getInstance();
    void setConfigTime(int timezone);
    long timestampGenerator();
  private :
    TimestampManager();
};

extern TimestampManager ManagerTimestamp;
#endif