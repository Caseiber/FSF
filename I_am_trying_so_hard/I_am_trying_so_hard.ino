/*
Sketch developed by Caroline Smith for her Senior Design Project

Algorithm for reading IR distances based off of tutorial at "https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/" 

Algorithm for reading magnetic fields based off of tutorial at "http://arduinolearning.com/code/arduino-mlx90393-magnetic-field-sensor-example.php"
  and "https://learn.adafruit.com/mlx90393-wide-range-3-axis-magnetometer/arduino" using the library from "https://github.com/tedyapo/arduino-MLX90393 by Theodore Yapo"

Algorithm for Bluetooth setup and transmission based off example at "https://github.com/tigoe/BluetoothLE-Examples/blob/master/ArduinoBLE_library_examples/BLE_accelerometer/BLE_accelerometer.ino"
*/

#include <Wire.h>
#include <ZSharpIR.h>
#include <MLX90393.h>

// These files will be included when Bluetooth is used
//#include <ArduinoLowPower.h>
//#include <ArduinoBLE.h>
//#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WiFiServer.h>
//#include <WiFiUdp.h>

// Define the IR and magnetic sensor
ZSharpIR ir = ZSharpIR(A2, 1080);

MLX90393 magnet;
MLX90393::txyz data;

//Global variables to be accessed
int distance;

//Normal magnetic conditions, found experimentally
float norm_x = 0;
float norm_y = 0;
float norm_z = 0;

float allowed_diff_x = 0;
float allowed_diff_y = 0;
float allowed_diff_z = 0;
int allowed_distance = 450;
bool car_present = false;

int t = 0;

void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  
  magnet.begin();
  magnet.setOverSampling(0);
  magnet.setDigitalFiltering(0);

 // WiFi.end();
 
  // Find the usual magnetic field for this area
  calibrate();
}

void loop() {
 
  distance = ir.distance();

  // If there is any object within a certain distance of the sensor, check the magnetometer; if not, there is not a car present
  if (distance < allowed_distance) {

        //Average the magnetic field for 20 cycles, ten times, and see if it is within the allowed distance from normal
        int loop_count = 0;
        int mag_check_count = 0;

        float sum_x = 0;
        float sum_y = 0;
        float sum_z = 0;

        float avg_x;
        float avg_y;
        float avg_z;
              
        while (loop_count < 10) {
            //Read the magnetic field from the magnetometer and check if it is within allowed difference
              
            for (int i = 0; i < 20; i++){
                magnet.readData(data);
                sum_x += data.x;
                sum_y += data.y;
                sum_z += data.z;
            }
            
            avg_x = sum_x / 20;
            avg_y = sum_y / 20;
            avg_z = sum_z / 20;

            // Check if the data is within the allowed distances from normal
            if (abs(avg_x - norm_x) > allowed_diff_x || abs(avg_y - norm_y) > allowed_diff_y || abs(avg_z - norm_z) > allowed_diff_z) {  
              mag_check_count++;
              delay(100);
            }
            else {
              mag_check_count = 0;
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

  // Print whether a car was found; this is for testing only
  if (car_present){
    Serial.println("taken");
  }
  else {
     Serial.println("open");
  }

  delay(5000);
  
  //THE REST OF THIS BLOCK IS FOR FUTURE USE OF BLUETOOTH TRANSMISSION

  // Communicate the status over Bluetooth
  /*BLEDevice central = BLE.central();

  // Check if the device is connected
  if (central) {
    // print the central's BT address:
    Serial.print("Connected to central: ");
    Serial.println(central.address());
  }
    
    // while the central remains connected, send the message
    //    while (central.connected()) {
    //    taken.writeValue(car_present);
    }*/

  //Go into low power mode for 5 minutes as soon as the check is done
  //LowPower.sleep(3000);
}

// This function takes 20 readings of the magnetic field and sets the normal field measurements for x, y, and z
void calibrate() {

  float sum_x = 0;
  float sum_y = 0;
  float sum_z = 0;
  
  for (int i = 0; i < 20; i++){
    magnet.readData(data);
    sum_x += data.x;
    sum_y += data.y;
    sum_z += data.z;
  }

  norm_x = sum_x / 20;
  norm_y = sum_y / 20;
  norm_z = sum_z / 20;

  //Set the allowed difference to be 20% of each value
  allowed_diff_x = norm_x/5;
  allowed_diff_y = norm_y/5;
  allowed_diff_z = norm_z/5;
 
}
