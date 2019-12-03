
#include <EEPROMex.h>

#include <Wire.h>

//Sensor Libraries
#include <DHT22.h>

#include <DFRobot_SHT20.h>

#include <GravityTDS.h>

#include <DS1307.h>

#define Server Serial3

#define EZO Serial1

//sensor pins
#define pHSensor A0

#define pHSensorPowerPin 22

#define insideDHT22Pin 24

#define outsideDHT22Pin 25

#define tdsSensorPin A1

#define turbiditySensorPin A2

#define liquidFilledSensor 23

//control pins

#define redLightPin 3
#define blueLightPin 4


#define pumpControlPin 27
#define mixerControlPin 28

#define humidityControlPin 29
//unit constants
const float cubicFeetToLitersConstant = 28.3168466;



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
float previouspHReading;
int AV;
float wantedpH;
float pHTolerance = 0.4;

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

int phControManuel = 0;
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
float humidityTolerance = 5.0;

//Liquid Fill

int liquidFilled = 0;
//Water Level

//CC


//States 

float lightStates[] = {
  100,
  100
}; //Red State then Blue State

int pumpState = 0;

int mixerState = 0;

// intervals

unsigned long pHInterval = (long)EEPROM.readInt(20) * (long)1000;
unsigned long wTInterval = (long)EEPROM.readInt(22) * (long)1000;
unsigned long wAInterval = (long)EEPROM.readInt(24) * (long)1000;
unsigned long tdsInterval = (long)EEPROM.readInt(26) * (long)1000;
unsigned long tInterval = (long)EEPROM.readInt(28) * (long)1000;


unsigned long pHPreviousMillis = 0;
unsigned long wTPreviousMillis = 0;
unsigned long wAPreviousMillis = 0;
unsigned long tdsPreviousMillis = 0;
unsigned long tPreviousMillis = 0;


//Pumps

const float VolumeOfEzoTubing;

void setup() {
  Serial.begin(9600);
  EZO.begin(9600);
  Server.begin(9600);


  waterTemperatureSensor.initSHT20();
  //delay(2000);
  waterTemperatureSensor.checkSHT20();

 
  Serial.println(pHInterval);
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

  
  wantedHumidity = (float)EEPROM.read(30);

  wantedpH = EEPROM.readFloat(31);


  tdsSensor.setKvalueAddress(16); //sets the EEPROM address for the tds sensor k value
  tdsSensor.setPin(tdsSensorPin);
  tdsSensor.setAref(5.0);
  tdsSensor.setAdcRange(1024);
  tdsSensor.begin();

  pinMode(liquidFilledSensor, INPUT);

  pinMode(redLightPin, OUTPUT);
  pinMode(blueLightPin, OUTPUT);
  // TurnRedLightsOn(lightStates[0]);d]
  //TurnBlueLightsOn(lightStates[1]); 
  // TODO need to check if it is okay to use the method so early in their processes 
  analogWrite(redLightPin, lightStates[0]);
  analogWrite(blueLightPin, lightStates[1]);
  updateAll();
}

