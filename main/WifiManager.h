#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
  public :
    String mac;
    static WifiManager& getInstance();
    //~WifiManager();
    void beginWifi(char ssid[], char password[]);
    bool checkConnected();
  private :
    WifiManager();
};

extern WifiManager ManagerWifi;
#endif