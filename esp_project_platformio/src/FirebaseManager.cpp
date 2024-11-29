#include "FirebaseManager.h"

Firebase firebaseData(FIREBASE_DB_URL,FIREBASE_API_KEY);

FirebaseManager& FirebaseManager::getInstance(){
  static FirebaseManager instance;
  return instance;
}

FirebaseManager::FirebaseManager() {
    this->mac = ManagerWifi.mac;
}

void FirebaseManager::sendData(JsonDocument jsonData, String path, bool useMacAddress = true) {
    String output;
    serializeJson(jsonData, output);
    if (useMacAddress) {
        firebaseData.setJson(path + this->mac,output);// + getMacAddress()
    } else {
        firebaseData.setJson(path,output);
    }
}

JsonDocument FirebaseManager::getData(String path, bool useMacAddress = true) {
    String output;
    if (useMacAddress) {
        output = firebaseData.getJson(path + this->mac);
    } else {
        output = firebaseData.getJson(path);
    }
    JsonDocument json_file;
    deserializeJson(json_file, output);
    return json_file;
}

FirebaseManager ManagerFirebase = FirebaseManager::getInstance();