#include <Wire.h>
#include <Firebase.h>
#include "secrets.h"
#include "Firebase_connection.h"
#include "Wifi_connection.h"
#include "Bluetooth_connection.h"
#include "device_data.h"
#include <EEPROM.h>

//SSID and Password Wifi
char storedSSID [32];
char storedPassword [64];
int EEPROMAddress = 0;

//Define pin of captors
#define LED_PIN A2 //led linked to the finite state of coffee machine : fix, short blinking, fast blinking
#define INFRA_PIN A4 //for the infrared presence captor
#define WATERLEVEL_PIN 23 //for the water level captor
#define BTN_ONOFF_PIN A0 //for the ON/OFF button
#define BTN_1COFFEE_PIN D4 //for the button to make 1 coffee
#define BTN_2COFFEE_PIN D2 //for the button to make 2 coffee

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

void setup() {
  Serial.begin(115200);
  //begin the connection to ESP32 with Bluetooth to store SSID and password of the Wifi network
  setBluetooth();
  //Then these values are obtained we can switch to Wifi connection
  //disconnect bluetooth

  //launch Wifi connection
  //setUpWifiStation();

  //about captors
  pinMode(INFRA_PIN,INPUT);
  pinMode(WATERLEVEL_PIN,INPUT);
  // pinMode(LED_PIN,INPUT);
  // pinMode(BUTTON_PIN,INPUT);
  // Wire.begin();
  // xTimerHandle1 = xTimerCreate( 
  //   "Timer 1",
  //   500,
  //   pdTRUE,
  //   ( void * ) 0,
  //   vTimerCallback
  // ); 
  // xTimerStart(xTimerHandle1, 0);
  //timerInterruptSensors();
}

void loop() {
}

// void timerInterruptSensors(){
//   infraValue = analogRead(INFRA_PIN);
//   if(infraValue==0){
//     devicedata.setIsCupDetected(true);
//   }else{
//     devicedata.setIsCupDetected(false);
//   }
//   //waterValue = digitalRead(WATERLEVEL_PIN);
//   int percentWaterLevel=convertWaterSensorValues();
//   devicedata.setWaterLevel(percentWaterLevel);
//   sendingDataSensors(ref_devicedata);
//   //ledValue = analogRead(LED_PIN);
//   //btnValue = analogRead(BUTTON_PIN);
//   // if(ledValue != 0){
//   //   Serial.println(F("| ledValue ="));
//   //   Serial.println(ledValue);
//   // }
//   // if(btnValue != 0){
//   //   Serial.println("| btnValue =");
//   //   Serial.println(btnValue);
//   // }
//   //delay(100);
// }

// void WaterLevelReading(){
//   memset(water_level_data,0,sizeof(water_level_data));
//   Wire.requestFrom(ATTINY1_HIGH_ADDR,sizeof(water_level_data));
//   while(sizeof(water_level_data) != Wire.available());
//   for(int i=0; i<sizeof(water_level_data);i++){
//     water_level_data[i] = Wire.read();
//   }
//   delay(10);
// }

// uint8_t convertWaterSensorValues(){
//   for(;;){
//     uint32_t touch_val = 0;
//     uint8_t trig_section = 0;
//     WaterLevelReading();
//     /*for(int i=0; i<sizeof(data);i++){
//       Serial.print(data[i]+" | ");
//     }*/
//     for (int i = 0 ; i < 8; i++) {
//       if (water_level_data[i] > THRESHOLD) {
//         touch_val |= 1 << i;

//       }
//     }
//     for(int i=0; i<sizeof(water_level_data);i++){
//       if(water_level_data[i] > THRESHOLD){
//         touch_val |= (uint32_t)1 << (8+i);
//       }
//     }
//     while(touch_val & 0x01){
//       trig_section++;
//       touch_val >>= 1;
//     }
//     Serial.print("Water level detected");
//     Serial.print(trig_section*5);
//     Serial.print("%");
//     Serial.println("________");
//     int output=trig_section*5;
//     return output;
//   }
// }


// void vTimerCallback(TimerHandle_t xTimer){
//   if (pvTimerGetTimerID(xTimer) == 0) {
//     //DeviceData devicedata();
//     timerInterruptSensors();
//   }
// }


//Pour bluetooth faire avec le BLE pas le Serial chit !

