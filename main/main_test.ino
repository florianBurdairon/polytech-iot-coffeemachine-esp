#include "BluetoothManager.h"
//#include "WifiManager.h"
#include "EepromManager.h"
#include <EEPROM.h>


char storedSSID [32];
char storedPassword [64];

void setup() {
  EEPROMClass SSID("wifi_ssid");
  EEPROMClass PASSWORD("wifi_password");
  //EepromManager ssid(SSID); 
  //EepromManager password(PASSWORD);
  Serial.begin(115200);
  
  // if (!ssid.begin(0x100) & !password.begin(0x200)) { // in begin you specify the amount of storage for this class ; in ESP32 EEPROM is of 512 bytes
  //   Serial.println("Failed to initialise EEPROMClass SSID & PASSWORD"); 
  //   return; 
  // }
  BluetoothManager bluetooth("CoffeeMachine_ESP32BLE");
  bluetooth.startBLE(SSID,PASSWORD);//ssid,password
  bluetooth.stopBLE();




}
 void loop() {
  // put your main code here, to run repeatedly:
  //begin the connection to ESP32 with Bluetooth to store SSID and password of the Wifi network
  
  
  //Then these values are obtained we can switch to Wifi connection
  //disconnect bluetooth
  
  //launch Wifi connection
  //WifiManager wifi(ssid.get(storedSSID),password.get(storedPassword));
  //wifi.begin();
 }