#include <EEPROM.h>
#include <Wire.h>

#include <DHT22.h>
#include "DFRobot_SHT20"

//sensor pins
#define pHSensor A0
#define pHSensorPowerPin 22
#define insideDHT22Pin 24
#define outsideDHT22Pin 25



//demensions
const float width 7
const float length 7
const float height 4
const float wantedWaterHeight 2



//pH Variables
float pHReading;
float pHReading1
float pHReading2;
float pHReading3;
int AV;

String CalCommand;
unsigned int CalLow = 292;
float CalLowpH = 4;
unsigned int CalMid = 497;
float CalMidpH = 6.8;
unsigned int CalHigh = 672;
float CalHighpH = 9.2;
byte highByte;
byte lowByte;
byte calStatus;
unsigned int pHTempInt;
bool PHEnabled = true;

//pH Control Peristaltic Pumps



//Temperature and Humidity Variables
//water
DFRobot_SHT20 waterTemperatureSensor;
float waterTemperature;

//air
DHT22 insideTemperatureSensor(insideDHT22Pin);

DHT22 outsideTemperatureSensor(outsideDHT22Pin);

float insideAirTemperature;
float outsideAirTemperature;
void setup(){
delay(2000);
}

void loop(){

}

void serialEvent(){

}

int pHGetMiddleAnalog(){

  int i;
  int j;
  int n = 5;
  int temp;

  int arr[5];
  for (i = 0; i < n; i++){
    arr[i] = analogRead(pHInputPin);
   
    delay(20);
  }
  for (i = 0; i < n; i++){
    for (j = 0; j < n-1; j++) {
      if (arr[j] > arr[j+1]) {
        int temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }
  return arr[2];
}