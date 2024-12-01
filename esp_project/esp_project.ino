#include "SetupBLEServer.h"
#include "EEPROMManager.h"
#include "WifiManager.h"
#include "TimestampManager.h"
#include <Wire.h>
#include "FirebaseManager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>


//Define pin of captors
#define LED_PIN A2 //led linked to the finite state of coffee machine : fix, short blinking, fast blinking
#define INFRA_PIN A4 //for the infrared presence captor OK
#define BTN_ONOFF_PIN A0 //for the ON/OFF button
#define BTN_1COFFEE_PIN 15 //D4 for the button to make 1 coffee
#define BTN_2COFFEE_PIN 14 //D2 for the button to make 2 coffee

//define param for water level
unsigned char water_level_high_data[12]={0};
unsigned char water_level_low_data[8] = {0};
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77

//for reuse ssid and password value through the code
char storedSSID [64];
char storedPassword [64];

//define the interruption function for IR cup detector
void IRAM_ATTR handleCupDetectedInterrupt(){
  DynamicJsonDocument cupCaptor(256);
  bool value;
  if(analogRead(INFRA_PIN) == 0){
    value = true;
  }
  else{
    value = false;
  }
  cupCaptor["presence"] = value;
  ManagerFirebase.sendData(cupCaptor, "sensors", true);
  Serial.println("Cup detected");
}

// Define the timer handle for firebase ping, getting ledstate and setting waterlevel
TimerHandle_t xFirebasePingTimerHandle;
TimerHandle_t xCoffeeTaskTimerHandle;
TimerHandle_t xWaterLevelTimerHandle;

// Callback function to send data to Firebase
void vFirebasePingTimerCallback(TimerHandle_t xTimer) {
  DynamicJsonDocument deviceStatus(256);
  deviceStatus["mac"] = ManagerWifi.mac;
  deviceStatus["lastOnline"] = Timestamp.get();
  deviceStatus["status"] = "ONLINE";
  ManagerFirebase.sendData(deviceStatus, "devices", true);
  Serial.println("Ping sent to Firebase");
}
//Callback function to send waterlevel value to Firebase
void vWaterLevelTimerCallback(TimerHandle_t xTimer){
  DynamicJsonDocument waterLevel(256);
  waterLevel["waterLevel"] = convertWaterSensorValues();
  ManagerFirebase.sendData(waterLevel, "sensors", true);
  Serial.println("Water level sent to Firebase");
}
//Callback function to get led state and send it to Firebase
void vCoffeeTaskTimerCallback(TimerHandle_t xTimer){
  JsonDocument currentRequest = ManagerFirebase.getData("requests/"+ManagerWifi.mac+"/current", false);
  DynamicJsonDocument ledState(256);
  String status;
  switch (expression)
  {
  case /* constant-expression */:
    status = "INITIALIZING";
    //process to activate coffee machine with the button

    break;
  case :
    status ="WARMING";
    break;
  case :
    status ="FILLING";
    break;
  case :
    status ="COLLECTING";
    break;  
  default:
    break;
  }
  ledState["status"] = status;
  ManagerFirebase.sendData(ledState, "requests/"+ManagerWifi.mac+"/current", false);
  Serial.println("Led state sent to Firebase");
  ManagerFirebase.removeData("requests/"+ManagerWifi.mac+"/current", false);
  Serial.println("request current remove in Firebase");
  //create the log file associated
  DynamicJsonDocument log(256);
  switch (expression)
  { 
    case /* constant-expression */:
      log["status"] = "DONE";
      break;
    case :
      log["status"] = "ERROR_WATER";
      break;
    case :
      log["status"] = "ERROR_CUP";
      break;
    case :
      log["status"] = "ERROR_TIMEOUT";
      break; 
    case :
      log["status"] = "ERROR_OFFLINE";
    break; 
    case :
      log["status"] = "ERROR_WARMING";
    break; 
    case :
      log["status"] = "ERROR_FILLING";
    break; 
    default:  
      log["status"] = "SUCCESS";
      break;
  }
  log[action]= currentRequest["action"];
  log["timestamp"] = Timestamp.get();
  log["uid"] = currentRequest["uid"];
  ManagerFirebase.sendData(log, "logs/"+ userUID + toString(Timestamp.get()), false);
}

// Callback to handle credentials
void handleCredentials(const String ssid, const String password) {
  Serial.printf("SSID: %s ;\n Password: %s ;\n ",ssid,password);
  ManagerEEPROM.writeData(ssid, password); //EEPROMManager is the singleton of EEPROMManager define in the .h
  Serial.println("End of write in EEprom");
  ESP.restart();
}

