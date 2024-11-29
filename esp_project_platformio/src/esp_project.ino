#include "SetupBLEServer.h"
#include "EEPROMManager.h"
#include "WifiManager.h"
#include "TimestampManager.h"
#include <Wire.h>


//Define pin of captors
#define LED_PIN A2 //led linked to the finite state of coffee machine : fix, short blinking, fast blinking
#define INFRA_PIN A4 //for the infrared presence captor OK
//#define WATERLEVEL_PIN 23  //23 I2C1 or I2C1 both same for the water level captor but not necessary
#define BTN_ONOFF_PIN A0 //for the ON/OFF button
#define BTN_1COFFEE_PIN 15 //D4 for the button to make 1 coffee
#define BTN_2COFFEE_PIN 14 //D2 for the button to make 2 coffee
//define param for water level
unsigned char water_level_data[12]={0};
#define ATTINY1_HIGH_ADDR   0x78
TimerHandle_t xTimerHandle1;

// Callback to handle credentials
void handleCredentials(const String ssid, const String password) {
  Serial.printf("SSID: %s ;\n Password: %s ;\n ",ssid,password);
  ManagerEEPROM.writeData(ssid, password); //EEPROMManager is the singleton of EEPROMManager define in the .h
  Serial.println("End of write in EEprom");
  ESP.restart();
}

String getDeviceName() {
  const char hexChars[] = "0123456789ABCDEF";
  String name = "Coffee Machine (";
  for (int i = 0; i < 4; i++) {
      name += hexChars[rand() % 16];
  }
  return name += ")";
}

char storedSSID [64];
char storedPassword [64];

void setup() {
  Serial.begin(115200);
  //ManagerEEPROM.clearData(512);  
  Serial.println("START ESP");  

  if (!ManagerEEPROM.begin(256,256)) { 
    Serial.println("Échec de l'initialisation de l'EEPROM SSID et PSSWD"); 
    ESP.restart();
    return; 
  } 
  Serial.println("EEPROM initialisée");
  ManagerEEPROM.getData(storedSSID,storedPassword);
  Serial.printf("storedSSID : %c\n storedPassword : %c\n", storedSSID, storedPassword);
  
  if (strcmp(storedSSID,0) == 0 && strcmp(storedPassword,0) == 0){
    //1st setup so start with BLE
    Serial.println("1st setup so start with BLE");
    SetupBLEServer::start(ManagerWifi.mac, getDeviceName(), false, handleCredentials);
    while (true);
  }
  else{
    //2nd setup so start with Wifi
    Serial.println("2nd setup so start with Wifi");
    ManagerWifi.beginWifi(storedSSID,storedPassword);//could not convert 'storedPassword' from 'char* [64]' to 'const String&'
    if(!ManagerWifi.checkConnected()){
      SetupBLEServer::start(ManagerWifi.mac, getDeviceName(), true, handleCredentials);
      while (true);
    }
    Timestamp.begin();
  }
  //about I2C
  Wire.begin();
  //about captors
  pinMode(INFRA_PIN,INPUT);
  pinMode(LED_PIN,INPUT);
  pinMode(BTN_ONOFF_PIN,OUTPUT);
  pinMode(BTN_1COFFEE_PIN,OUTPUT);
  pinMode(BTN_2COFFEE_PIN,OUTPUT);
}

void loop() {
    Serial.println(Timestamp.get());
    delay(1000);
    //define sequence of the coffee machine
    if (Serial.available() > 0) {
    String monitor_command = Serial.readStringUntil('\n');
    monitor_command.trim();
  
    if (monitor_command.equalsIgnoreCase("ON")){
      Serial.println("Start machine");
      digitalWrite(BTN_ONOFF_PIN, HIGH); 
    }
    else if (monitor_command.equalsIgnoreCase("OFF")){
      Serial.println("Stop machine");
      digitalWrite(BTN_ONOFF_PIN, LOW); //not necessary, just wait until the time (30min) of wake down and check when launch ; now need to check led frequencies
    }
    else if (monitor_command.equalsIgnoreCase("1coffee")){
      if(analogRead(INFRA_PIN) == 0){
        Serial.println("Cup detected");
        Serial.println("Start 1 coffee");
        digitalWrite(BTN_1COFFEE_PIN, HIGH);
        delay(1500);
        digitalWrite(BTN_1COFFEE_PIN, LOW);
        Serial.println("BTN 1 coffee down");
      }
      else{
        Serial.println("No cup detected : please add one to serve a coffee");
      }
    }
    else{
      if(analogRead(INFRA_PIN) == 0){
        Serial.println("Cup detected");
        Serial.println("Start 2 coffees");
        digitalWrite(BTN_2COFFEE_PIN, HIGH);
        delay(1500);
        digitalWrite(BTN_2COFFEE_PIN, LOW);
        Serial.println("BTN 2 coffees down");
      }
      else{
        Serial.println("No cup detected : please add two to serve 2 coffees");
      }
    }
  }

  // xTimerHandle1 = xTimerCreate( 
  //   "Timer 1",
  //   500,
  //   pdTRUE,
  //   ( void * ) 0,
  //   vTimerCallback
  // ); 
  // xTimerStart(xTimerHandle1, 0);

}

// void vTimerCallback(TimerHandle_t xTimer){
//   if (pvTimerGetTimerID(xTimer) == 0) {
//     //DeviceData devicedata();
//     timerInterruptSensors();
//   }
// }
//for capting the waterlevel
void WaterLevelReading(){
  memset(water_level_data,0,sizeof(water_level_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, sizeof(water_level_data));
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
    for (int i = 0 ; i < 8; i++) {
      if (water_level_data[i] > 100) {
        touch_val |= 1 << i;
      }
    }
    for(int i=0; i<sizeof(water_level_data);i++){
      if(water_level_data[i] > 100){
        touch_val |= (uint32_t)1 << (8+i);
      }
    }
    while(touch_val & 0x01){
      trig_section++;
      touch_val >>= 1;
    }
    return trig_section*5;
  }
}