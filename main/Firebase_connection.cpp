#include <Firebase.h>
#include "secrets.h"
#include "Firebase_connection.h"
#include <ArduinoJson.h>
#include "device_data.h"

#include <string>
#include <Arduino.h>
using namespace std;

//the different paths in firebase database
String pathdevice;
const String pathdata= "sensors/"+pathdevice;
const String pathrequest= pathdevice +"/request";

//DeviceData devicedata();

Firebase firebaseData(FIREBASE_DTB_URL);
//firebaseData.begin(FIREBASE_DTB_URL,FIREBASE_API_KEY);

void setUpFirebase(String mac_address){
  pathdevice=mac_address;
}

//about the sending of sensors data which are set by a 1sec. timer
void sendingDataSensors(DeviceData& devicedata){
  JsonDocument json_file;
  //jsonify the data..
  json_file["waterlevel"]= devicedata.getWaterLevel();
  json_file["presence"]= devicedata.getIsCupDetected();
  String output;
  serializeJson(json_file, output);
  //..and send to firebase
  firebaseData.setJson(pathdata,output);
}

