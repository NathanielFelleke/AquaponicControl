#include <EEPROM.h>

#include <Wire.h>

//Sensor Libraries
#include <DHT22.h>

#include <DFRobot_SHT20.h>

#include <GravityTDS.h>

#include <DS1307.h>


//sensor pins
#define pHSensor A0

#define pHSensorPowerPin 22

#define insideDHT22Pin 24

#define outsideDHT22Pin 25

#define tdsSensorPin A1

#define turbiditySensorPin A2

#define liquidFilledSensor 23

//control pins

# define redLightPin 3
# define blueLightPin 4


#define pumpControlPin 27
#define mixerControlPin 28

//unit constants

const float cubicFeetToLitersConstant = 28.3168466;

//time update

#define updata_flag 0 //1 updata time,0 only show time
//Modify the following data
# define TIME_YR 19# define TIME_MTH 11# define TIME_DATE 24# define TIME_DOW 7# define TIME_HR 2# define TIME_MIN 50# define TIME_SEC 0
char * str[] = {
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday"
}; //week
int rtc[7];

String MasterSerialCommand; //serial input from computer
String ServerSerialCommand; //serial input from the ESP8266, which recieves commands from the Server
String WaterTestSerialCommand; //serial input from an arduino controlling the water liquid test kit (unused for now)
String FeederSerialCommand; //Serial input from arduino controlling the automatic feeders for the fish (unused for now)

//demensions
const float containerWidth = 7;
const float containerLength = 7;
const float containerHeight = 4;
const float wantedWaterHeight = 2;

//pH Variables
float pHReading;
float pHReading1;
float pHReading2;
float pHReading3;
int AV;
float wantedpH;
float allowedLowpH;
float allowedHighpH;

unsigned int CalLow = 341;
float CalLowpH = 4.00;
unsigned int CalMid = 398;
float CalMidpH = 6.85;
unsigned int CalHigh = 443;
float CalHighpH = 9.17;
byte HighByte;
byte LowByte;
byte calStatus;
unsigned int pHTempInt;
bool pHEnabled = true;

//pH Control Peristaltic Pumps

float pHUpConstant;
float pHDownConstant;

float pHUpMolarity;
float pHDownMolarity;

//TDS Sensor Variables

GravityTDS tdsSensor;

float tdsValue;
float ecValue;
float allowedLowTDS;
float allowedHighTDS;


float tdsCalibrationValue;
//Temperature and Humidity Variables
//water
DFRobot_SHT20 waterTemperatureSensor;
float waterTemperature = 25;
float wantedWaterTemperature;
float allowedLowWaterTemperature;
float allowedHighWaterTemperature;

//turbidity
float turbidityVoltage;
float allowedLowTurbidity;
float allowedHighTurbidity;

//air
DHT22 insideTemperatureSensor(insideDHT22Pin);

DHT22 outsideTemperatureSensor(outsideDHT22Pin);

float insideAirTemperature;
float outsideAirTemperature;
float allowedLowAirTemperature;
float allowedHighAirTemperature;

float insideHumidity;
float outsideHumidity;
float wantedHumidity;
float allowedLowHumidity;
float allowedHighHumidity;

//Liquid Fill

int liquidFilled = 0;
//Water Level

//CC


//lights 

float lightStates = {
  100,
  100
}; //Red State then Blue State


