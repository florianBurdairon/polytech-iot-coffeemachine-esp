#include <EEPROM.h>

#define EEPROM_SIZE 512            // Define EEPROM size
#define START_ADDRESS 0            // Starting address in EEPROM
#define STRING_MARKER "SAVED"      // Marker to check if strings are saved
#define MAX_STRING_LENGTH 100      // Maximum length of each string


String readStringFromEEPROM(int startAddress);
void writeStringToEEPROM(int startAddress, const String& value);
bool areStringsSaved();
void saveStringsToEEPROM(const String& str1, const String& str2);
void clearEEPROM();