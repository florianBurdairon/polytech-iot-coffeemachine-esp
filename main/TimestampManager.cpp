#include "TimestampManager.h"

TimestampManager& TimestampManager::getInstance(){
  static TimestampManager instance;
  return instance;
}

TimestampManager::TimestampManager(int updateInterval = 60000, const char* server1 = "pool.ntp.org"){
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, server1, 0, updateInterval); // Update interval set to 60 seconds
  this->timeclient = timeClient;
}

void TimestampManager::setConfigTime(int timezone=3600) {
  this->timeclient.begin();
  this->timeclient.setTimeOffset(timezone); //France is at GMT+1 so setTime to 3600
}
long TimestampManager::timestampGenerator() { //unsigned long ?
  while(!this->timeclient.update()){
    this->timeclient.forceUpdate();
  }
  Serial.print("formatted_date");
  Serial.println(this->timeclient.getFormattedDate());
  long epochTime = this->timeclient.getEpochTime();
  long currentMillis = millis() % 1000;
  Serial.print("Epoch Time: ");
  Serial.print(epochTime);
  Serial.print(".");
  Serial.println(currentMillis);
  return epochTime;//retourne un long qui est le temps en millisecondes écoule le 1er janvier 1970
}

TimestampManager ManagerTimestamp = TimestampManager::getInstance();

