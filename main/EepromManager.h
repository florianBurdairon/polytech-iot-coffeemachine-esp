#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H
#include <EEPROM.h>
#include <Arduino.h>

class EEPROMManager{

  public:
    EEPROMManager(EEPROMClass eepromClass1,EEPROMClass eepromClass2);
    bool beginEEPROMClasses(int size_memory1=256,int size_memory2=256);
    void writeAndCommit(String receivedData1,String receivedData2);
    void getEEPROMData(char &storedData1,char &storedData2);
    bool clearEEPROM(int EEPROM_size);
    
  private:
    EEPROMClass SSID;
    EEPROMClass PASSWORD;
};
#endif