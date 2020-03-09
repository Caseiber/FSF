#include <Wire.h>
 
#include "Adafruit_MLX90393.h"
 
Adafruit_MLX90393 sensor = Adafruit_MLX90393();

int num = 0;
 
void setup(void)
{
  Serial.begin(9600);
 
  /* Wait for serial on USB platforms. */
  while(!Serial) {
      delay(10);
  }
 
  Serial.println("Starting Adafruit MLX90393 Demo");
 
  if (sensor.begin())
  {
    Serial.println("Found a MLX90393 sensor");
  }
  else
  {
    Serial.println("No sensor found ... check your wiring?");
    while (1);
  }

  Serial.println("T     X     Y     Z");
}
 
void loop(void)
{
    float x, y, z;
 
    if(sensor.readData(&x, &y, &z)) {
        Serial.println(String(num)+ " " + String(x) + " " + String(y) + " " + String(z));
    } else {
        Serial.println("Unable to read XYZ data from the sensor.");
    }
    
    ++num;
    delay(100);
}
