#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H
#include <EEPROM.h>
#include <Arduino.h>

class EEPROMManager{

  public:
    EEPROMClass SSID;
    EEPROMClass PASSWORD;
    static EEPROMManager& getInstance();
    bool begin(int size_ssid,int size_password) ;
    void writeData(String ssid,String password) ;
    void getData(char storedData1[],char storedData2[]) ;
    bool clearData(int EEPROM_size) ;
    
  private:
    //EEPROMClass SSID;
    EEPROMManager();
};

extern EEPROMManager ManagerEEPROM;
#endif