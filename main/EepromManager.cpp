#include "EEPROMManager.h"

EEPROMManager& EEPROMManager::getInstance() {
    static EEPROMManager instance;
    return instance;
}

EEPROMManager::EEPROMManager(){
  this->SSID = EEPROMClass SSID("wifi_ssid");
  this->PASSWORD = EEPROMClass PASSWORD("wifi_password");
}

bool EEPROMManager::begin(int size_ssid=256,int size_password=256) const{
  return this->SSID.begin(size_ssid) && this->PASSWORD.begin(size_password);
}

void EEPROMManager::writeData(String ssid,String password) const{
  this->SSID.writeString(0,ssid.c_str());
  this->SSID.commit();
  this->PASSWORD.writeString(0,password.c_str());
  this->PASSWORD.commit();
}

void EEPROMManager::getData(char &storedData1,char &storedData2) const{
  this->SSID.get(0,storedData1);
  this->PASSWORD.get(0,storedData2);
}

bool EEPROMManager::clearData(int EEPROM_size) const{
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

EEPROMManager EEPROM = EEPROMManager::getInstance();