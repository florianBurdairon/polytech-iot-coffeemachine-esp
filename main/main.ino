#include <Wire.h>
#include "Firebase_connection.h"
#include "Wifi_connection.h"
#include "Bluetooth_connection.h"
#include "device_data.h"


//Define pin of captors
#define LED_PIN A2 //led linked to the finite state of coffee machine : fix, short blinking, fast blinking
#define INFRA_PIN A4 //for the infrared presence captor
#define WATERLEVEL_PIN 23
#define BUTTON_PIN A0

//define param for water level
unsigned char water_level_data[12]={0};
#define ATTINY1_HIGH_ADDR   0x78
#define THRESHOLD 100

//values read
int ledValue = 0;
int infraValue = 0;
int waterValue;
int btnValue = 0;

//class object
DeviceData devicedata;
DeviceData& ref_devicedata = devicedata;

//timer
TimerHandle_t xTimerHandle1;
// TODO: lire capteur niveau eau, les 3 boutons, les états de la led (chiant?)
//ATTENTION: ne pas avoir pc branché sur secteur en même temps que branché sur carte ESP car prblm isolement masse

void setup() {
  Serial.begin(115200);
  //DeviceData devicedata;
  setUpWifiStation();
  //setUpFirebase();
  //about captors
  pinMode(INFRA_PIN,INPUT);
  pinMode(WATERLEVEL_PIN,INPUT);
  // pinMode(LED_PIN,INPUT);
  // pinMode(BUTTON_PIN,INPUT);
  Wire.begin();
  // xTimerHandle1 = xTimerCreate( /* Just a text name, not used by the RTOS kernel. */
  //   "Timer 1",
  //   /* The timer period in ticks, must be greater than 0. */
  //   500,
  //   /* The timers will auto-reload themselves when they expire. */
  //   pdTRUE,
  //   /* The ID is used to store a count of the number of times the
  //     timer has expired, which is initialised to 0. */
  //   ( void * ) 0,
  //   /* Each timer calls the same callback when it expires. */
  //   vTimerCallback
  // ); 
  // xTimerStart(xTimerHandle1, 0);
}

void loop() {
  //sendingDataSensors();
  //delay(3000);
}

void timerInterruptSensors(){
  infraValue = analogRead(INFRA_PIN);
  if(infraValue==0){
    devicedata.setIsCupDetected(true);
    Serial.println("Cup of coffee detected");
  }else{
    devicedata.setIsCupDetected(false);
    Serial.println("Be aware no cup detected !");
  }
  //waterValue = digitalRead(WATERLEVEL_PIN);
  int percentWaterLevel=convertWaterSensorValues();
  devicedata.setWaterLevel(percentWaterLevel);
  sendingDataSensors(ref_devicedata);
  //ledValue = analogRead(LED_PIN);
  //btnValue = analogRead(BUTTON_PIN);
  // if(ledValue != 0){
  //   Serial.println("| ledValue =");
  //   Serial.println(ledValue);
  // }
  // if(btnValue != 0){
  //   Serial.println("| btnValue =");
  //   Serial.println(btnValue);
  // }
  //delay(100);
}

void WaterLevelReading(){
  memset(water_level_data,0,sizeof(water_level_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR,sizeof(water_level_data));
  while(sizeof(water_level_data) != Wire.available());
  for(int i=0; i<sizeof(water_level_data);i++){
    water_level_data[i] = Wire.read();
  }
  delay(10);
}

uint8_t convertWaterSensorValues(){
  for(;;){
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    WaterLevelReading();
    /*for(int i=0; i<sizeof(data);i++){
      Serial.print(data[i]+" | ");
    }*/
    for (int i = 0 ; i < 8; i++) {
      if (water_level_data[i] > THRESHOLD) {
        touch_val |= 1 << i;

      }
    }
    for(int i=0; i<sizeof(water_level_data);i++){
      if(water_level_data[i] > THRESHOLD){
        touch_val |= (uint32_t)1 << (8+i);
      }
    }
    while(touch_val & 0x01){
      trig_section++;
      touch_val >>= 1;
    }
    Serial.print("Water level detected");
    Serial.print(trig_section*5);
    Serial.print("%");
    Serial.println("________");
    int output=trig_section*5;
    return output;
  }
}


void vTimerCallback(TimerHandle_t xTimer){
  if (pvTimerGetTimerID(xTimer) == 0) {
    //DeviceData devicedata();
    timerInterruptSensors();
  }
}