void setup() {
  Serial.begin(9600);
  Serial.begin(9600);
  Serial3.begin(9600);


  waterTemperatureSensor.initSHT20();
  //delay(2000);
  waterTemperatureSensor.checkSHT20();

  calStatus = EEPROM.read(3);
  if (calStatus > 7) {
    calStatus = 0;
    EEPROM.write(3, calStatus);
  }
  Serial.println(calStatus);
  if ((calStatus & 1) == 1) {
    HighByte = EEPROM.read(4);
    LowByte = EEPROM.read(5);
    if (HighByte + LowByte < 510) {
      CalLow = (HighByte << 8) + LowByte;
    }
    HighByte = EEPROM.read(6);
    LowByte = EEPROM.read(7);
    if (HighByte + LowByte < 510) {
      pHTempInt = (HighByte << 8) + LowByte;
      CalLowpH = pHTempInt / 100.0;
    }
  }
  Serial.print("Low pH: ");
  Serial.print(CalLow);
  Serial.print("   ");
  Serial.println(CalLowpH);

  if ((calStatus & 2) == 2) {
    HighByte = EEPROM.read(8);
    LowByte = EEPROM.read(9);
    if (HighByte + LowByte < 510) {
      CalMid = (HighByte << 8) + LowByte;
    }
    HighByte = EEPROM.read(10);
    LowByte = EEPROM.read(11);
    if (HighByte + LowByte < 510) {
      pHTempInt = (HighByte << 8) + LowByte;
      CalMidpH = pHTempInt / 100.0;
    }
  }
  Serial.print("Mid pH: ");
  Serial.print(CalMid);
  Serial.print("   ");
  Serial.println(CalMidpH);
  if ((calStatus & 4) == 4) {
    HighByte = EEPROM.read(12);
    LowByte = EEPROM.read(13);
    if (HighByte + LowByte < 510) {
      CalHigh = (HighByte << 8) + LowByte;
    }
    Serial.println(CalHigh);
    HighByte = EEPROM.read(14);
    LowByte = EEPROM.read(15);
    if (HighByte + LowByte < 510) {
      pHTempInt = (HighByte << 8) + LowByte;
      CalHighpH = pHTempInt / 100.0;
    }
  }
  Serial.print("High pH: ");
  Serial.print(CalHigh);
  Serial.print("   ");
  Serial.println(CalHighpH);

  tdsSensor.setKvalueAddress(16); //sets the EEPROM address for the tds sensor k value
  tdsSensor.setPin(tdsSensorPin);
  tdsSensor.setAref(5.0);
  tdsSensor.setAdcRange(1024);
  tdsSensor.begin();

  pinMode(liquidLevelSensor, INPUT);

  pinMode(redLightPin, OUTPUT);
  pinMode(blueLightPin, OUTPUT);
 // TurnRedLightsOn(lightStates[0]);
 //TurnBlueLightsOn(lightStates[1]); 
// TODO need to check if it is okay to use the method so early in their processes 
  analogWrite(redLightPin, lightStates[0]);
  analogWrite(blueLightPin, lightStates[1]);
}

void loop() {

  while (Serial.available()) {
    delay(4); //delay to allow buffer to fill
    if (Serial.available() > 0) {
      char c = Serial.read(); //gets one byte from serial buffer
      MasterSerialCommand += c; // Add bytes to string
    }
  } //End of while loop that reads string
  //time update


  while (Serial1.avaiilable()) {
    delay(4);
    if (Serial1.available() > 0) {
      char c = Serial1.read();
      ServerSerialCommand += c; //updating the ServerSerialCommand
    }
  }

  if (ServerSerialCommand.length() > 0) {
    //TODO Create Code that checks for serial coming from ESP8266
    if (ServerSerialCommand == "C") {
      initServer();
    }

    ServerSerialCommand = "";
  }

  //checking for updates from the computer
  if (MasterSerialCommand.length() > 0) {
    MasterSerialCommand.toUpperCase();
    Serial.println(MasterSerialCommand);
    if (MasterSerialCommand.startsWith("CL")) {
      MasterSerialCommand = MasterSerialCommand.substring(2);
      CalLowpH = MasterSerialCommand.toFloat();
      CalLow = pHGetMiddleAnalog();
      calStatus = calStatus | 1;
      EEPROM.write(3, calStatus);
      EEPROM.write(4, highByte(CalLow));
      EEPROM.write(5, lowByte(CalLow));
      pHTempInt = CalLowpH * 100.0;
      EEPROM.write(6, highByte(pHTempInt));
      EEPROM.write(7, lowByte(pHTempInt));
    } else if (MasterSerialCommand.startsWith("CM")) {
      MasterSerialCommand = MasterSerialCommand.substring(2);
      CalMidpH = MasterSerialCommand.toFloat();
      CalMid = pHGetMiddleAnalog();
      calStatus = calStatus | 2;
      EEPROM.write(3, calStatus);
      EEPROM.write(8, highByte(CalMid));
      EEPROM.write(9, lowByte(CalMid));
      pHTempInt = CalMidpH * 100.0;
      EEPROM.write(10, highByte(pHTempInt));
      EEPROM.write(11, lowByte(pHTempInt));
    } else if (MasterSerialCommand.startsWith("CU")) {
      MasterSerialCommand = MasterSerialCommand.substring(2);
      CalHighpH = MasterSerialCommand.toFloat();
      CalHigh = pHGetMiddleAnalog();
      calStatus = calStatus | 4;
      EEPROM.write(3, calStatus);
      EEPROM.write(12, highByte(CalHigh));
      EEPROM.write(13, lowByte(CalHigh));
      pHTempInt = CalHighpH * 100.0;
      EEPROM.write(14, highByte(pHTempInt));
      EEPROM.write(15, lowByte(pHTempInt));
    } else if (MasterSerialCommand.startsWith("CTDS")) {
      MasterSerialCommand = MasterSerialCommand.substring(4);
      tdsCalibrationValue = MasterSerialCommand.toFloat();
      tdsSensor.calibrate(tdsCalibrationValue, 25);
    }
    MasterSerialCommand = "";
  } // End of if (MasterSerialCommand.length() >0 )



  //update values
  //get temperatures
  //
  //Serial.print(waterTemperature);

  showtime();
  delay(1000);
}

