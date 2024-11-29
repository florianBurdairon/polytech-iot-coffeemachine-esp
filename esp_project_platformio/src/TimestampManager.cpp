#include "TimestampManager.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

TimestampManager& TimestampManager::getInstance(){
  static TimestampManager instance;
  return instance;
}

TimestampManager::TimestampManager(){
  int updateInterval = 10000; 
  const char* server1 = "pool.ntp.org";
}

void TimestampManager::begin() {
  timeClient.begin();
}
long TimestampManager::get() {
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }
  long epochTime = timeClient.getEpochTime();
  return epochTime;
}

TimestampManager Timestamp = TimestampManager::getInstance();

