#include "EEPROMManager.h"

void writeStringToEEPROM(int startAddress, const String& value) {
  int len = value.length();
  EEPROM.write(startAddress, len);  // Save string length at the start
  for (int i = 0; i < len; i++) {
    EEPROM.write(startAddress + 1 + i, value[i]);  // Write string character by character
  }
}

String readStringFromEEPROM(int startAddress) {
  int len = EEPROM.read(startAddress);  // Read string length
  char data[len + 1];
  for (int i = 0; i < len; i++) {
    data[i] = EEPROM.read(startAddress + 1 + i);  // Read character by character
  }
  data[len] = '\0';  // Null-terminate the string
  return String(data);
}

bool areStringsSaved() {
  char marker[sizeof(STRING_MARKER)];
  for (int i = 0; i < sizeof(marker); i++) {
    marker[i] = EEPROM.read(START_ADDRESS + i);
  }
  return strcmp(marker, STRING_MARKER) == 0;
}

void saveStringsToEEPROM(const String& str1, const String& str2) {
  for (int i = 0; i < sizeof(STRING_MARKER); i++) {
    EEPROM.write(START_ADDRESS + i, STRING_MARKER[i]);  // Write the marker
  }
  writeStringToEEPROM(START_ADDRESS + sizeof(STRING_MARKER), str1);  // Save first string
  writeStringToEEPROM(START_ADDRESS + sizeof(STRING_MARKER) + 1 + MAX_STRING_LENGTH, str2);  // Save second string
  
  EEPROM.commit();  // Save changes to EEPROM
}

void clearEEPROM() {
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);  // Write 0 to each byte
  }
  EEPROM.commit();  // Ensure changes are saved
  Serial.println("EEPROM cleared.");
}