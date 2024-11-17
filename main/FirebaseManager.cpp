#include <Firebase.h>
#include "secrets.h"
#include <ArduinoJson.h>
#include "device_data.h"
#include <Arduino.h>

class FirebaseManager{
    public:
        FirebaseManager(String mac) {
            this->mac = mac;
            firebaseData(FIREBASE_DB_URL);//(FIREBASE_DTB_URL,FIREBASE_API_KEY)
        }

        void sendData(JsonDocument jsonData, String path, bool useMacAddress = true) {
            String output;
            serializeJson(jsonData, output);
            if (useMacAddress) {
                firebaseData.setJson(path + getMacAddress(),output);
            } else {
                firebaseData.setJson(path,output);
            }
        }

        JsonDocument getData(String path, bool useMacAddress = true) {
            String output;
            if (useMacAddress) {
                output = firebaseData.getJson(path + getMacAddress());
            } else {
                output = firebaseData.getJson(path);
            }
            JsonDocument json_file;
            deserializeJson(json_file, output);
            return json_file;
        }

    private:
        String mac;
        Firebase firebaseData;     
}