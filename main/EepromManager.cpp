#include "EepromManager.h"

class EepromManager(){
    public:
        EepromManager(EEPROMClass eepromClass){
          this->eepromClass = eepromClass;
        }
        bool begin(int size_memory){
          return eepromClass.begin(size_memory);
        };

        void write(String data){
          const char *content = data.c_str();
          eepromClass.writeString(0, content);
          eepromClass.commit();
        };

        String get(char storedValue){
          return String(eepromClass.get(0,storedValue));
        };
    
    private:
        EEPROMClass eepromClass;
}