#include "SetupBLEServer.h"
#include "EEPROMManager.h"
#include "secrets.h"
// #include "WifiManager.h"
// #include "TimestampManager.h"
// #include <Wire.h>
// #include "FirebaseManager.h"
// #include <freertos/FreeRTOS.h>
// #include <freertos/timers.h>

// //Define pin of captors
// #define LED_PIN A2 //led linked to the finite state of coffee machine : fix, short blinking, fast blinking
// #define INFRA_PIN A4 //for the infrared presence captor OK
// #define BTN_ONOFF_PIN A0 //for the ON/OFF button
// #define BTN_1COFFEE_PIN 15 //D4 for the button to make 1 coffee
// #define BTN_2COFFEE_PIN 14 //D2 for the button to make 2 coffee

// //define param for water level
// unsigned char water_level_high_data[12]={0};
// unsigned char water_level_low_data[8] = {0};
// #define ATTINY1_HIGH_ADDR 0x78
// #define ATTINY2_LOW_ADDR 0x77

//for reuse ssid and password value through the code
char storedSSID [16];
char storedPassword [16];

// //define the interruption function for IR cup detector
// void IRAM_ATTR handleCupDetectedInterrupt(){
//   JsonDocument cupCaptor;
//   bool value;
//   if(analogRead(INFRA_PIN) <= 100){
//     value = true;
//   }
//   else{
//     value = false;
//   }
//   cupCaptor["presence"] = value;
//   ManagerFirebase.sendData(cupCaptor, "sensors", true);
//   Serial.println("Cup detected");
// }

// // Define the timer handle for firebase ping, getting ledstate and setting waterlevel
// TimerHandle_t xFirebasePingTimerHandle;
// TimerHandle_t xCoffeeTaskTimerHandle;
// TimerHandle_t xWaterLevelTimerHandle;
// TimerHandle_t xCalculatedLedFrequencyTimerHandle;

// // Callback function to send data to Firebase
// void vFirebasePingTimerCallback(TimerHandle_t xTimer) {
//   JsonDocument deviceStatus;
//   deviceStatus["mac"] = ManagerWifi.mac;
//   deviceStatus["lastOnline"] = Timestamp.get();
//   deviceStatus["status"] = "ONLINE";
//   ManagerFirebase.sendData(deviceStatus, "devices", true);
//   Serial.println("Ping sent to Firebase");
// }
// //Callback function to send waterlevel value to Firebase
// void vWaterLevelTimerCallback(TimerHandle_t xTimer){
//   JsonDocument waterLevel;
//   waterLevel["waterLevel"] = convertWaterSensorValues();
//   ManagerFirebase.sendData(waterLevel, "sensors", true);
//   Serial.println("Water level sent to Firebase");
// }

// //Callback function and variables to get led state and calculate the frequency for changing status request in Firebase
// volatile int stateChangeCount = 0;
// float frequency = 0.0f;

// // Callback function to check LED state and count the number of state changes
// void IRAM_ATTR checkLEDState() {
//   static bool lastState = LOW;
//   bool currentState = digitalRead(LED_PIN);
//   if (currentState != lastState) {
//     stateChangeCount++;
//     lastState = currentState;
//   }
// }
// // Callback function to calculate the LED frequency
// void calculateLedFrequency(TimerHandle_t xTimer){
//   frequency = (stateChangeCount / 2.0f ); // Divide by 2 because each cycle has two state changes
//   stateChangeCount = 0; // Reset the count for the next interval
// }

