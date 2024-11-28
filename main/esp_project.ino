#include "SetupBLEServer.h"
#include "EEPROMManager.h"
#include "WifiManager.h"

// Callback to handle credentials
void handleCredentials(const String ssid, const String password) {
  Serial.printf("SSID: %s ;\n Password: %s ;\n ",ssid,password);
  ManagerEEPROM.writeData(ssid, password); //EEPROMManager is the singleton of EEPROMManager define in the .h
  Serial.println("End of write in EEprom");
  ESP.restart();
}

char storedSSID [64];
char storedPassword [64];

void setup() {
  Serial.begin(115200);
  //ManagerEEPROM.clearData(512);  
  Serial.println("START ESP");  

  if (!ManagerEEPROM.begin(256,256)) { 
    Serial.println("Échec de l'initialisation de l'EEPROM SSID et PSSWD"); 
    ESP.restart();
    return; 
  } 
  Serial.println("EEPROM initialisée");
  ManagerEEPROM.getData(storedSSID,storedPassword);
  Serial.printf("storedSSID : %c\n storedPassword : %c\n", storedSSID, storedPassword);
  
  if (strcmp(storedSSID,0) == 0 && strcmp(storedPassword,0) == 0){
    //1st setup so start with BLE
    Serial.println("1st setup so start with BLE");
    SetupBLEServer::start(ManagerWifi.mac,handleCredentials);
    //delay(10000);//check until 10sec
    //if (strcmp(storedSSID,"") != 0 && strcmp(storedPassword,"") != 0){
    //  SetupBLEServer::stop();
    //}
    //else{
    //  SetupBLEServer::start(ManagerWifi.mac,handleCredentials);
    //}
  }
  else{
    //2nd setup so start with Wifi
    Serial.println("2nd setup so start with Wifi");
    ManagerWifi.beginWifi(storedSSID,storedPassword);//could not convert 'storedPassword' from 'char* [64]' to 'const String&'
    if(!ManagerWifi.checkConnected()){
      SetupBLEServer::start(ManagerWifi.mac,handleCredentials);
    }
  }
}

void loop() {
    // Add any periodic task here
    //case if Wifi connection is lost
    // if(!ManagerWifi.checkConnected()){
    //   ManagerWifi.beginWifi(storedSSID,storedPassword);
    //   //ESP.restart();
    // }
    
}
