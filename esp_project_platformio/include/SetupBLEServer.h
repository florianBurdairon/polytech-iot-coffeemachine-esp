#ifndef SETUP_BLE_SERVER_H
#define SETUP_BLE_SERVER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ArduinoJson.h>
#include <functional>
#include <string>
#include "secrets.h"

class SetupBLEServer {
public:
    static void start(const String& mac, const String& deviceName, const bool isWifiReset, std::function<void(const String&, const String&)> callback);

private:
    static String serializeDeviceInfo(const String& name, const String& mac, const bool isWifiReset);
};

class CustomCharacteristicCallbacks : public BLECharacteristicCallbacks {
public:
    CustomCharacteristicCallbacks(std::function<void(const String&, const String&)> callback);
    void onWrite(BLECharacteristic* characteristic) override;

private:
    std::function<void(const String&, const String&)> onCredentialsReceived;
};

#endif // SETUP_BLE_SERVER_H