// bool isCoffeeMachineReady(){
//   //check if the coffee machine is ready to make coffee
//   return frequency == 0.0f && digitalRead(LED_PIN)==HIGH;
// }
// //action to do with the coffee machine
// bool doRequestedAction(String action){
//   if(action.equalsIgnoreCase("1CUP")){
//     Serial.println("Start 1 coffee");
//     digitalWrite(BTN_1COFFEE_PIN, HIGH);
//     delay(500);
//     digitalWrite(BTN_1COFFEE_PIN, LOW);
//     return true;
//   }
//   else if(action.equalsIgnoreCase("2CUP")){
//     Serial.println("Start 2 coffees");
//     digitalWrite(BTN_2COFFEE_PIN, HIGH);
//     delay(500);
//     digitalWrite(BTN_2COFFEE_PIN, LOW);
//     return true;
//   }
//   return false;
// }
// void transformRequest(JsonDocument macRequest){
//   //exchange the current by the next request
//   JsonDocument nextRequest;
//   long timesTamp = macRequest["next"]["timestamp"];
//   if(Timestamp.get() - timesTamp < 0){
//     return;
//   }
//   nextRequest["current"] = macRequest["next"];
//   //take the first item of the list of requests if list isn't empty in order to transform it in the next request
//   JsonObject list = macRequest["list"];
//   if(list.size() > 0){
//     nextRequest["next"] = list.begin()->value(); 
//     //and then remove it from the list
//     JsonObject::iterator it = list.begin();
//     list.remove(it->key()); 
//     if(list.size() > 0) {
//       nextRequest["list"] = list;
//     }
//     else {
//       nextRequest["list"] = "null";
//     }
//   }
//   else{//else set the next request to null
//     nextRequest["next"] = "null";
//   }  
//   ManagerFirebase.sendData(nextRequest, "requests/"+ ((String)macRequest["mac"]), false);
// }

// //Callback function to handle the coffee requests
// void vCoffeeTaskTimerCallback(TimerHandle_t xTimer) {
//   JsonDocument macRequest = ManagerFirebase.getData("requests/", true);//get the current, the list and the next requests
//   JsonDocument nextRequest;
//   macRequest["next"]["status"] = "INITIALISING";
//   nextRequest["current"] = macRequest["next"];
//   ManagerFirebase.sendData(nextRequest,"requests/"+((String)macRequest["current"]["mac"]), false);
  
//   JsonDocument log;

//   // Check if there is a current request
//   if( ((String)macRequest["current"]).equalsIgnoreCase("null")){
//     // Check next request
//     if( ((String)nextRequest["current"]).equalsIgnoreCase("null")){
//       transformRequest(macRequest);
//     }
//     return;
//   }

//   // Check if the current request is initialising
//   if(((String)macRequest["current"]["status"]).equalsIgnoreCase("INITIALISING")) {
//     if(analogRead(INFRA_PIN) <= 100 && convertWaterSensorValues() >= 0.1) {
//       JsonDocument setRequest;
//       setRequest = macRequest["current"];
//       setRequest["status"] = "WARMING";
//       ManagerFirebase.sendData(setRequest, "requests/"+((String)macRequest["current"]["mac"])+"/current", false);
//     }
//     else if(analogRead(INFRA_PIN) > 100) {
//       // No cup detected
//       // Log the request
//       JsonDocument logRequest;
//       logRequest = macRequest["current"];
//       logRequest["status"] = "ERROR_CUP";
//       ManagerFirebase.sendData(logRequest, "logs/"+((String)macRequest["current"]["uid"])+"/"+((String)macRequest["current"]["timestamp"]), false);
      
//       // Remove the current request
//       JsonDocument clearRequest;
//       clearRequest["current"] = "null";
//       ManagerFirebase.sendData(clearRequest, "requests/", true);
//     }
//     else {
//       // Not enough water
//       // Log the request
//       JsonDocument logRequest;
//       logRequest = macRequest["current"];
//       logRequest["status"] = "ERROR_WATER";
//       ManagerFirebase.sendData(logRequest, "logs/"+((String)macRequest["current"]["uid"])+"/"+((String)macRequest["current"]["timestamp"]), false);
      
//       // Remove the current request
//       JsonDocument clearRequest;
//       clearRequest["current"] = "null";
//       ManagerFirebase.sendData(clearRequest, "requests/", true);
//     }
//     return;
//   }

