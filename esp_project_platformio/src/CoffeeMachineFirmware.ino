#include <WiFi.h>
#include <Firebase.h>
#include "SetupBLEServer.h"
#include "EEPROMManager.h"
#include "MacManager.h"
#include "TimestampManager.h"
#include "secrets.h"
#include <Wire.h>

#define ATTEMPT_LIMIT 10
//Define pin of captors
#define LED_PIN A2 //16  led linked to the finite state of coffee machine : fix (before starting=LOW and after warming=HIGH), short blinking, fast blinking
#define INFRA_PIN A4 //for the infrared presence captor OK
#define BTN_ONOFF_PIN A0 //for the ON/OFF button
#define BTN_1COFFEE_PIN 9  //15=D4 for the button to make 1 coffee on ESP32 //for esp32s3 change by 9 
#define BTN_2COFFEE_PIN 5 //14=D2 for the button to make 2 coffee //for esp32s3 change by 5

//define param for water level
unsigned char water_level_high_data[12]={0};
unsigned char water_level_low_data[8] = {0};
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77

// Generate name for device
String getNameCoffeeMachine();
String deviceName;

//tasks and callbacks functions
void firebasePingTask(void* parameter);
void sensorsUpdateTask(void* parameter);
void requestTask(void* parameter);

//timer and interrupt for ledstate
TimerHandle_t xCalculatedLedFrequencyTimerHandle;
void vCalculateLedFrequencyTimerCallback(TimerHandle_t xTimer);// Callback function to calculate the LED frequency
void IRAM_ATTR checkLEDStateInterrupt(); // Interrupt function to check LED state by counting the number of state changes
//variables to get led state and calculate the frequency for changing status request in Firebase
volatile int stateChangeCount = 0;
float frequency = 0.0f;

// Handle saving wifi credential after BLE provisionning and restarting ESP
void handleBLEProvisionning(String ssid, String password);

// Check if wifi is not connected then it start the BLE provisionning
void checkWifi();

