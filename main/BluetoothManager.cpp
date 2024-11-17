#include "BluetoothManager.h"
#include "secrets.h"

class BluetoothManager{
    public:
        String name;

        BluetoothManager(String name){
            this->name = name;
            BLEServer *pServer;
            BLECharacteristic *pCharacteristic;
            BLEService *pService;
            BLEAdvertising *pAdvertising;
            this->pServer = *pServer;
            this->pCharacteristic = *pCharacteristic;
            this->pService = *pService;
            this->pAdvertising = *pAdvertising;
        }

        void startBLE(EEPROMClass ssid, EEPROMClass password){//EepromManager ssid, EepromManager password
            BLEDevice::init(name);
            this->pServer = BLEDevice::createServer();
            this->pService = this->pServer->createService(SERVICE_UUID);
            this->pCharacteristic = this->pService->createCharacteristic(
                CHARACTERISTIC_UUID,
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE
            );
            this->pService->start();
            // Start advertising the service
            this->pAdvertising = BLEDevice::getAdvertising();
            this->pAdvertising->addServiceUUID(SERVICE_UUID);
            this->pAdvertising->setScanResponse(true);
            this->pAdvertising->setMinPreferred(0x06);  // Functions that help with iPhone compatibility
            this->pAdvertising->setMinPreferred(0x12);
            BLEDevice::startAdvertising();

            isReading = true;
            xTaskCreate([](void *p) {
                ((BluetoothManager*)p)->taskRead(ssid,password);
            }, "taskRead", 10000, this, 1, NULL);
            sendBLE("ESP32 will switch to Wifi connection after diisconnected from Bluetooth");
            //stopBLE();
        };

        void stopBLE() { 
            BLEDevice::deinit(true); 
        };

        ~BluetoothManager(){
            isReading = false;
        };

        void sendBLE(String message){
            if(this.pCharacteristic != NULL){
                pCharacteristic->setValue(message);
            }
        };

    private :
        bool isReading;
        void taskRead(EEPROMClass ssid, EEPROMClass password){//EepromManager ssid, EepromManager password
            while (isReading) {
                String value = this.pCharacteristic->getValue();
                if (value.length() > 0) {
                    //as deserialization split ssid & password
                    const char* delimiter = ",";
                    ssid.writeString(0,value.substring(0, value.indexOf(delimiter,0)));
                    password.writeString(0,value.substring(value.indexOf(delimiter,0)+1));
                    printf("writeEEprom finishes \n");
                    //inutile ensuite
                    char storedSSID [32];
                    char storedPassword [64];
                    printf("storedSSID= %c\n",ssid.get(0,storedSSID));
                    printf("storedPassword= %c\n",password.get(0, storedPassword));
                }
                delay(100)
            }
        };
}