# FSF (Free Space Finder)

This is for my senior engineering design project, which is a parking space sensor.

The program is made to be used with an Arduino MKR WiFi 1010 microcontroller. 

Current functionality allows it to calibrate itself when not under a car, and detect when a car is on top of it, and broadcast the status of its space as a BLE Peripheral. Later modifications will include battery saving functions.

### Prerequisites

The Arduino IDE is necessary to run this project, and can be found for free at Ardunio.cc. Additionally, it uses libraries written for a MLX90393 magnetometer (https://github.com/tedyapo/arduino-MLX90393) and a Sharp IR sensor (https://github.com/zoubworldArduino/ZSharpIR).

## Author

* **Caroline Smith**

## Acknowledgments

My algorithm for reading IR distances based off of tutorial at "https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/"

My algorithm for reading magnetic fields based off of tutorial at "http://arduinolearning.com/code/arduino-mlx90393-magnetic-field-sensor-example.php" and "https://learn.adafruit.com/mlx90393-wide-range-3-axis-magnetometer/arduino"

My algorithm for Bluetooth setup and transmission based off example at "https://github.com/tigoe/BluetoothLE-Examples/blob/master/ArduinoBLE_library_examples/BLE_accelerometer/BLE_accelerometer.ino" and "https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Peripheral/ButtonLED/ButtonLED.ino"
