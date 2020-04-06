// SharpIR library found at "https://github.com/guillaume-rico/SharpIR"
#include <ZSharpIR.h>

#include <Wire.h>
#include <ArduinoLowPower.h>
#include <ArduinoBLE.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#define IRPin A2
#define model 1080

ZSharpIR ir = ZSharpIR(IRPin, model);

BLEService parkingService("19B10010-E8F2-537E-4F6C-D104768A1214");

BLEIntCharacteristic taken("082b9438-e83c-11e8-9f32-f2801f1b9fd1", BLERead | BLENotify);


int distance = 0;
float x, y, z;

void setup() {
  Serial.begin(9600);

  /* TEST Wait for serial on USB platforms. */
  while(!Serial) {
      delay(10);
  }

  // Start the Bluetooth transmitter
  while (!BLE.begin()) {
    Serial.println("Waiting for BLE to start");
    delay(1);
  }

  // set the local name that the peripheral advertises:
  BLE.setLocalName("FSF");
  
  // set the UUID for the service:
  BLE.setAdvertisedService(parkingService);

  // add the characteristics to the service and add the service
  parkingService.addCharacteristic(taken);
  BLE.addService(parkingService);

  taken.writeValue(0);
  BLE.advertise();
}

void loop() {

 // Wifi.end();

  //Read and print the distance from the IR sensor
  distance = ir.distance();

  // TEST
  Serial.println("Distance: "+ String(distance) + " cm");

  // Communicate the status over Bluetooth
  BLEDevice central = BLE.central();

  // Check if the device is connected
  if (central) {
    // print the central's BT address:
    Serial.print("Connected to central: ");
    Serial.println(central.address());
  }
    
  // while the central remains connected, send the message
  while (central.connected()) {
    taken.writeValue(distance);
  }

  delay(2000);
}
