
/*
Sketch developed by Caroline Smith for her Senior Design Project

Algorithm for reading IR distances based off of tutorial at "https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/" 

Algorithm for reading magnetic fields based off of tutorial at "http://arduinolearning.com/code/arduino-mlx90393-magnetic-field-sensor-example.php"
  and "https://learn.adafruit.com/mlx90393-wide-range-3-axis-magnetometer/arduino"
*/

// SharpIR library found at "https://github.com/guillaume-rico/SharpIR"
#include <SharpIR.h>

// Adafruit library for using the magnetometer, standard library
#include <Adafruit_MLX90393.h>

#include <Wire.h>

#define IRPin A0
#define model 1080

SharpIR ir = SharpIR(IRPin, model);
Adafruit_MLX90393 magnet = Adafruit_MLX90393();

int distance = 0;
float x, y, z;

float norm_x = 5;
float norm_y = 5;
float norm_z = 5;

float allowed_diff_mag = 5;

bool car_present = false;

void setup() {
  Serial.begin(9600);

  /* Wait for serial on USB platforms. */
  while(!Serial) {
      delay(10);
  }

  if (magnet.begin())
  {
    Serial.println("Found a MLX90393 sensor");
  }
  else
  {
    Serial.println("No sensor found ... check your wiring?");
    while (1);
  }
}

void loop() {

  //Read and print the distance from the IR sensor
  distance = ir.distance();
  
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

  //Report car sightings
  if (car_present){
    Serial.println("I do believe I see a car");
  }
  else {
    Serial.println("No cars here, boss");
  }
  
  delay(1000);
}
