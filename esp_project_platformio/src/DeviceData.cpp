#include "DeviceData.h"
DeviceData& DeviceData::getInstance(){
  static DeviceData instance;
  return instance;
}
DeviceData::DeviceData(){
  waterLevel=0;
  isCupDetected=false;
}
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
DeviceData Data = DeviceData::getInstance();