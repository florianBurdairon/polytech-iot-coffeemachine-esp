#ifndef SETUP_BLE_SERVER_H
#define SETUP_BLE_SERVER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ArduinoJson.h>
#include <functional>
#include <string>

// UUIDs for BLE service and characteristics
#define SERVICE_UUID "e29834b1-fdbe-4780-84f2-f3a87d0e97fb"
#define WIFI_CREDENTIAL_UUID "dddbf1dc-63f8-4146-bbee-523b47143be8"
#define DEVICE_INFO_UUID "f5609179-c8f5-49b9-8f88-f4b86ed493e4"

class SetupBLEServer {
public:
    static void start(const String& mac, const String& deviceName, const bool wifiReset, std::function<void(const String&, const String&)> callback);

private:
    static String serializeDeviceInfo(const String& name, const String& mac, const bool wifiReset);
};

class CustomCharacteristicCallbacks : public BLECharacteristicCallbacks {
public:
    CustomCharacteristicCallbacks(std::function<void(const String&, const String&)> callback);
    void onWrite(BLECharacteristic* characteristic) override;

private:
    std::function<void(const String&, const String&)> onCredentialsReceived;
};

#endif // SETUP_BLE_SERVER_H
