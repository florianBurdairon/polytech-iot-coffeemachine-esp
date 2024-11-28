#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H
#include <EEPROM.h>
#include <Arduino.h>

class EEPROMManager{

  public:
    static EEPROMManager& getInstance();
    bool begin(int size_ssid=256,int size_password=256) const;
    void writeData(String ssid,String password) const;
    void getData(char &storedData1,char &storedData2) const;
    bool clearData(int EEPROM_size) const;
    
  private:
    EEPROMClass SSID;
    EEPROMClass PASSWORD;
    EEPROMManager();
};

extern EEPROMManager EEPROM;
#endif