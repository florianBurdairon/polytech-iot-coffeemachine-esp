#ifndef FIREBASE_CONNECTION_H
#define FIREBASE_CONNECTION_H
#include "device_data.h" 
#include <Arduino.h>
#include <string>

void setUpFirebase(String mac_address);
void sendingDataSensors(DeviceData& devicedata);

#endif