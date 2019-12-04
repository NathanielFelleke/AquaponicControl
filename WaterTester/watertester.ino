#include <Wire.h>

#include <DFRobot_TCS34725.h>

float red;
float blue;
float green;
float clear;

DFRobot_TCS34725 tcs = DFRobot_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
void setup(){
if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
 
}

void loop(){
    tcs.getRGBC(&red, &green, &blue, &clear);
    tcs.lock(); 
}