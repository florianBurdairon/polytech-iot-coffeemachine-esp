#include"device_data.h"
#include <string>
using namespace std;

float DeviceData::getWaterLevel(){
  return waterLevel;
}
bool DeviceData::getIsCupDetected(){
  return isCupDetected;
}
void DeviceData::setWaterLevel(float waterLevel){
  this->waterLevel=waterLevel;
}
void DeviceData::setIsCupDetected(bool isCupDetected){
  this->isCupDetected=isCupDetected;
}
void DeviceData::setMachineState(string machineState){
  this->machineState=machineState;
}