int pHGetMiddleAnalog() {

  int i;
  int j;
  int n = 5;
  int temp;

  int arr[5];
  for (i = 0; i < n; i++) {
    arr[i] = analogRead(pHSensor);

    delay(20);
  }
  for (i = 0; i < n; i++) {
    for (j = 0; j < n - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
  return arr[2];
}

unsigned int turbidityGetMiddleAnalog() {
  int i, j;
  int n = 5;
  int temp;
  int arr[5];
  for (i = 0; i < n; i++) {
    arr[i] = analogRead(turbiditySensorPin);
    delay(100);
  }
  for (i = 0; i < n; i++) {
    for (j = 0; j < n - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
  return arr[2];
} // End GetMiddleAnalog

float cubicFeetToLiters(float x) {
  return x * cubicFeetToLitersConstant;
}

float pHUpAdjustmentCalculation(float wantedpH, float currentpH) {
  //will return the mL needed to get wanted higher pH
  float pHUpVolume;

  return pHUpVolume;
}

float pHDownAdjustmentcalculation(float wantedpH, float currentpH) {
  //will return the ml needed to get lower pH
  float pHDownVolume;

  return pHDownVolume;
}



void updatepH() {

  AV = pHGetMiddleAnalog();
  if (AV > CalMid) {
    pHReading = CalMidpH + (CalHighpH - CalMidpH) / (CalHigh - CalMid) * (AV - CalMid);
  }
  if (AV <= CalMid) {
    pHReading = CalMidpH - (CalMidpH - CalLowpH) / (CalMid - CalLow) * (CalMid - AV);
  }

  if (pHReading > 2 && pHReading < 12) {
    pHReading3 = pHReading2;
    pHReading2 = pHReading1;
    pHReading1 = pHReading;
    Serial.print("pH = ");
    Serial.println(pHReading);
    Serial1.write("ph:" + pHReading);
  } // End of if PHReading > 2 && PHReading < 12
  else {
    Serial1.write("ph:error");
    Serial.println("pH Probe Error");
  }
  delay(50);

  //TODO write code that updates esp8266 through serial
}

void updateAirTemperature() {
  insideAirTemperature = insideTemperatureSensor.getTemperatureC();
  outsideAirTemperature = outsideTemperatureSensor.getTemperatureC();

  insideHumidity = insideTemperatureSensor.getHumidity();
  outsideHumidity = outsideTemperatureSensor.getHumidity();
  delay(50);
  Serial1.write("iat:" + (String) insideAirTemperature); // TODO check to see if works
  delay(50);
  Serial1.write("oat:" + (String) outsideAirTemperature);
  delay(50);
  Serial1.write("ih:" + (String) insideHumidity);
  delay(50);
  Serial1.write("oh:" + (String) outsideHumidity);
  //TODO write code that updates esp8266 through serial
}

void updateWaterTemperature() {
  waterTemperature = waterTemperatureSensor.readTemperature();
  delay(50);
  Serial1.write("wt:" + (String)waterTemperature); // TODO check to see if it works
  //TODO write code that updates esp8266 through serial
}

void updateTurbidity() {
  turbidityVoltage = turbidityGetMiddleAnalog(); //TODO check to see if it works
  delay(50);
  Serial1.write("tv:" + turbidityVoltage); // TODO check if it works
  //TODO write code that updates esp8266 through serial

}

void updateLiquidFilled() {
  liquidFilled = digitalRead(liquidFilledSensor);
  delay(50);
  Serial1.write("lf:" + (String) liquidFilled);
  //TODO write code that updates esp8266 through serial
}

void updateTDS() {
  tdsSensor.setTemperature(waterTemperature);
  tdsSensor.update();
  tdsValue = tdsSensor.getTdsValue();
  ecValue = tdsSensor.getEcValue();
  delay(50);
  Serial1.write("tds:" + (String)tdsValue);
  //TODO write code that updates esp8266 through serial
}

void updateAll() {
  updateWaterTemperature();
  updateAirTemperature();
  updatepH();
  updateTurbidity();
  updateTDS();
}

//Peristaltic Pump Control Functions

//Light Control

void TurnAllLightsOn(float brightness) {
  float analogBrightnessValue;
  float filteredBrightness;
  if (brightness > 100.0 || brightness < 0.0) {
    analogBrightnessValue = 255;
  } else {
    analogBrightnessValue = brightness * 2.55;
  }

  if (analogBrightnessValue == 255) {
    digitalWrite(blueLightPin, HIGH);
    digitalWrite(redLightPin, HIGH);
  } else {
    analogWrite(redLightPin, analogBrightnessValue);
    analogWrite(blueLightPin, analogBrightnessValue);
  }

  filteredBrightness = analogBrightnessValue / 2.55;

  lightStates[0] = filteredBrightness;
  lightStates[1] = filteredBrightness;
  delay(50);
  Serial1.write("ls:" + (String)lightStates[0]);
  //TODO write code that updates esp8266 through serial
}

void TurnAllLightsOff() {
  digitalWrite(redLightPin, LOW);
  digitalWrite(blueLightPin, LOW);
  lightStates[0] = 0;
  lightStates[1] = 0;
  delay(50);
  Serial1.write("ls:" + (String)lightStates[0]);
  //TODO write code that updates esp8266 through serial
}


void TurnRedLightsOn(float brightness) {
  float analogBrightnessValue;
  float filteredBrightness;
  if (brightness > 100.0 || brightness < 0.0) {
    analogBrightnessValue = 255;
  } else {
    analogBrightnessValue = brightness * 2.55;
  }

  if (analogBrightnessValue == 255) {
    digitalWrite(redLightPin, HIGH);
  } else {
    analogWrite(redLightPin, analogBrightnessValue);
  }

  filteredBrightness = analogBrightnessValue / 2.55;
  lightStates[0] = filteredBrightness;
  delay(50);
  Serial1.write("lsr:" + (String)lightStates[0]);
  //TODO write code that updates esp8266 through serial
}

void TurnRedLightsOff() {
  digitalWrite(redLightPin, LOW);
  lightStates[0] = 0;
  delay(50);
  Serial1.write("lsr:" + (String)lightStates[0]);
  //TODO write code that updates esp8266 through serial
}

void TurnBlueLightsOn(float brightness) {
  float analogBrightnessValue;
  float filteredBrightness;
  if (brightness > 100.0 || brightness < 0.0) {
    analogBrightnessValue = 255;
  } else {
    analogBrightnessValue = brightness * 2.55;
  }

  if (analogBrightnessValue == 255) {
    digitalWrite(blueLightPin, HIGH);
  } else {
    analogWrite(blueLightPin, analogBrightnessValue);
  }

  filteredBrightness = analogBrightnessValue / 2.55;
  lightStates[0] = filteredBrightness;
  delay(50);
  Serial1.write("lsb:" + (String)lightStates[1]);
  //TODO write code that updates esp8266 through serial
}

void TurnBlueLightsOff() {
  digitalWrite(blueLightPin, LOW);
  lightStates[1] = 0;
  delay(50);
  Serial1.write("lsb:" + (String)lightStates[1]);
  //TODO write code that updates esp8266 through serial
}

//Time Methods

void setTime(int sec, int min, int hour, int dow, int date, int month, int year) {
  RTC.get(rtc, true);
  RTC.stop();

  RTC.set(DS1307_SEC, rtc[0] + 1);
  RTC.start();
  if (updata_flag) {
    Serial.println(" OldTime:  ");
    showtime();
    RTC.stop();

    RTC.set(DS1307_SEC, sec); //---Update time----
    RTC.set(DS1307_MIN, min);
    RTC.set(DS1307_HR, hour);
    RTC.set(DS1307_DOW, dow);
    RTC.set(DS1307_DATE, date);
    RTC.set(DS1307_MTH, month);
    RTC.set(DS1307_YR, year);
    RTC.start();
    Serial.println(" SetTime:  ");
    showtime();
  }
}

void showtime() //
{
  RTC.get(rtc, true); {
    Serial.print(rtc[6]); /*YEAR MONTH DATE*/
    Serial.print("-");
    Serial.print(rtc[5]);
    Serial.print("-");
    Serial.println(rtc[4]);

    Serial.print(rtc[2]); /*HOUR  MIN SEC */
    Serial.print(":");
    Serial.print(rtc[1]);
    Serial.print(":");
    Serial.println(rtc[0]);

    Serial.println(str[rtc[3] - 1]); /********WEEK*****/
    Serial.println("**********");
  }
}

void initServer() {

}