#ifndef TIMESTAMPMANAGER_H
#define TIMESTAMPMANAGER_H
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

class TimestampManager {
  public :
    static TimestampManager& getInstance();
    void begin();
    long get();
  private :
    TimestampManager();
};

extern TimestampManager Timestamp;
#endif