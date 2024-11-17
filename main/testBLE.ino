#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
//#include <Arduino.h>
//#include <WiFi.h>

//exemple simple mais qui permet quand même de set les 2 valeurs d'eeprom
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc" //12345678-1234-1234-1234-123456789abc
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321" //87654321-4321-4321-4321-cba987654321

EEPROMClass SSID("wifi_ssid");
EEPROMClass PASSWORD("wifi_password");
char storedSSID [32];
char storedPassword [64];

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String value = pCharacteristic->getValue();
      if (value.length() > 0) {
        const char* delimiter = ",";
        SSID.writeString(0,value.substring(0, value.indexOf(delimiter,0)).c_str());
        PASSWORD.writeString(0,value.substring(value.indexOf(delimiter,0)+1).c_str());
        Serial.println("writeEEprom finishes \n");
        SSID.get(0,storedSSID);//copy only the value stored in eeprom
        printf("storedSSID= %s\n",String(storedSSID));
        PASSWORD.get(0, storedPassword);
        printf("storedPassword= %s\n",String(storedPassword));
        pCharacteristic->setValue("values ssid and password stored in eeprom");
        // BLEDevice::deinit(true);  
        // Serial.println("BLE déconnecté");
      }
    }
};

void setup() {
  Serial.begin(115200);
  SSID.begin(0x100);
  PASSWORD.begin(0x200);
  BLEDevice::init("CoffeeMachine_ESP32BLE");
  BLEServer *pServer = BLEDevice::createServer();

  // Create a BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE characteristic
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(storedSSID, storedPassword);
  // while (WiFi.status() != WL_CONNECTED) {
  //     delay(500);
  // }
  // Serial.println("WiFi connected.");

}

void loop() {
}