void setup() {
  pinMode(INFRA_PIN,INPUT);
  pinMode(LED_PIN,INPUT);
  pinMode(BTN_ONOFF_PIN,OUTPUT);
  pinMode(BTN_1COFFEE_PIN,OUTPUT);
  pinMode(BTN_2COFFEE_PIN,OUTPUT);
  
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Wire.begin();
  //clearEEPROM();
  deviceName = getNameCoffeeMachine();

  // Strings saved in EEPROM
  if(areStringsSaved()) {
    Serial.println("Retrieve wifi crendentials from EEPROM");
    String ssid = readStringFromEEPROM(START_ADDRESS + sizeof(STRING_MARKER));
    String password = readStringFromEEPROM(START_ADDRESS + sizeof(STRING_MARKER) + 1 + MAX_STRING_LENGTH);
    delay(500);
    Serial.println("Credentials found in EEPROM:");
    Serial.println(ssid);
    Serial.println(password);

    // Setup wifi with credentials from EEPROM
    Serial.println("Connecting to wifi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    delay(1000);

    checkWifi();
    Serial.println("Wifi connected");

    //Task to ping firebase
    xTaskCreate(firebasePingTask, "Ping Task", 4096, NULL, 1, NULL);//change priority to 1
    xTaskCreate(sensorsUpdateTask, "Sensors Task", 4096, NULL, 1, NULL);
    xTaskCreate(requestTask, "Requests Task", 8192, NULL, 1, NULL);
    attachInterrupt(digitalPinToInterrupt(LED_PIN),checkLEDStateInterrupt,HIGH);//CHANGE
    //for measuring led frequency
    xCalculatedLedFrequencyTimerHandle = xTimerCreate(
      "LedFrequencyTimer",
      pdMS_TO_TICKS(1000),
      pdTRUE,
      ( void * ) 0,
      vCalculateLedFrequencyTimerCallback
    );
    xTimerStart(xCalculatedLedFrequencyTimerHandle, 0);
    // Check if the timer was created successfully
    if (xCalculatedLedFrequencyTimerHandle != NULL) {
      // Start the timer
      if (xTimerStart(xCalculatedLedFrequencyTimerHandle, 0) != pdPASS) {
        Serial.println("Failed to start the led frequency timer");
      }
    } else {
      Serial.println("Failed to create the led frequency timer");
    }

  }
  // No string saved in EEPROM
  else {
    Serial.println("Start BLE Provisionning");
    SetupBLEServer::start(getInterfaceMacAddress(ESP_MAC_WIFI_STA), deviceName, false, handleBLEProvisionning);
    Serial.print("Waiting for provisionning");
    while(true) {
      delay(10000);
      Serial.print(".");
    }
  }
}

void loop() {
  checkWifi();

  //isCoffeeMachineReady();

  delay(1000);
}

/*
* Define all the methods
*/

String getNameCoffeeMachine() {
  String macAddress = getInterfaceMacAddress(ESP_MAC_WIFI_STA);
  unsigned long seed = 0 ;
  for(int i=0; i< macAddress.length();i++){
    seed = seed *256 + (unsigned char)macAddress[i];
  }
  srand(seed);
  const char hexChars[] = "0123456789ABCDEF";
  String name = "Coffee Machine (";
  for (int i = 0; i < 4; i++) {
      name += hexChars[rand() % 16];
  }
  return name += ")";
}

void handleBLEProvisionning(String ssid, String password) {
  Serial.println("SSID : " + ssid + " - Password : " + password);
  saveStringsToEEPROM(ssid, password);
  delay(1000);
  ESP.restart();
}

void checkWifi() {
  // Check if wifi is connected
  int attemptCounter = 0;
  while(WiFi.status() != WL_CONNECTED) {
    if(attemptCounter >= ATTEMPT_LIMIT) { // If too many attempt of reconnecting then restart BLE provisionning
      Serial.println("Can't connect to wifi");
      WiFi.disconnect(true); 
      WiFi.mode(WIFI_OFF);
      delay(5000);
      Serial.println("Restart BLE provisionning");
      SetupBLEServer::start(getInterfaceMacAddress(ESP_MAC_WIFI_STA), deviceName, true, handleBLEProvisionning);
      Serial.print("Waiting for provisionning");
      while(true) {
        delay(10000);
        Serial.print(".");
      }
    }
    // Try reconnecting to wifi
    Serial.println("Wifi not connected : try reconnecting...");
    WiFi.reconnect();
    attemptCounter++;
    delay(2000);
  }
}

// Function to send data and the ping to Firebase
void firebasePingTask(void* parameter) {
  String macAddress = getInterfaceMacAddress(ESP_MAC_WIFI_STA);
  while(true) {
    JsonDocument deviceStatus;
    deviceStatus["mac"] = macAddress;
    deviceStatus["lastOnline"] = Timestamp.get();
    deviceStatus["status"] = "ONLINE";
    deviceStatus["name"] = deviceName;
    String output;
    serializeJson(deviceStatus, output);
    Firebase firebaseData(FIREBASE_DB_URL,FIREBASE_API_KEY);
    firebaseData.setJson("devices/"+macAddress, output);
    //Serial.printf("timestamp= %i \n",((int)Timestamp.get()));
    vTaskDelay(pdMS_TO_TICKS(10000));//10000
  }
}

// Task to send sensors values to Firebase
void sensorsUpdateTask(void* parameter){
  String macAddress = getInterfaceMacAddress(ESP_MAC_WIFI_STA);
  bool value;
  while(true) {
    JsonDocument sensorsValues;
    float waterlevel = convertWaterSensorValues();
    sensorsValues["waterlevel"] = waterlevel;
    //Serial.print("waterlevel = ");
    //Serial.println(waterlevel);
    int detectionValue = analogRead(INFRA_PIN);
    //Serial.printf("IRcaptor = %i \n",detectionValue);
    if(detectionValue <= 1000){
      value = true; 
      //Serial.println("Cup detected");
    }
    else{
      value = false;
      //Serial.println("No Cup detected");
    }
    sensorsValues["presence"] = value;
    String data;
    serializeJson(sensorsValues, data);
    Firebase firebaseData(FIREBASE_DB_URL,FIREBASE_API_KEY);
    firebaseData.setJson("sensors/"+ macAddress, data);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


//for capting the waterlevel
void WaterLevelReadingSection() {
  // Clear the data buffers
  memset(water_level_low_data, 0, sizeof(water_level_low_data));
  memset(water_level_high_data, 0, sizeof(water_level_high_data));

  // Request and read low-level water data
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8); // sizeof(water_level_low_data)
  uint32_t start = millis();
  while (Wire.available() < 8) {
    // if (millis() - start > 1000) { // 100ms timeout
    //   Serial.println("Timeout while reading low-level water data");
    //   return;
    // }
  }
  for (int i = 0; i < 8; i++) {
    water_level_low_data[i] = Wire.read();
  }

  // Delay for stability
  delay(10);

  // Request and read high-level water data
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12); // sizeof(water_level_high_data)
  start = millis();
  while (Wire.available() < 12) {
    // if (millis() - start > 1000) { // 100ms timeout
    //   Serial.println("Timeout while reading high-level water data");
    //   return;
    // }
  }
  for (int i = 0; i < 12; i++) {
    water_level_high_data[i] = Wire.read();
  }

  // Delay for stability
  delay(10);
}

float convertWaterSensorValues() {
  uint32_t touch_val = 0;
  uint8_t trig_section = 0;

  // Read water level data
  WaterLevelReadingSection();

  // Process low-level water data
  for (int i = 0; i < 8; i++) { // sizeof(water_level_low_data)
    if (water_level_low_data[i] > 100) {
      touch_val |= 1 << i;
    }
  }

  // Process high-level water data
  for (int i = 0; i < 12; i++) { // sizeof(water_level_high_data)
    if (water_level_high_data[i] > 100) {
      touch_val |= (uint32_t)1 << (8 + i);
    }
  }

  // Calculate the triggered section
  while (touch_val & 0x01) {
    trig_section++;
    touch_val >>= 1;
  }

  return trig_section * 5 / 100.0f;
}

void requestTask(void* parameter) {
  String macAddress = getInterfaceMacAddress(ESP_MAC_WIFI_STA);//macAdress
  
  Firebase firebaseData(FIREBASE_DB_URL,FIREBASE_API_KEY);
  while(true) {
    String deviceRequestsJson = firebaseData.getJson("requests/"+macAddress);
    Serial.println(deviceRequestsJson);
    JsonDocument deviceRequests;
    deserializeJson(deviceRequests, deviceRequestsJson);
    String currentJson = deviceRequests["current"];
    String nextJson = deviceRequests["next"];
    String listJson = deviceRequests["list"];
    if(!currentJson.equalsIgnoreCase("null")) {
      JsonDocument current;
      deserializeJson(current, currentJson);
      JsonDocument sendRequest;
      sendRequest = current;
      
      JsonDocument sendLog;
      String status = current["status"];
      bool log = false;
      long timestamp = current["timestamp"];
      if(!status.equalsIgnoreCase("COLLECTING") && (Timestamp.get() - timestamp) > 300){//in case where the ESP shut down for any reason during the process, check after restarting it if the request is still valid (5min)
        sendLog = current;
        Serial.println("Request sup to 5min");
        Serial.println(Timestamp.get());
        sendLog["status"] = "ERROR_TIMEOUT";
        log = true;
      }
      if(status.equalsIgnoreCase("INITIALIZING")) {
      // Check if the coffee machine is ready to start a new request
        Serial.println("Initialising current request");

        // Check if the request is too old
        long timestamp = current["timestamp"];
        if(Timestamp.get() - timestamp > 30) { // Error timeout of 30s
          sendLog = current;
          sendLog["status"] = "ERROR_TIMEOUT";
          log = true;
          Serial.println(Timestamp.get());
          Serial.println("errorTimeout");
        }

        // Check if the coffee machine is ready to start a new request
        if(analogRead(INFRA_PIN) <= 1000 && convertWaterSensorValues() >= 0.1 && !log) {//waterlevel > 10% avoiding pump to suck in air 
          sendRequest["status"] = "WARMING";
        }
        // If there is no cup
        else if(analogRead(INFRA_PIN) > 1000 && !log){
          sendLog = current;
          sendLog["status"] = "ERROR_CUP";
          log = true;
        }
        // If there is not enough water
        else if (!log){
          sendLog = current;
          sendLog["status"] = "ERROR_WATER";
          log = true;
        }
      }
      else if(status.equalsIgnoreCase("WARMING")) { // Start warming the coffee machine and check when the coffee machine is warm
        Serial.println("Current request status : WARMING");
        // Start warming the coffee machine
        Serial.println("Start machine");
        digitalWrite(BTN_ONOFF_PIN, HIGH); //start machine by activating the button("relais ON/OFF")
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(BTN_ONOFF_PIN, LOW);
        
        // // Check if the coffee machine is warm
        // bool isReady = false;
        // int timeout = millis() + 60000;
        // while(!isReady && millis() < timeout) {
        //   vTaskDelay(pdMS_TO_TICKS(10000));
        //   isReady = isCoffeeMachineReady();
        // }

        // vTaskDelay(pdMS_TO_TICKS(60000));
        // if(isCoffeeMachineReady()) {//isReady
        //   sendRequest["status"] = "FILLING";
        // }
        // else {
        //   sendLog = current;
        //   sendLog["status"] = "ERROR_WARMING";
        //   log=true;
        // }

        // Assume the coffee machine is ready after 60s
        vTaskDelay(pdMS_TO_TICKS(60000));
        sendRequest["status"] = "FILLING";
      }
      else if(status.equalsIgnoreCase("FILLING")) { // Start filling the coffee machine and check when the cup is filled
        Serial.println("Current request status : FILLING");
        // Start filling the coffee machine
        String action = current["action"];
        if(action.equalsIgnoreCase("1CUP")) {
          Serial.println("Start filling 1 cup");
          digitalWrite(BTN_1COFFEE_PIN, HIGH); // Select 1 cup
          vTaskDelay(500);
          digitalWrite(BTN_1COFFEE_PIN, LOW);
          
        }
        else if(action.equalsIgnoreCase("2CUP")) {
          Serial.println("Start filling 2 cups");
          digitalWrite(BTN_2COFFEE_PIN, HIGH);
          vTaskDelay(500);
          digitalWrite(BTN_2COFFEE_PIN, LOW);

        }
        else {
          Serial.println("Unknown action");
          sendLog = current;
          sendLog["status"] = "ERROR_FILLING";
          log = true;
        }
      
        // // Check if the cup is filled
        // bool isReady = false;
        // int timeout = millis() + 60000;
        // while(!isReady && millis() < timeout && !log) {
        //   isReady = isCoffeeMachineReady();
        // }

        // Assume the cup is filled after 60s
        vTaskDelay(pdMS_TO_TICKS(60000));
        if(!log) {
          sendRequest["status"] = "COLLECTING";
        }
        else {
          sendLog = current;
          sendLog["status"] = "ERROR_FILLING";
          log = true;
        }
      }
      else if(status.equalsIgnoreCase("COLLECTING")) { // Wait for the user to collect the coffee
        Serial.println("Current request status : COLLECTING");
        // Wait for the user to collect the coffee
        digitalWrite(BTN_ONOFF_PIN, HIGH); //Turn off the machine
        vTaskDelay(500);
        digitalWrite(BTN_ONOFF_PIN, LOW);
        if (analogRead(INFRA_PIN) > 1000){ // Wait for cup to be removed
          Serial.println("Cup removed !");

          // Send log to Firebase
          sendLog = current;
          sendLog["status"] = "SUCCESS";
          log = true;
        }

      }
      else{
        Serial.println("Unknown status");
      }

      // Send log to Firebase if there is an log
      if(log) {
        // Send log to Firebase
        Serial.println("send log to firebase");
        String logJson;
        serializeJson(sendLog, logJson);
        Serial.println(logJson);
        String uid = sendLog["uid"];
        String timestamp = sendLog["timestamp"];
        firebaseData.setJson("logs/"+uid+"/"+timestamp,logJson);

        // Remove current request //remain to move the next as current and the list[0] to the next
        firebaseData.setString("requests/"+macAddress+"/current", "null");
      
      }
      else { //send sendRequest for updating the request status
        String requestJson;
        Serial.println("send requestJson");
        serializeJson(sendRequest, requestJson);
        firebaseData.setJson("requests/"+macAddress+"/current", requestJson);
      }
    }
    else {
      Serial.println("No current request");
      bool hasNext = false;
      //move the next to the current if next !=null
      if(!nextJson.equalsIgnoreCase("null")) {
        Serial.println("Check next request");
        hasNext = true;
        JsonDocument next;
        deserializeJson(next, nextJson);
        long timestamp = next["timestamp"];
        if (timestamp - Timestamp.get() < 15) { // Change the request status 15s before the request time
          Serial.println("Move next to current");
          next["status"] = "INITIALIZING";
          String requestJson;
          serializeJson(next, requestJson);
          firebaseData.setJson("requests/"+macAddress+"/current", requestJson);
          firebaseData.setString("requests/"+macAddress+"/next", "null");
          hasNext = false;
        }
      }
      if(!hasNext && !listJson.equalsIgnoreCase("null")) {
        Serial.println("Move first element of list to next");
        JsonDocument list;
        deserializeJson(list, listJson);
        String updatedNextJson;
        String updatedListJson;
        // Get the first element of the list and set it as the next request
        JsonObject listObject = list.as<JsonObject>();
        if(listObject.size() > 0){
          Serial.println("List not empty");
          updatedNextJson = ((String)listObject.begin()->value()); //next=list[0]
          //and then remove it from the list
          JsonObject::iterator it = listObject.begin();
          listObject.remove(it->key()); 

          Serial.print("listObject.size() =");
          Serial.println(listObject.size());
          
          if(listObject.size() > 0) {
            //list = listObject.as<JsonDocument>();
            serializeJson(list, updatedListJson);
          }
          else {
            updatedListJson = "null";
          }
        }
        Serial.print("updatedNextJson = ");
        Serial.println(updatedNextJson);
        Serial.print("updatedListJson = ");
        Serial.println(updatedListJson);
        firebaseData.setJson("requests/"+macAddress+"/next", updatedNextJson);
        if(updatedListJson.equalsIgnoreCase("null")) {
          firebaseData.setString("requests/"+macAddress+"/list", updatedListJson);
        }
        else {
          firebaseData.setJson("requests/"+macAddress+"/list", updatedListJson);
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// Interrupt function to check LED state by counting the number of state changes
void IRAM_ATTR checkLEDStateInterrupt() {
  static bool lastState = LOW;
  bool currentState = digitalRead(LED_PIN);
  if (currentState != lastState) {
    stateChangeCount++;
    lastState = currentState;
  }
}

// Callback function to calculate the LED frequency
void vCalculateLedFrequencyTimerCallback(TimerHandle_t xTimer){
  frequency = (stateChangeCount / 2.0f ); // Divide by 2 because each cycle has two state changes
  stateChangeCount = 0; // Reset the count for the next interval
}

//check if the coffee machine is ready to make coffee
bool isCoffeeMachineReady(){
  Serial.print("frequency =");
  Serial.println(frequency);
  Serial.println(digitalRead(LED_PIN));
  return frequency == 0.0f ;//&& digitalRead(LED_PIN)==HIGH;
}