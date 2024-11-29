#ifndef DEVICEDATA_H
#define DEVICEDATA_H
#include <Arduino.h>

class DeviceData{
  public:
    static DeviceData& getInstance();
    float getWaterLevel();
    bool getIsCupDetected();
    void setWaterLevel(float waterLevel);
    void setIsCupDetected(bool isCupDetected);

  private:
    float waterLevel ;
    bool isCupDetected;
    String machineState;
    DeviceData();
};

extern DeviceData Data;

#endif