//give the BLE name of the device
String getDeviceName() {
  const char hexChars[] = "0123456789ABCDEF";
  String name = "Coffee Machine (";
  for (int i = 0; i < 4; i++) {
      name += hexChars[rand() % 16];
  }
  return name += ")";
}

//for capting the waterlevel
void WaterLevelReadingSection(){
  //for 8 sections
  memset(water_level_low_data,0,sizeof(water_level_low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);//sizeof(water_level_low_data)
  while( 8 != Wire.available());//sizeof(water_level_low_data)
  for(int i=0; i< 8;i++){ //sizeof(water_level_low_data)
    water_level_low_data[i] = Wire.read();
  }
  delay(10);

  //for 12 sections
  memset(water_level_high_data,0,sizeof(water_level_high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR,12);//sizeof(water_level_high_data)
  while( 12 != Wire.available());//sizeof(water_level_high_data)
  for(int i=0; i< 12;i++){ //sizeof(water_level_high_data)
    water_level_high_data[i] = Wire.read();
  }
  delay(10);
}

uint8_t convertWaterSensorValues(){
  for(;;){
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    WaterLevelReadingSection();
    for (int i = 0 ; i < 8; i++) {//sizeof(water_level_low_data)
      if (water_level_low_data[i] > 100) {
        touch_val |= 1 << i;
      }
    }
    for(int i=0; i< 12;i++){ //sizeof(water_level_high_data)
      if(water_level_high_data[i] > 100){
        touch_val |= (uint32_t)1 << (8+i);
      }
    }
    while(touch_val & 0x01){
      trig_section++;
      touch_val >>= 1;
    }
    return trig_section*5 / 100.0f;
  }
}

/// @brief //define the setup of the coffee machine
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
    //SetupBLEServer::start(ManagerWifi.mac, getDeviceName(), false, handleCredentials);
    handleCredentials(TEST_SSID,TEST_PASSWORD);
    //while (true);
  }
  else{
    //not the 1st setup so start directly with Wifi
    Serial.println("2nd setup so start with Wifi");
    ManagerWifi.beginWifi(storedSSID,storedPassword);//could not convert 'storedPassword' from 'char* [64]' to 'const String&'
    if(!ManagerWifi.checkConnected()){
      ManagerWifi.reconnectWifi();
      //SetupBLEServer::start(ManagerWifi.mac, getDeviceName(), true, handleCredentials);
      //while (true);
    }
    Timestamp.begin();
    //about I2C water level captor
    Wire.begin();
    //about captors
    pinMode(INFRA_PIN,INPUT);
    pinMode(LED_PIN,INPUT);
    pinMode(BTN_ONOFF_PIN,OUTPUT);
    pinMode(BTN_1COFFEE_PIN,OUTPUT);
    pinMode(BTN_2COFFEE_PIN,OUTPUT);
    //about IR cup detector that detect cup under ~17cm of the captor
    attachInterrupt(digitalPinToInterrupt(INFRA_PIN),handleCupDetectedInterrupt,CHANGE);

    // Create timers
    //for ping to firebase ("the heart of the ESP")
    xFirebasePingTimerHandle = xTimerCreate(
      "FirebasePingTimer",          // Timer name
      pdMS_TO_TICKS(180000),    // Timer period in ticks (3 minutes)
      pdTRUE,                   // Auto-reload
      (void*)0,                 // Timer ID
      vFirebasePingTimerCallback    // Callback function
    );
    // Check if the timer was created successfully
    if (xFirebasePingTimerHandle != NULL) {
      // Start the timer
      if (xTimerStart(xFirebasePingTimerHandle, 0) != pdPASS) {
        Serial.println("Failed to start the ping firebase timer");
      }
    } else {
      Serial.println("Failed to create the ping firebase timer");
    }

    //for waterlevel ping to firebase
    xWaterLevelTimerHandle = xTimerCreate(
      "WaterLevelTimer",          
      pdMS_TO_TICKS(10000),    // 10 seconds
      pdTRUE,                   
      (void*)0,                 
      vWaterLevelTimerCallback    
    );
    if (xWaterLevelTimerHandle != NULL) {
      if (xTimerStart(xWaterLevelTimerHandle, 0) != pdPASS) {
        Serial.println("Failed to start the waterlevel timer");
      }
    } else {
      Serial.println("Failed to create the waterlevel timer");
    }

    //for cooffe task get and ping to firebase
    xCoffeeTaskTimerHandle = xTimerCreate(
      "xCoffeeTaskTimer",          
      pdMS_TO_TICKS(10000),    // 
      pdTRUE,                  
      (void*)0,                 
      vCoffeeTaskTimerCallback   
    );
  }
}


/// @brief //define the sequence of the coffee machine
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



