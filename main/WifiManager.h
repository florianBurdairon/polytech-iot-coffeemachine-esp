#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include <Arduino.h>


class WifiManager {
    public :
        String mac;
        String ssid;
        String password;
        WifiManager(String ssid, String password);
        ~WifiManager();
        void begin();
}
#endif