//   // Check if the current request is warming
//   if( ((String)macRequest["current"]["status"]).equalsIgnoreCase("WARMING")) {
//     Serial.println("Start machine");
//     digitalWrite(BTN_ONOFF_PIN, HIGH); //start machine by activating the button("relais ON/OFF")
//     delay(500);
//     digitalWrite(BTN_ONOFF_PIN, LOW);
//     bool isReady = false;
//     int timeout = millis() + 60000;
//     while(!isReady && millis() < timeout) {
//       isReady = isCoffeeMachineReady();
//     }
//     if (isReady) {
//       JsonDocument setRequest;
//       setRequest = macRequest["current"];
//       setRequest["status"] = "FILLING";
//       ManagerFirebase.sendData(setRequest, "requests/"+((String)macRequest["current"]["mac"])+"/current", false);
//     }
//     else {
//       // Error warming
//       // Log the request
//       JsonDocument logRequest;
//       logRequest = macRequest["current"];
//       logRequest["status"] = "ERROR_WARMING";
//       ManagerFirebase.sendData(logRequest, "logs/"+((String)macRequest["current"]["uid"])+"/"+((String)macRequest["current"]["timestamp"]), false);
      
//       // Remove the current request
//       JsonDocument clearRequest;
//       clearRequest["current"] = "null";
//       ManagerFirebase.sendData(clearRequest, "requests/", true);
//     }
//     return;
//   }

//   // Check if the current request is filling
//   if(((String)macRequest["current"]["status"]).equalsIgnoreCase("FILLING")) {
//     doRequestedAction(((String)macRequest["current"]["action"]));
//     bool isReady = false;
//     long timeout = millis() + 60000;
//     while(!isReady && millis() < timeout) {
//       isReady = isCoffeeMachineReady();
//     }
//     if(isReady) {
//       JsonDocument setRequest;
//       setRequest = macRequest["current"];
//       setRequest["status"] = "COLLECTING";
//       ManagerFirebase.sendData(setRequest, "requests/"+((String)macRequest["current"]["mac"])+"/current", false);
//     }
//     else {
//       // Error filling
//       // Log the request
//       JsonDocument logRequest;
//       logRequest = macRequest["current"];
//       logRequest["status"] = "ERROR_FILLING";
//       ManagerFirebase.sendData(logRequest, "logs/"+((String)macRequest["current"]["uid"])+"/"+((String)macRequest["current"]["timestamp"]), false);
      
//       // Remove the current request
//       JsonDocument clearRequest;
//       clearRequest["current"] = "null";
//       ManagerFirebase.sendData(clearRequest, "requests/", true);
//     }
//     return;
//   }

//   // Check if the current request is collecting
//   if(((String)macRequest["current"]["status"]).equalsIgnoreCase("COLLECTING")) {
//     digitalWrite(BTN_ONOFF_PIN, HIGH); //Turn off the machine
//     delay(500);
//     digitalWrite(BTN_ONOFF_PIN, LOW);
//     while(analogRead(INFRA_PIN) <= 100) {
//       // Wait for cup to be removed
//     }
//     Serial.println("Cup detected");
    
//     // Log the request
//     JsonDocument logRequest;
//     logRequest = macRequest["current"];
//     logRequest["status"] = "SUCCESS";
//     ManagerFirebase.sendData(logRequest, "logs/"+((String)macRequest["current"]["uid"])+"/"+((String)macRequest["current"]["timestamp"]), false);
    
//     // Remove the current request
//     JsonDocument clearRequest;
//     clearRequest["current"] = "null";
//     ManagerFirebase.sendData(clearRequest, "requests/", true);
//     return;
//   }
  
