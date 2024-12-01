#include "WifiManager.h"

WifiManager& WifiManager::getInstance() {
    static WifiManager instance;
    return instance;
}

WifiManager::WifiManager() {
  this->mac = WiFi.macAddress();
  WiFi.mode(WIFI_STA);
}

void WifiManager::reconnectWifi(){
  int countDelay = 0;
  while(!checkConnected() || countDelay < 5 ){
    WiFi.reconnect();
    delay(1000);
    countDelay++;
  }
  if(countDelay==5){
    WiFi.disconnect();
  }
}

void WifiManager::beginWifi(char ssid[], char password[]) {
  WiFi.begin(ssid, password);
  int stepBeforeResetBLE = 0;
  while (WiFi.status() != WL_CONNECTED && stepBeforeResetBLE!=6) { // let 6s for being connected to the Wifi station
    delay(1000); 
    stepBeforeResetBLE++;
  }
}

bool WifiManager::checkConnected(){
  return WiFi.status() == WL_CONNECTED ;
}

WifiManager ManagerWifi = WifiManager::getInstance();