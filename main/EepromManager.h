#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H
#include <EEPROM.h>
#include <Arduino.h>

class EepromManager{
    public:
        bool begin(int size_memory);
        void write(String data);
        String get(char storedValue);
    private:
        EEPROMClass eepromClass;
};

#endif