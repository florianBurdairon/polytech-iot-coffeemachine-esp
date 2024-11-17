#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "EepromManager.h"

class BluetoothManager{
    public:
        String name;

        BluetoothManager(String name);
        ~BluetoothManager();

        void startBLE(EEPROMClass ssid, EEPROMClass password);//EepromManager ssid, EepromManager password
        void stopBLE();
        void sendBLE(String message);

    private:
        bool isReading;

        void taskRead(EEPROMClass ssid, EEPROMClass password);
};

#endif // BLUETOOTHMANAGER_H