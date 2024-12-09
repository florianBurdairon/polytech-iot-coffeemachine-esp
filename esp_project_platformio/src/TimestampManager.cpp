#include "HardwareSerial.h"
#include "TimestampManager.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);//

TimestampManager& TimestampManager::getInstance(){
  static TimestampManager instance;
  return instance;
}

void TimestampManager::begin() {
  timeClient.begin();
}
long TimestampManager::get() {
  if(!timeClient.update()){
    timeClient.forceUpdate();
  }
  long epochTime = timeClient.getEpochTime();
  return epochTime;
}

TimestampManager Timestamp = TimestampManager::getInstance();

