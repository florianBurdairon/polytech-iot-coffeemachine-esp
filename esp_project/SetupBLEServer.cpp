#include "SetupBLEServer.h"

String SetupBLEServer::serializeDeviceInfo(const String& name, const String& mac, const bool wifiReset) {
    DynamicJsonDocument jsonFile(256);
    String deviceInfo;
    jsonFile["name"] = name;
    jsonFile["mac"] = mac;
    if(wifiReset) {
      jsonFile["status"] = "RESET_WIFI";
    }
    else {
      jsonFile["status"] = "RESET";
    }
    jsonFile["lastOnline"] = 0;
    serializeJson(jsonFile, deviceInfo);
    return deviceInfo;
}

CustomCharacteristicCallbacks::CustomCharacteristicCallbacks(
    std::function<void(const String&, const String&)> callback)
    : onCredentialsReceived(callback) {}

void CustomCharacteristicCallbacks::onWrite(BLECharacteristic* characteristic) {
    Serial.println("onWrite callback");
    String value = characteristic->getValue().c_str();
    if (characteristic->getUUID().toString() == WIFI_CREDENTIAL_UUID) {
        DynamicJsonDocument jsonFile(256);
        deserializeJson(jsonFile, value.c_str());
        const String receivedSSID = jsonFile["ssid"];
        const String receivedPassword = jsonFile["password"];
        onCredentialsReceived(receivedSSID, receivedPassword);
    }
}

void SetupBLEServer::start(const String& mac, const String& deviceName, const bool wifiReset, std::function<void(const String&, const String&)> callback) {
    Serial.println("Start server");
    BLEDevice::init(deviceName);
    BLEServer* server = BLEDevice::createServer();

    BLEService* service = server->createService(SERVICE_UUID);

    // Characterisitc for wifi credential (READ/WRITE)
    BLECharacteristic* characteristicWifiSetup = service->createCharacteristic(
        WIFI_CREDENTIAL_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    characteristicWifiSetup->setCallbacks(new CustomCharacteristicCallbacks(callback));

    // Characteristic for device info (READ)
    String deviceInfo = serializeDeviceInfo(deviceName, mac, wifiReset);
    Serial.println(deviceInfo);
    BLECharacteristic* characteristicDeviceInfo = service->createCharacteristic(
        DEVICE_INFO_UUID,
        BLECharacteristic::PROPERTY_READ);
    characteristicDeviceInfo->setValue((uint8_t*)deviceInfo.c_str(), deviceInfo.length());

    service->start();
    BLEAdvertising* advertising = server->getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->start();
}
