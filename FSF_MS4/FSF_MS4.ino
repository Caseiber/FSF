/*
Sketch developed by Caroline Smith for her Senior Design Project

Algorithm for reading IR distances based off of tutorial at "https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/" using the library
from "https://github.com/zoubworldArduino/ZSharpIR"

Algorithm for reading magnetic fields based off of tutorial at "http://arduinolearning.com/code/arduino-mlx90393-magnetic-field-sensor-example.php"
  and "https://learn.adafruit.com/mlx90393-wide-range-3-axis-magnetometer/arduino" using the library from "https://github.com/tedyapo/arduino-MLX90393 by Theodore Yapo"

Algorithm for Bluetooth setup and transmission based off example at "https://github.com/tigoe/BluetoothLE-Examples/blob/master/ArduinoBLE_library_examples/BLE_accelerometer/BLE_accelerometer.ino"
and "https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Peripheral/ButtonLED/ButtonLED.ino"
*/

#include <Wire.h>
#include <ZSharpIR.h>
#include <MLX90393.h>

#include <ArduinoLowPower.h>
#include <ArduinoBLE.h>

// Define the IR and magnetic sensor
ZSharpIR ir = ZSharpIR(A2, 1080);
MLX90393 magnet;
MLX90393::txyz data;

// Define the Bluetooth parking service, defining the Arduino as the peripheral
BLEService parkingService("19B10010-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic taken("082b9438-e83c-11e8-9f32-f2801f1b9fd1", BLERead | BLENotify);

//Global variables to be accessed
int distance;

//Normal magnetic conditions and variance, to be calibrated later on
float norm_x = 0;
float norm_y = 0;
float norm_z = 0;

float allowed_diff_x = 0;
float allowed_diff_y = 0;
float allowed_diff_z = 0;

int allowed_distance = 450;

bool car_present = false;
int car_present_int = 0;

void setup() {
  
  Serial.begin(9600);
  Wire.begin();

  // Wait for the serial to begin transmitting
  while(!Serial);
  
  // Start the Bluetooth transmitter
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // Define the name of the device for the Bluetooth network
  BLE.setLocalName("FSF");
  
  BLE.setAdvertisedService(parkingService);

  // Add the characteristics to the service and add the service
  parkingService.addCharacteristic(taken);
  BLE.addService(parkingService);

  taken.writeValue(0);
  BLE.advertise();

  // Set up the magnet to begin taking measurements
  magnet.begin();
  magnet.setOverSampling(0);
  magnet.setDigitalFiltering(0);
 
  //Find the usual magnetic field for this area
  calibrate();
}

void loop() {
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      
      distance = ir.distance();
    
      // If there is any object within a certain distance of the sensor, check the magnetometer; if not, there is not a car present
      if (distance < allowed_distance) {
        car_present = checkMagField();
      }   
      else {
        car_present = false;
      }
    
      // Print whether a car was found and update the integer to be transmitted
      // Print statement will be removed when bluetooth is fully functional
      if (car_present){
        Serial.println("taken");
        car_present_int = 1;
      }
      else {
         Serial.println("open");
         car_present_int = 0;
      }
    
      // Update the peripheral with the status of the space
      taken.writeValue(car_present_int);

      // TODO: INCLUDE POWER MANAGEMENT
      //Go into low power mode for 5 minutes as soon as the check is done
      //LowPower.sleep(300000);
    }
  }

  delay(100);
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
  allowed_diff_x = abs(norm_x/5);
  allowed_diff_y = abs(norm_y/5);
  allowed_diff_z = abs(norm_z/5);

  // Very small allowed differences will cause issues because of minor fluctuations
  if (allowed_diff_x < 5) {
    allowed_diff_x = 5;
  }
  
  if (allowed_diff_y < 5) {
    allowed_diff_y = 5;
  }
  
  if (allowed_diff_z < 5) {
    allowed_diff_z = 5;
  }
}

// This function averages the magnetic field for 20 cycles, ten times, and see if it is within the allowed distance from normal 
bool checkMagField(){
    int loop_count = 0;
    int mag_check_count = 0;

    float sum_x = 0;
    float sum_y = 0;
    float sum_z = 0;

    float avg_x;
    float avg_y;
    float avg_z;
          
    while (loop_count < 10) {
      sum_x = 0;
      sum_y = 0;
      sum_z = 0;      
      
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
    return true;
  }
  else {
    return false;
  }
}
