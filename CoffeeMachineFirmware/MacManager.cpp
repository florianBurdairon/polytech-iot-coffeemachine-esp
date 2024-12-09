#include "MacManager.h"

String getDefaultMacAddress() {
  String mac = "";
  unsigned char mac_base[6] = {0};
  if (esp_efuse_mac_get_default(mac_base) == ESP_OK) {
    char buffer[18];  // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    mac = buffer;
  }
  return mac;
}

String getInterfaceMacAddress(esp_mac_type_t interface) {
  String mac = "";
  unsigned char mac_base[6] = {0};
  if (esp_read_mac(mac_base, interface) == ESP_OK) {
    char buffer[18];  // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    mac = buffer;
  }
  return mac;
}