//   // Check if the current request is taking too long
//   long timestamp = macRequest["current"]["timestamp"];
//   if(Timestamp.get() - timestamp > 30){//error timeout of 30s
//     macRequest["current"]["status"] = "ERROR_TIMEOUT";
//     JsonDocument setRequest;
//     setRequest[((String)timestamp)]= macRequest["current"];
//     ManagerFirebase.sendData(setRequest, "logs/"+((String)macRequest["current"]["uid"]), false);
//     return;
//   }
// }

// Callback to handle credentials
void handleCredentials(const String ssid, const String password) {
  Serial.printf("SSID: %s ;\n Password: %s ;\n ",ssid,password);
  delay(10000);
  ManagerEEPROM.writeData(ssid, password); //EEPROMManager is the singleton of EEPROMManager define in the .h
  Serial.println("End of write in EEprom");
  //ESP.restart();
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

// //for capting the waterlevel
// void WaterLevelReadingSection(){
//   //for 8 sections
//   memset(water_level_low_data,0,sizeof(water_level_low_data));
//   Wire.requestFrom(ATTINY2_LOW_ADDR, 8);//sizeof(water_level_low_data)
//   while( 8 != Wire.available());//sizeof(water_level_low_data)
//   for(int i=0; i< 8;i++){ //sizeof(water_level_low_data)
//     water_level_low_data[i] = Wire.read();
//   }
//   delay(10);

//   //for 12 sections
//   memset(water_level_high_data,0,sizeof(water_level_high_data));
//   Wire.requestFrom(ATTINY1_HIGH_ADDR,12);//sizeof(water_level_high_data)
//   while( 12 != Wire.available());//sizeof(water_level_high_data)
//   for(int i=0; i< 12;i++){ //sizeof(water_level_high_data)
//     water_level_high_data[i] = Wire.read();
//   }
//   delay(10);
// }

// float convertWaterSensorValues(){
//   for(;;){
//     uint32_t touch_val = 0;
//     uint8_t trig_section = 0;
//     WaterLevelReadingSection();
//     for (int i = 0 ; i < 8; i++) {//sizeof(water_level_low_data)
//       if (water_level_low_data[i] > 100) {
//         touch_val |= 1 << i;
//       }
//     }
//     for(int i=0; i< 12;i++){ //sizeof(water_level_high_data)
//       if(water_level_high_data[i] > 100){
//         touch_val |= (uint32_t)1 << (8+i);
//       }
//     }
//     while(touch_val & 0x01){
//       trig_section++;
//       touch_val >>= 1;
//     }
//     return trig_section*5 / 100.0f;
//   }
// }

// /// @brief //define the setup of the coffee machine
void setup() {
  Serial.begin(115200);
//   //ManagerEEPROM.clearData(512);  
  Serial.println("START ESP");  

  if (!ManagerEEPROM.begin(256,256)) { 
    Serial.println("Échec de l'initialisation de l'EEPROM SSID et PSSWD"); 
    ESP.restart();
    return; 
  } 
  Serial.println("EEPROM initialisée");
  ManagerEEPROM.getData(storedSSID,storedPassword);
  Serial.printf("storedSSID : %c\n storedPassword : %c\n", storedSSID, storedPassword);
  
  // if(true){// (strcmp(storedSSID,"") == 0 && strcmp(storedPassword,"") == 0){
    //1st setup so start with BLE
    // Serial.println("1st setup so start with BLE");
    SetupBLEServer::start("DC:54:75:D0:D0:F8", getDeviceName(), false, handleCredentials);//ManagerWifi.mac
    //storedSSID = "galaxya425g8288" ;
    //storedPassword = "avmw4585" ;
    //handleCredentials("galaxya425g8288","avmw4585");
    //while (true);
  // }
  // else{
//     //not the 1st setup so start directly with Wifi
    // Serial.println("2nd setup so start with Wifi");
    //handleCredentials("galaxya425g8288","avmw4585");
//     ManagerWifi.beginWifi(storedSSID,storedPassword);//could not convert 'storedPassword' from 'char* [64]' to 'const String&'
//     if(!ManagerWifi.checkConnected()){
//       ManagerWifi.reconnectWifi();
//       //SetupBLEServer::start(ManagerWifi.mac, getDeviceName(), true, handleCredentials);
//       //while (true);
//     }
//     Timestamp.begin();
//     //about I2C water level captor
//     Wire.begin();
//     //about captors
//     pinMode(INFRA_PIN,INPUT);
//     pinMode(LED_PIN,INPUT);
//     pinMode(BTN_ONOFF_PIN,OUTPUT);
//     pinMode(BTN_1COFFEE_PIN,OUTPUT);
//     pinMode(BTN_2COFFEE_PIN,OUTPUT);
    
//     //about IR cup detector that detect cup under ~17cm of the captor
//     attachInterrupt(digitalPinToInterrupt(INFRA_PIN),handleCupDetectedInterrupt,CHANGE);
//     attachInterrupt(digitalPinToInterrupt(LED_PIN),checkLEDState,CHANGE);
//     // Create timers
//     //for ping to firebase ("the heart of the ESP")
//     xFirebasePingTimerHandle = xTimerCreate(
//       "FirebasePingTimer",          // Timer name
//       pdMS_TO_TICKS(60000),    // Timer period in ticks (1 minute)
//       pdTRUE,                   // Auto-reload
//       (void*)0,                 // Timer ID
//       vFirebasePingTimerCallback    // Callback function
//     );
//     // Check if the timer was created successfully
//     if (xFirebasePingTimerHandle != NULL) {
//       // Start the timer
//       if (xTimerStart(xFirebasePingTimerHandle, 0) != pdPASS) {
//         Serial.println("Failed to start the ping firebase timer");
//       }
//     } else {
//       Serial.println("Failed to create the ping firebase timer");
//     }

//     //for waterlevel ping to firebase
//     xWaterLevelTimerHandle = xTimerCreate(
//       "WaterLevelTimer",          
//       pdMS_TO_TICKS(10000),    // 10 seconds
//       pdTRUE,                   
//       (void*)0,                 
//       vWaterLevelTimerCallback    
//     );
//     if (xWaterLevelTimerHandle != NULL) {
//       if (xTimerStart(xWaterLevelTimerHandle, 0) != pdPASS) {
//         Serial.println("Failed to start the waterlevel timer");
//       }
//     } else {
//       Serial.println("Failed to create the waterlevel timer");
//     }

//     //for measuring led frequency
//     xCalculatedLedFrequencyTimerHandle = xTimerCreate(
//       "LedFrequencyTimer",
//       pdMS_TO_TICKS(1000),
//       pdTRUE,
//       ( void * ) 0,
//       calculateLedFrequency
//     );
//     xTimerStart(xCalculatedLedFrequencyTimerHandle, 0);
//     // Check if the timer was created successfully
//     if (xCalculatedLedFrequencyTimerHandle != NULL) {
//       // Start the timer
//       if (xTimerStart(xCalculatedLedFrequencyTimerHandle, 0) != pdPASS) {
//         Serial.println("Failed to start the led frequency timer");
//       }
//     } else {
//       Serial.println("Failed to create the led frequency timer");
//     }

//     //for coffee task get and ping to firebase
//     xCoffeeTaskTimerHandle = xTimerCreate(
//       "xCoffeeTaskTimer",          
//       pdMS_TO_TICKS(30000),    // 
//       pdTRUE,                  
//       (void*)0,                 
//       vCoffeeTaskTimerCallback   
//     );
//     if (xCoffeeTaskTimerHandle != NULL) {
//       if (xTimerStart(xCoffeeTaskTimerHandle, 0) != pdPASS) {
//         Serial.println("Failed to start the coffee task timer");
//       }
//     } else {
//       Serial.println("Failed to create the coffee task timer");
//     }
  // }
}

void loop() {
  //Serial.println("loop works");
}