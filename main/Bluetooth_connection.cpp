
//connexion bluetooth
// #include "BluetoothSerial.h"
// #include "Bluetooth_connection.h"

// String device_name = "ESP32-BT-Slave";
// BluetoothSerial SerialBT;

// void setBluetooth(){
//     // Check if Bluetooth is available
//   #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//   #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
//   #endif

//   // Check Serial Port Profile
//   #if !defined(CONFIG_BT_SPP_ENABLED)
//   #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
//   #endif
//   SerialBT.begin(device_name);
// }

// void bluetoothWriteRead(){
//   if (Serial.available()) {
//     SerialBT.write(Serial.read());
//   }
//   if (SerialBT.available()) {
//     Serial.write(SerialBT.read());
//   }
//   delay(20);
//   //Serial.BTreadStringUntil();
// }