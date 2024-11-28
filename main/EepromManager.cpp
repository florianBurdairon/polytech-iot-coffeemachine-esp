#include "EEPROMManager.h"

EEPROMManager& EEPROMManager::getInstance() {
    static EEPROMManager instance;
    return instance;
}

EEPROMManager::EEPROMManager(){
  this->SSID = EEPROMClass("wifi_ssid");
  this->PASSWORD = EEPROMClass("wifi_password");
}

bool EEPROMManager::begin(int size_ssid,int size_password) {
  return SSID.begin(size_ssid) && PASSWORD.begin(size_password);
}

void EEPROMManager::writeData(String ssid,String password) {
  SSID.writeString(0,ssid.c_str());
  SSID.commit();
  PASSWORD.writeString(0,password.c_str());
  PASSWORD.commit();
}

void EEPROMManager::getData(char storedData1[],char storedData2[]) { //char &storedData1
  SSID.get(0,storedData1);
  PASSWORD.get(0,storedData2);
}

bool EEPROMManager::clearData(int EEPROM_size) {
  if(!EEPROM.begin(EEPROM_size)){
    Serial.println("Echec to begin eeprom");
    return false;
  }
  else{
    for(int i=0; i< EEPROM_size; i++){
      EEPROM.write(i, 0);
      Serial.printf("EEPROM %i reset",i);
    }
    EEPROM.commit();
    Serial.println("EEPROM reset");
  }
  return true;
}

EEPROMManager ManagerEEPROM = EEPROMManager::getInstance();