void loop() {
  unsigned long currentMillis = millis();
  while (Serial.available()) {
    delay(4); //delay to allow buffer to fill
    if (Serial.available() > 0) {
      char c = Serial.read(); //gets one byte from serial buffer
      MasterSerialCommand += c; // Add bytes to string
    }
  } //End of while loop that reads string
  //time update


  while (Server.available()) {
    delay(4);
    if (Server.available() > 0) {
      char c = Server.read();
      ServerSerialCommand += c; //updating the ServerSerialCommand
    }
  }

  if (ServerSerialCommand.length() > 0) {
    //TODO Create Code that checks for serial coming from ESP8266
    if (ServerSerialCommand == "c") {
      delay(1000);
      
      initServer();
    }
    if (ServerSerialCommand.startsWith("CL")) {
      ServerSerialCommand = ServerSerialCommand.substring(2);
      CalLowpH = ServerSerialCommand.toFloat();
      CalLow = pHGetMiddleAnalog();
      calStatus = calStatus | 1;
      EEPROM.write(3, calStatus);
      EEPROM.write(4, highByte(CalLow));
      EEPROM.write(5, lowByte(CalLow));
      pHTempInt = CalLowpH * 100.0;
      EEPROM.write(6, highByte(pHTempInt));
      EEPROM.write(7, lowByte(pHTempInt));
    } else if (ServerSerialCommand.startsWith("CM")) {
      ServerSerialCommand = ServerSerialCommand.substring(2);
      CalMidpH = ServerSerialCommand.toFloat();
      CalMid = pHGetMiddleAnalog();
      calStatus = calStatus | 2;
      EEPROM.write(3, calStatus);
      EEPROM.write(8, highByte(CalMid));
      EEPROM.write(9, lowByte(CalMid));
      pHTempInt = CalMidpH * 100.0;
      EEPROM.write(10, highByte(pHTempInt));
      EEPROM.write(11, lowByte(pHTempInt));
    } else if (ServerSerialCommand.startsWith("CU")) {
      ServerSerialCommand = ServerSerialCommand.substring(2);
      CalHighpH = ServerSerialCommand.toFloat();
      CalHigh = pHGetMiddleAnalog();
      calStatus = calStatus | 4;
      EEPROM.write(3, calStatus);
      EEPROM.write(12, highByte(CalHigh));
      EEPROM.write(13, lowByte(CalHigh));
      pHTempInt = CalHighpH * 100.0;
      EEPROM.write(14, highByte(pHTempInt));
      EEPROM.write(15, lowByte(pHTempInt));
    } else if (ServerSerialCommand.startsWith("CTDS")) {
      ServerSerialCommand = ServerSerialCommand.substring(4);
      tdsCalibrationValue = ServerSerialCommand.toFloat();
      tdsSensor.calibrate(tdsCalibrationValue, 25);
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
  if(currentMillis - pHPreviousMillis > pHInterval){
    updatepH();
    if(pHReading > wantedpH + pHTolerance){
      pHDownAdjustment(pHDownAdjustmentcalculation());
    }
    else(pHReading < wantedpH - pHTolerance){
      pHUpAdjustment(pHUpAdjustmentCalculation());
    }
    pHPreviousMillis = currentMillis;
  }
  else if(currentMillis - wTPreviousMillis > wTInterval){
    updateWaterTemperature();
    wTPreviousMillis = currentMillis;
  }
  else if(currentMillis - wAPreviousMillis > wAInterval){
    updateAirTemperature();
    wAPreviousMillis = currentMillis;
    if(insideHumidity<wantedHumidity - humidityTolerance){
      digitalWrite(humidityControlPin, HIGH);
    }
    
  }
  else if(currentMillis - tdsPreviousMillis> tdsInterval){
    updateTDS();
    tdsPreviousMillis = currentMillis;
  }
  else if(currentMillis - tPreviousMillis > tInterval){
    updateTurbidity();
    tPreviousMillis = currentMillis;
  }
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

float pHUpAdjustmentCalculation() {
  //will return the mL needed to get wanted higher pH
  float pHUpVolume;
  
  return pHUpVolume;
}

float pHDownAdjustmentcalculation() {
  //will return the ml needed to get lower pH
  float pHDownVolume;

  return pHDownVolume;
}



void updatepH() {
  previouspHReading = pHReading;
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
    Server.print((String)"{\"ph\":" + (String)pHReading + (String)"}");
  } // End of if PHReading > 2 && PHReading < 12
  else {    // "{\"foo\":\"bar\"}"
    Server.print("{\"ph\":\"error\"}");
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
  Server.print((String)"{\"iat\":" + (String)"\"" + (String)insideAirTemperature + (String)"\"}");
  delay(50);
   // TODO check to see if works
  Server.print((String)"{\"ih\":" + (String)"\"" + (String)insideHumidity + (String)"\"}");
  delay(50);
  Server.print((String)"{\"oat\":" + (String)"\"" + (String)outsideAirTemperature + (String)"\"}");
  delay(50);
  Server.print((String)"{\"oh\":" + (String)"\"" + (String)outsideHumidity + (String)"\"}");
  //TODO write code that updates esp8266 through serial
}

void updateWaterTemperature() {
  waterTemperature = waterTemperatureSensor.readTemperature();
  delay(50);
  Server.print((String)"{\"wt\":" + (String)waterTemperature + (String)"}");
  // TODO check to see if it works
  //TODO write code that updates esp8266 through serial
}

void updateTurbidity() {
  turbidityVoltage = turbidityGetMiddleAnalog(); //TODO check to see if it works
  delay(50);
  Server.print((String)"{\"tv\":" + (String)turbidityVoltage + (String)"}");
  // TODO check if it works
  //TODO write code that updates esp8266 through serial

}

void updateLiquidFilled() {
  liquidFilled = digitalRead(liquidFilledSensor);
  delay(50);
  Server.print((String)"{\"lf\":" + (String)liquidFilled + (String)"}");
  //TODO write code that updates esp8266 through serial
}

void updateTDS() {
  tdsSensor.setTemperature(waterTemperature);
  tdsSensor.update();
  tdsValue = tdsSensor.getTdsValue();
  ecValue = tdsSensor.getEcValue();
  delay(50);
  Server.print((String)"{\"tds\":" + (String)tdsValue + (String)"}");
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
  Server.print((String)"{\"lsr\":" + (String)lightStates[0] + ", \"lsb\":" + (String)lightStates[1] + (String)"}");
  //TODO write code that updates esp8266 through serial
}

void TurnAllLightsOff() {
  digitalWrite(redLightPin, LOW);
  digitalWrite(blueLightPin, LOW);
  lightStates[0] = 0;
  lightStates[1] = 0;
  delay(50);
  Server.print((String)"{\"lsr\":" + (String)lightStates[0] + ", \"lsb\":" + (String)lightStates[1] + (String)"}");
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
  Server.print((String)"{\"lsr\":"+ (String)lightStates[0] + (String)"}");
  //TODO write code that updates esp8266 through serial
}

void TurnRedLightsOff() {
  digitalWrite(redLightPin, LOW);
  lightStates[0] = 0;
  delay(50);
  Server.print((String)"{\"lsr\":"+ (String)lightStates[0] + (String)"}");
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
  Server.print((String)"{\"lsb\":" + (String)lightStates[1] + (String)"}");
 
  //TODO write code that updates esp8266 through serial
}

void TurnBlueLightsOff() {
  digitalWrite(blueLightPin, LOW);
  lightStates[1] = 0;
  delay(50);
  Server.print((String)"{\"lsb\":" + (String)lightStates[1] + (String)"}");
  //TODO write code that updates esp8266 through serial
}


void TurnPumpOn() {
  digitalWrite(pumpControlPin, HIGH);
  pumpState = 1;
  delay(50);
  Server.print((String)"{\"ps\":" + (String)pumpState + (String)"}");
}

void TurnPumpOff() {
  digitalWrite(pumpControlPin, LOW);
  pumpState = 0;
  delay(50);
  Server.print((String)"{\"ps\":" + (String)pumpState + (String)"}");
}

void TurnMixerOn() {
  digitalWrite(mixerControlPin, HIGH);
  mixerState = 1;
  delay(50);
  Server.print((String)"{\"ms\":" +  (String)mixerState + (String)"}");
}

void TurnMixerOff() {
  digitalWrite(mixerControlPin, LOW);
  mixerState = 0;
  delay(50);
  Server.print((String)"{\"ms\":"  + (String)mixerState + (String)"}");
}
//Time Methods

void setTime(int sec, int min, int hour, int dow, int date, int month, int year) {
    RTC.stop();

    RTC.set(DS1307_SEC, sec); //---Update time----
    RTC.set(DS1307_MIN, min);
    RTC.set(DS1307_HR, hour);
    RTC.set(DS1307_DOW, dow);
    RTC.set(DS1307_DATE, date);
    RTC.set(DS1307_MTH, month);
    RTC.set(DS1307_YR, year);
    RTC.start();
}

void initServer() {
  Server.write("");
}


void pHUpAdjustment(float volume){

}

void pHDownAdjustment(float volume){
  if(pHDownPump == "EZO")
  {
    PrimeEzoPump();
    EZO.print((String)"D, " + (String)volume);
  }

}

void PrimeEzoPump()
{
  EZO.print((String)"D, " + (String)VolumeOfEzoTubing);
}