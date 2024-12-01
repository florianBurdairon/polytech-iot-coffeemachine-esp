#include "SetupBLEServer.h"

String SetupBLEServer::serializeDeviceInfo(const String& name, const String& mac, const bool isWifiReset) {
    DynamicJsonDocument jsonFile(256);
    String deviceInfo;
    jsonFile["name"] = name;
    jsonFile["mac"] = mac;
    if (isWifiReset)
    {
        jsonFile["status"] = "RESET_WIFI";
    }
    else
    {
        jsonFile["status"] = "RESET";
    }
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

void SetupBLEServer::start(const String& mac, const String& deviceName, const bool isWifiReset, std::function<void(const String&, const String&)> callback) {
    Serial.println("Start server");
    BLEDevice::init(deviceName);//std::string(deviceName.c_str())
    BLEServer* server = BLEDevice::createServer();

    BLEService* service = server->createService(SERVICE_UUID);

    BLECharacteristic* characteristicWifiSetup = service->createCharacteristic(
        WIFI_CREDENTIAL_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    BLECharacteristic* characteristicDeviceInfo = service->createCharacteristic(
        DEVICE_INFO_UUID,
        BLECharacteristic::PROPERTY_READ);

    characteristicWifiSetup->setCallbacks(new CustomCharacteristicCallbacks(callback));

    String deviceInfo = serializeDeviceInfo(deviceName, mac, isWifiReset);
    Serial.println(deviceInfo);
    characteristicDeviceInfo->setValue((uint8_t*)deviceInfo.c_str(), deviceInfo.length());

    service->start();
    BLEAdvertising* advertising = server->getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->start();
}
