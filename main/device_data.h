#ifndef DEVICEDATA_H
#define DEVICEDATA_H

#include <string>
using namespace std;

class DeviceData{
  private:
    float waterLevel ;
    bool isCupDetected;
    std::string machineState;
  public:
    float getWaterLevel();
    bool getIsCupDetected();
    void setWaterLevel(float waterLevel);
    void setIsCupDetected(bool isCupDetected);
    void setMachineState(string machineState);
};
#endif