#include <WiFi.h>
#include "TimestampManager.h"

const char *ssid = "Galaxy A42 5G8288";
const char *password = "avmw4585";

//TimestampManager tm;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  ManagerTimestamp.setConfigTime(60000);
}

void loop() {
  Serial.print("Timestamp generated = ");
  Serial.println(ManagerTimestamp.timestampGenerator());
  delay(1000);
}

