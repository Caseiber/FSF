/*
Sketch developed by Caroline Smith for her Senior Design Project

Many sections of this sketch include status printing and are for testing purposes only and will not be included in the final product. They are marked with "TEST"

Algorithm for reading IR distances based off of tutorial at "https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/" 

Algorithm for reading magnetic fields based off of tutorial at "http://arduinolearning.com/code/arduino-mlx90393-magnetic-field-sensor-example.php"
  and "https://learn.adafruit.com/mlx90393-wide-range-3-axis-magnetometer/arduino"

Algorithm for Bluetooth setup and transmission based off example at "https://github.com/tigoe/BluetoothLE-Examples/blob/master/ArduinoBLE_library_examples/BLE_accelerometer/BLE_accelerometer.ino"
*/

// SharpIR library found at "https://github.com/guillaume-rico/SharpIR"
#include <ZSharpIR.h>

// Adafruit library for using the magnetometer, standard library
#include <Adafruit_MLX90393.h>

#include <Wire.h>
#include <ArduinoLowPower.h>
#include <ArduinoBLE.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#define IRPin A0
#define model 1080

ZSharpIR ir = ZSharpIR(IRPin, model);
Adafruit_MLX90393 magnet = Adafruit_MLX90393();

BLEService parkingService("082b91ae-e83c-11e8-9f32-f2801f1b9fd1");

BLEIntCharacteristic taken("082b9438-e83c-11e8-9f32-f2801f1b9fd1", BLERead | BLENotify);


int distance = 0;
float x, y, z;

float norm_x = 5;
float norm_y = 5;
float norm_z = 5;

float allowed_diff_mag = 10;
bool car_present = false;

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
  BLE.setLocalName("Sensor1");
  
  // set the UUID for the service:
  BLE.setAdvertisedService(parkingService);

  // add the characteristics to the service and add the service
  parkingService.addCharacteristic(taken);
  BLE.addService(parkingService);

  BLE.advertise();
}

void loop() {

 // Wifi.end();

  //Read and print the distance from the IR sensor
  distance = ir.distance();

  // TEST
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // If there is any object within 75 cm of the sensor, check the magnetometer; if not, there is not a car present
  if (distance < 75) {

        //Check if the magnetic field reading 10 times and see if it is within the allowed distance from normal
        int loop_count = 0;
        int mag_check_count = 0;
        
        while (loop_count < 10) {
          
           //Read and print the magnetic field from the magnetometer, confirming that the sensor is functioning
           if (magnet.readData(&x, &y, &z)) {
            // TEST
               Serial.print("X: "); Serial.print(x, 4); Serial.println(" uT");
               Serial.print("Y: "); Serial.print(y, 4); Serial.println(" uT");
               Serial.print("Z: "); Serial.print(z, 4); Serial.println(" uT");

               // Check if the data is within the allowed distances from normal
               if (abs(x - norm_x) > allowed_diff_mag || abs(y - norm_y) > allowed_diff_mag || abs(z - norm_z) > allowed_diff_mag) {
                   mag_check_count++;            
                   delay(500);
               }
               else {
                   mag_check_count = 0;
               }
           }
           else {
               Serial.println("Unable to read XYZ data from the sensor.");
           }
           loop_count++; 
        }
            
        // If the magnetic reading is off for ten consecutive readings, a car is present
        if (mag_check_count == 10) {
          car_present = true;
        }
        else {
          car_present = false;
        }
     }
  else {
    car_present = false;
  }

  //TEST Report car sightings
  if (car_present){
    Serial.println("taken");
  }
  else {
     Serial.println("open");
  }

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
      taken.writeValue(car_present);
    }

  //Go into low power mode for 5 minutes as soon as the check is done
  LowPower.sleep(3000);
}
