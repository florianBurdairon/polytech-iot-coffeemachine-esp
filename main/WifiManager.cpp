#include "WifiManager.h"
#include <WiFi.h>

class WifiManager {
    public :
        String mac = WiFi.macAddress();
        String ssid;
        String password;

        WifiManager(String ssid, String password) {
            this->ssid = ssid;
            this->password = password;

            WiFi.mode(WIFI_STA);
        }

        ~WifiManager() {
            WiFi.disconnect();
        }

        void begin() {
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
            }
        }
}