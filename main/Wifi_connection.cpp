#include "Wifi_connection.h"
#include "Firebase_connection.h"

//a faire la connection au debut bluetooth puis wifi et recabler la cafetièere de florian et voir à boulanger un tag de localisation et un prot multi
//dans un premeir temps recoit en blutooth donnée connexion au wifi local puis après s'être connecté a la box l'esp devient AP
//partie Wifi
#include <WiFi.h>
#include "secrets.h"

//mode station

NetworkServer server(80);

void setUpWifiStation(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  setUpFirebase(WiFi.macAddress());//.c_str()
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  //faudra gérer déconnexion avec WiFi.disconnect();
}