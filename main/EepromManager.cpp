#include "EEPROMManager.h"

EEPROMManager::EEPROMManager(EEPROMClass eepromClass1,EEPROMClass eepromClass2){
  this->SSID = eepromClass1;
  this->PASSWORD = eepromClass2;
}

bool EEPROMManager::beginEEPROMClasses(int size_memory1=256,int size_memory2=256){
  return this->SSID.begin(size_memory1) && this->PASSWORD.begin(size_memory2);
}

void EEPROMManager::writeAndCommit(String receivedData1,String receivedData2){
  this->SSID.writeString(0,receivedData1.c_str());
  this->SSID.commit();
  this->PASSWORD.writeString(0,receivedData2.c_str());
  this->PASSWORD.commit();
}

void EEPROMManager::getEEPROMData(char &storedData1,char &storedData2){
  this->SSID.get(0,storedData1);
  this->PASSWORD.get(0,storedData2);
}

bool EEPROMManager::clearEEPROM(int EEPROM_size){
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

