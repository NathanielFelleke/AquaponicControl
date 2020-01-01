
#include <EEPROMex.h>

#include <Wire.h>

//Sensor Libraries
#include <DHT22.h>

#include <DFRobot_SHT20.h>

#include <GravityTDS.h>

#include <DS1307.h>

#include <HCSR04.h>


#define Master Serial
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

#define distanceTriggerPin 13

#define distanceEchoPin 12

//control pins

#define redLightPin 3
#define blueLightPin 4

#define pumpControlPin 27
#define mixerControlPin 28

#define humidityControlPin 29

//EEPROM Variables 
const int heightOfSensorEEPROM = 3;
const int waterDrainingIntervalEEPROM = 7;
const int HourLightOnEEPROM = 9;
const int HourLightOffEEPROM = 11;
const int MinuteLightOnEEPROM = 13;
const int MinuteLightOffEEPROM = 15;
const int tdsSensorCalEEPROM = 16;
const int pHIntervalEEPROM = 20;
const int wTIntervalEEPROM = 22;
const int wAIntervalEEPROM = 24;
const int tdsIntervalEEPROM = 26;
const int tIntervalEEPROM = 28;
const int pumpIntervalEEPROM = 30; 
const int calStatusEEPROM = 35;
const int CalLowEEPROM = 36;
const int CalLowpHEEPROM = 38;
const int CalMidEEPROM = 42;
const int CalMidphEEPROM = 44;
const int CalHighEEPROM = 48;
const int CalHighpHEEPROM = 50;
const int wantedHumidityEEPROM = 54;
const int wantedpHEEPROM = 55;
const int pHToleranceEEPROM = 59;
const int WantedWaterLevelEEPROM = 63;
const int WaterContainerAreaEEPROM = 67;
const int WantedAutomaticBrightnessEEPROM = 71;

//unit constants
const float cubicFeetToLitersConstant = 28.3168466;



int rtc[7];

String MasterSerialCommand; //serial input from computer
String ServerSerialCommand; //serial input from the ESP8266, which recieves commands from the Server
String WaterTestSerialCommand; //serial input from an arduino controlling the water liquid test kit (unused for now)
String FeederSerialCommand; //Serial input from arduino controlling the automatic feeders for the fish (unused for now)

//demensions
float WaterContainerArea = EEPROM.readFloat(WaterContainerAreaEEPROM);
float WantedWaterLevel = EEPROM.readFloat(WantedWaterLevelEEPROM);

//pH Variables
float pHReading;
float pHReading1;
float pHReading2;
float pHReading3;
float previouspHReading;
int AV;
float wantedpH;
float pHTolerance = EEPROM.readFloat(pHToleranceEEPROM);

unsigned int CalLow = 341;
float CalLowpH = 4.01;
unsigned int CalMid = 398;
float CalMidpH = 6.86;
unsigned int CalHigh = 443;
float CalHighpH = 9.18;
byte calStatus;
unsigned int pHTempInt;
bool pHEnabled = true;


//pH Control Peristaltic Pumps

float pHUpConstant;
float pHDownConstant;

float pHUpMolarity;
float pHDownMolarity;

int pHControlManuel;
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
int previousLiquidFilled= 0;
//Water Level

float distance;
float waterLevel;
float heightOfSensor; 
UltraSonicDistanceSensor distanceSensor(distanceTriggerPin, distanceEchoPin); //TODO move to setup()

//CC

int HourLightOn;
int MinuteLightOn; 
int HourLightOff; 
int MinuteLightOff;
int AutomaticLightControl = true;
//States 

float lightStates[] = {
  100,
  100
};

int WantedAutomaticBrightness;
 //Red State then Blue State

int pumpState = 0;

int mixerState = 0;

// intervals

unsigned long pHInterval;
unsigned long wTInterval; 
unsigned long wAInterval; 
unsigned long tdsInterval;
unsigned long tInterval; 
unsigned long pumpInterval; 
unsigned long rtcInterval; 
unsigned long distanceSensorInterval; 
unsigned long waterDrainingInterval; 


unsigned long pHPreviousMillis;
unsigned long wTPreviousMillis;
unsigned long wAPreviousMillis;  
unsigned long tdsPreviousMillis;
unsigned long tPreviousMillis;
unsigned long rtcPreviousMillis;
unsigned long distanceSensorPreviousMillis;
unsigned long pumpPreviousMillis;
unsigned long temporaryDistanceSensorPreviousMillis;

//Pumps

const float VolumeOfEzoTubing;

void setup() {
  Master.begin(9600);
  EZO.begin(9600);
  Server.begin(9600);

  //EEPROM.writeFloat(3,2.5);
  //EEPROM.writeInt(7, 300);
//TODO remove after writing once

  waterTemperatureSensor.initSHT20();
  //delay(10000);
  waterTemperatureSensor.checkSHT20();


  if (calStatus > 7) {
    calStatus = 0;
    EEPROM.update(calStatusEEPROM, calStatus);
  }
  Master.println(calStatus);
  if ((calStatus & 1) == 1) {
    CalLow = EEPROM.readInt(CalLowEEPROM);
    CalLowpH = EEPROM.readFloat(CalLowpHEEPROM);
    
  }
  Master.print("Low pH: ");
  Master.print(CalLow);
  Master.print("   ");
  Master.println(CalLowpH);

  if ((calStatus & 2) == 2) {
    CalMid = EEPROM.readInt(CalMidEEPROM);
    CalMidpH = EEPROM.readFloat(CalMidphEEPROM);
    
  }
  Master.print("Mid pH: ");
  Master.print(CalMid);
  Master.print("   ");
  Master.println(CalMidpH);
  if ((calStatus & 4) == 4) {
    CalHigh = EEPROM.readInt(CalHighEEPROM);
    CalHighpH = EEPROM.readFloat(CalHighpHEEPROM);
    
  }
  Master.print("High pH: ");
  Master.print(CalHigh);
  Master.print("   ");
  Master.println(CalHighpH);

  pHControlManuel = 0;
  waterTemperature = 25; //only sets number just in case other methods are dependent and to not break them
  heightOfSensor = EEPROM.readFloat(heightOfSensorEEPROM);
  wantedHumidity = (float)EEPROM.read(wantedHumidityEEPROM);
  HourLightOn = EEPROM.readInt(HourLightOnEEPROM);
  MinuteLightOn = EEPROM.readInt(MinuteLightOnEEPROM);
  HourLightOff = EEPROM.readInt(HourLightOffEEPROM);
  MinuteLightOff = EEPROM.readInt(MinuteLightOffEEPROM);
  AutomaticLightControl = true;
  WantedAutomaticBrightness = EEPROM.read(WantedAutomaticBrightnessEEPROM);
  wantedpH = EEPROM.readFloat(wantedpHEEPROM);

  pHInterval = (long)EEPROM.readInt(pHIntervalEEPROM) * (long)1000;
  wTInterval = (long)EEPROM.readInt(wTIntervalEEPROM) * (long)1000;
  wAInterval = (long)EEPROM.readInt(wAIntervalEEPROM) * (long)1000; 
  tdsInterval = (long)EEPROM.readInt(tdsIntervalEEPROM) * (long)1000;
  tInterval = (long)EEPROM.readInt(tIntervalEEPROM) * (long)1000;
  pumpInterval = (long)EEPROM.readInt(pumpIntervalEEPROM) * (long)1000;
  rtcInterval = (long)60000;
  distanceSensorInterval = (long)60000;
  waterDrainingInterval = (long)EEPROM.readInt(waterDrainingIntervalEEPROM) * (long)1000;
  AllDataInterval = 900000;

  pHPreviousMillis = 0;
  wTPreviousMillis = 0;
  wAPreviousMillis = 0;  
  tdsPreviousMillis = 0;
  tPreviousMillis = 0;
  rtcPreviousMillis = 0;
  distanceSensorPreviousMillis  = 0;
  pumpPreviousMillis = 0;
  AllDataPreviousMillis = 0;



  tdsSensor.setKvalueAddress(tdsSensorCalEEPROM); //sets the EEPROM address for the tds sensor k value
  tdsSensor.setPin(tdsSensorPin);
  tdsSensor.setAref(5.0);
  tdsSensor.setAdcRange(1024);
  tdsSensor.begin();

  pinMode(liquidFilledSensor, INPUT);

  pinMode(humidityControlPin,OUTPUT);
  pinMode(redLightPin, OUTPUT);
  pinMode(blueLightPin, OUTPUT);
  // TurnRedLightsOn(lightStates[0]);d]
  //TurnBlueLightsOn(lightStates[1]); 
  // TODO need to check if it is okay to use the method so early in their processes 
  analogWrite(redLightPin, lightStates[0]);
  analogWrite(blueLightPin, lightStates[1]);
  UpdateAllData(false);
  
}

void loop() {
  unsigned long currentMillis = millis();
   while (Server.available()) {
    delay(4);
    if (Server.available() > 0) {
      char c = Server.read();
      ServerSerialCommand += c; //updating the ServerSerialCommand
    }
  }

  while (Master.available()) {
    delay(4); //delay to allow buffer to fill
    if (Master.available() > 0) {
      char c = Master.read(); //gets one byte from serial buffer
      MasterSerialCommand += c; // Add bytes to string
    }
  } //End of while loop that reads string
  //time update
  if (ServerSerialCommand.length() > 0) {
    Master.println(ServerSerialCommand);
    if (ServerSerialCommand == "is") {
      delay(1000);
      
      UpdateAllData(false);
    }
    else if(ServerSerialCommand.startsWith("wph:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      wantedpH = ServerSerialCommand.toFloat();
      EEPROM.updateFloat(wantedpHEEPROM,wantedpH);
    }
    else if(ServerSerialCommand.startsWith("wh:")){
      ServerSerialCommand = ServerSerialCommand.substring(3);
      wantedHumidity = ServerSerialCommand.toFloat();
      EEPROM.update(wantedHumidityEEPROM, wantedHumidity); 
    }
    else if(ServerSerialCommand.startsWith("wpi:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      pumpInterval = (long)ServerSerialCommand.toInt() * (long)1000;
      EEPROM.updateInt(pumpIntervalEEPROM, ServerSerialCommand.toInt());
    }
    else if(ServerSerialCommand.startsWith("wdi:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      waterDrainingInterval = (long)ServerSerialCommand.toInt() * (long)1000;
      EEPROM.updateInt(waterDrainingIntervalEEPROM, ServerSerialCommand.toInt());
    }
    else if(ServerSerialCommand.startsWith("phi:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      pHInterval = (long)ServerSerialCommand.toInt() * (long)1000;
      EEPROM.updateInt(pHIntervalEEPROM, ServerSerialCommand.toInt());
      Master.print(pHInterval);
    }
    else if(ServerSerialCommand.startsWith("wti:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      wTInterval = (long)ServerSerialCommand.toInt() * (long)1000;
      EEPROM.updateInt(wTIntervalEEPROM, ServerSerialCommand.toInt());
    }
    else if(ServerSerialCommand.startsWith("asi:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      wAInterval = (long)ServerSerialCommand.toInt() * (long)1000;
      EEPROM.updateInt(wAIntervalEEPROM,ServerSerialCommand.toInt());
    }
    else if(ServerSerialCommand.startsWith("tdsi:")){
      ServerSerialCommand = ServerSerialCommand.substring(5);
      tdsInterval = (long)ServerSerialCommand.toInt() * (long)1000;
      EEPROM.updateInt(tdsIntervalEEPROM, ServerSerialCommand.toInt());
    }
    else if(ServerSerialCommand.startsWith("ti:")){
      ServerSerialCommand = ServerSerialCommand.substring(3);
      tInterval = (long)ServerSerialCommand.toInt() * (long)1000;
      EEPROM.updateInt(tIntervalEEPROM, ServerSerialCommand.toInt());
    }
    else if(ServerSerialCommand.startsWith("dsh:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      heightOfSensor = ServerSerialCommand.toFloat();
      EEPROM.updateFloat(heightOfSensorEEPROM,heightOfSensor);
    }
    else if(ServerSerialCommand.startsWith("wab:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      WantedAutomaticBrightness = ServerSerialCommand.toInt();
      EEPROM.update(WantedAutomaticBrightnessEEPROM, WantedAutomaticBrightness);
    }
    else if(ServerSerialCommand.startsWith("wwl:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      WantedWaterLevel = ServerSerialCommand.toFloat();
      EEPROM.updateFloat(WantedWaterLevelEEPROM, WantedWaterLevel);
    }
    else if(ServerSerialCommand.startsWith("wca:")){
      ServerSerialCommand = ServerSerialCommand.substring(4);
      WaterContainerArea = ServerSerialCommand.toFloat();
      EEPROM.updateFloat(WaterContainerAreaEEPROM, WaterContainerArea); //TODO finish
    }
    else if (ServerSerialCommand.startsWith("CL")) {
      ServerSerialCommand = ServerSerialCommand.substring(2);
      CalLowpH = ServerSerialCommand.toFloat();
      CalLow = pHGetMiddleAnalog();
      calStatus = calStatus | 1;
      EEPROM.update(calStatusEEPROM, calStatus);
      EEPROM.updateInt(CalLowEEPROM, CalLow);
      EEPROM.updateFloat(CalLowpHEEPROM, CalLowpH);
      
    } else if (ServerSerialCommand.startsWith("CM")) {
      ServerSerialCommand = ServerSerialCommand.substring(2);
      CalMidpH = ServerSerialCommand.toFloat();
      CalMid = pHGetMiddleAnalog();
      calStatus = calStatus | 2;
      EEPROM.update(calStatusEEPROM, calStatus);
      EEPROM.updateInt(CalMidEEPROM, CalMid);
      EEPROM.updateFloat(CalMidphEEPROM, CalMidpH);
      
    } else if (ServerSerialCommand.startsWith("CU")) {
      ServerSerialCommand = ServerSerialCommand.substring(2);
      CalHighpH = ServerSerialCommand.toFloat();
      CalHigh = pHGetMiddleAnalog();
      calStatus = calStatus | 4;
      EEPROM.update(calStatusEEPROM, calStatus);
      EEPROM.updateInt(CalHighEEPROM, CalHigh);
      EEPROM.updateFloat(CalHighpHEEPROM, CalHighpH);
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
    Master.println(MasterSerialCommand);
    if (MasterSerialCommand.startsWith("CL")) {
      MasterSerialCommand = MasterSerialCommand.substring(2);
      CalLowpH = MasterSerialCommand.toFloat();
      CalLow = pHGetMiddleAnalog();
      calStatus = calStatus | 1;
      EEPROM.update(calStatusEEPROM, calStatus);
      EEPROM.updateInt(CalLowEEPROM, CalLow);
      
      EEPROM.updateFloat(CalLowpHEEPROM, CalLowpH);
      
    } else if (MasterSerialCommand.startsWith("CM")) {
      MasterSerialCommand = MasterSerialCommand.substring(2);
      CalMidpH = MasterSerialCommand.toFloat();
      CalMid = pHGetMiddleAnalog();
      calStatus = calStatus | 2;
      EEPROM.update(calStatusEEPROM, calStatus);
      EEPROM.updateInt(CalMidEEPROM, CalMid);
      EEPROM.updateFloat(CalMidphEEPROM, CalMidpH);
      
    } else if (MasterSerialCommand.startsWith("CU")) {
      MasterSerialCommand = MasterSerialCommand.substring(2);
      CalHighpH = MasterSerialCommand.toFloat();
      CalHigh = pHGetMiddleAnalog();
      calStatus = calStatus | 4;
      EEPROM.update(calStatusEEPROM, calStatus);
      EEPROM.updateInt(CalHighEEPROM, CalHigh);
      EEPROM.updateFloat(CalHighpHEEPROM, CalHighpH);
    } else if (MasterSerialCommand.startsWith("CTDS")) {
      MasterSerialCommand = MasterSerialCommand.substring(4);
      tdsCalibrationValue = MasterSerialCommand.toFloat();
      tdsSensor.calibrate(tdsCalibrationValue, 25);
    }
    MasterSerialCommand = "";
  } // End of if (MasterSerialCommand.length() >0 )
  if(currentMillis - pHPreviousMillis > pHInterval){
    updatepH(true);
    if(pHReading > wantedpH + pHTolerance){
      pHDownAdjustment(pHDownAdjustmentCalculation());
    }
    else if(pHReading < wantedpH - pHTolerance){
      pHUpAdjustment(pHUpAdjustmentCalculation());
    }
    pHPreviousMillis = currentMillis;
  }
  if(currentMillis - pumpPreviousMillis > pumpInterval && !liquidFilled){
    updateWaterLevel(true);
    TurnPumpOn();
    pumpPreviousMillis = currentMillis;
  }
  
  else if(currentMillis - wTPreviousMillis > wTInterval){
    updateWaterTemperature(true);
    wTPreviousMillis = currentMillis;
  }
  else if(currentMillis - wAPreviousMillis > wAInterval){
    updateAirTemperature(true);
    wAPreviousMillis = currentMillis;
    if(insideHumidity< wantedHumidity - humidityTolerance){
      digitalWrite(humidityControlPin, HIGH);
    }
  }
  else if(currentMillis - tdsPreviousMillis> tdsInterval){
    updateTDS(true);
    tdsPreviousMillis = currentMillis;
  }
  else if(currentMillis - tPreviousMillis > tInterval){
    updateTurbidity(true);
    tPreviousMillis = currentMillis;
  }
  else if(currentMillis - distanceSensorPreviousMillis > distanceSensorInterval || currentMillis - temporaryDistanceSensorPreviousMillis > waterDrainingInterval){
    updateWaterLevel(true);
    distanceSensorPreviousMillis = currentMillis;
  }
  if(currentMillis - rtcPreviousMillis > rtcInterval){
    RTC.get(rtc,true);
    LightControl();
    //rtc[6]  // year rtc[5] //month rtc[4] //date rtc[2] //hour rtc[1] //min rtc[0] //sec rtc[3] //day of week
    rtcPreviousMillis = currentMillis;
  }
  if(pumpState){
    updateLiquidFilled(true);
    if(liquidFilled){
      TurnPumpOff();
      updateWaterLevel(true);
      temporaryDistanceSensorPreviousMillis = currentMillis;
    }
  }
  if(currentMillis - AllDataPreviousMillis > AllDataInterval){
    UpdateAllData();
  }
}
void UpdateAllData(bool individual){
  updateWaterTemperature(individual);
  updateAirTemperature(individual);
  updatepH(individual);
  updateTurbidity(individual);
  updateTDS(individual);
  updateWaterLevel(individual);
  if(!individual){
     Server.print((String)"{\"init\":true" + (String)",\"wt\":" + (String)waterTemperature +  (String)",\"iat\":" + (String)insideAirTemperature + (String)",\"ih\":" +  (String)insideHumidity + (String)",\"oat\":" + (String)outsideAirTemperature + (String)",\"oh\":" + (String)outsideHumidity + (String)",\"ph\":" + (String)pHReading + (String)",\"tv\":" + (String)turbidityVoltage + (String)",\"tds\":" + (String)tdsValue + (String)",\"wl\":" + (String)waterLevel + (String)",\"phi\":"  + (String)(pHInterval/1000) + (String)",\"wti\":" + (String)(wTInterval/1000) + (String)",\"tdsi\":" + (String)(tdsInterval/1000) + (String)",\"ti\":" + (String)(tInterval/1000) + (String)",\"asi\":" + (String)(wAInterval/1000) + (String)",\"wdi\":" + (String)(waterDrainingInterval/1000) + (String)",\"wpi\":" + (String)(pumpInterval/1000)+ (String)",\"wab\":" + (String)WantedAutomaticBrightness +  (String)",\"wph\":" + (String)wantedpH + (String)",\"wh\":" + (String)wantedHumidity + (String)",\"wwl\":" + (String)WantedWaterLevel + (String)",\"dsh\":" + (String)heightOfSensor + (String)",\"wca\":" + (String)WaterContainerArea (String)",\"pt\":" + (String)pHTolerance +(String)"}");
  }
}
void UpdateAll(bool individual) {
  updateWaterTemperature(individual);
  updateAirTemperature(individual);
  updatepH(individual);
  updateTurbidity(individual);
  updateTDS(individual);
  updateWaterLevel(individual);
  if(!individual){
    Server.print((String)"{\"init\":false" + (String)",\"wt\":" + (String)waterTemperature +  (String)",\"iat\":" + (String)insideAirTemperature + (String)",\"ih\":" +  (String)insideHumidity + (String)",\"oat\":" + (String)outsideAirTemperature + (String)",\"oh\":" + (String)outsideHumidity + (String)",\"ph\":" + (String)pHReading + (String)",\"tv\":" + (String)turbidityVoltage + (String)",\"tds\":" + (String)tdsValue + (String)",\"wl\":" + (String)waterLevel + (String)"}");
  }
}


void updatepH(bool individual) {
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
    if(individual){
      Server.print((String)"{\"ph\":" + (String)pHReading + (String)"}");
    }
  } // End of if PHReading > 2 && PHReading < 12
  else if(individual){    
    Server.print("{\"ph\":\"error\"}");
  }
}

void updateAirTemperature(bool individual) {
  insideAirTemperature = insideTemperatureSensor.getTemperatureC();
  outsideAirTemperature = outsideTemperatureSensor.getTemperatureC();

  insideHumidity = insideTemperatureSensor.getHumidity();
  outsideHumidity = outsideTemperatureSensor.getHumidity(); 
  if(individual){
    Server.print((String)"{\"iat\":" + (String)insideAirTemperature + (String)",\"ih\":" +  (String)insideHumidity + (String)",\"oat\":" + (String)outsideAirTemperature + (String)",\"oh\":" + (String)outsideHumidity + (String)"}");
  }
}

void updateWaterTemperature(bool individual) {
  waterTemperature = waterTemperatureSensor.readTemperature();
  if(individual){
    Server.print((String)"{\"wt\":" + (String)waterTemperature + (String)"}");
  }
}

void updateTurbidity(bool individual) {
  turbidityVoltage = turbidityGetMiddleAnalog() * (5.0 / 1024.0); //TODO check if turbidity math is correct
  if(individual){
    Server.print((String)"{\"tv\":" + (String)turbidityVoltage + (String)"}");
  }
}

void updateLiquidFilled(bool individual) {
  previousLiquidFilled = liquidFilled;
  liquidFilled = digitalRead(liquidFilledSensor);
  if(previousLiquidFilled != liquidFilled && individual){
    Server.print((String)"{\"lf\":" + (String)liquidFilled + (String)"}");
  }
}

void updateTDS(bool individual) {
  tdsSensor.setTemperature(waterTemperature);
  tdsSensor.update();
  tdsValue = tdsSensor.getTdsValue();
  ecValue = tdsSensor.getEcValue();
  if(individual){
    Server.print((String)"{\"tds\":" + (String)tdsValue + (String)"}");
  }
}

void updateWaterLevel(bool individual){
  distance = distanceSensor.measureDistanceCm()/100;
  waterLevel = (heightOfSensor - distance);
  if(individual){
    Server.print((String)"{\"wl\":" + (String)waterLevel + (String)"}");
  }
}
//Light Control

void LightControl(){

  if(AutomaticLightControl){
    if((!lightStates[0] || !lightStates[1]) && ((rtc[2]==HourLightOn && rtc[1]>=MinuteLightOn) ||(rtc[2]>HourLightOn && rtc[2]<HourLightOff) )){
      TurnAllLightsOn((float)WantedAutomaticBrightness);
    }
    else if((lightStates[0] || lightStates[1]) && ((rtc[2]==HourLightOff && rtc[1]>=MinuteLightOff) || (rtc[2]<=HourLightOn&& rtc[1] < MinuteLightOn ))){
      TurnAllLightsOff();
    }
  }
}

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
  Server.print((String)"{\"lsr\":" + (String)lightStates[0] + ", \"lsb\":" + (String)lightStates[1] + (String)"}"); 
}
void TurnAllLightsOff() {
  digitalWrite(redLightPin, LOW);
  digitalWrite(blueLightPin, LOW);
  lightStates[0] = 0;
  lightStates[1] = 0;

  Server.print((String)"{\"lsr\":" + (String)lightStates[0] + ", \"lsb\":" + (String)lightStates[1] + (String)"}");

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
  
  Server.print((String)"{\"lsr\":"+ (String)lightStates[0] + (String)"}");

}

void TurnRedLightsOff() {
  digitalWrite(redLightPin, LOW);
  lightStates[0] = 0;
  
  Server.print((String)"{\"lsr\":"+ (String)lightStates[0] + (String)"}");
 
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
  
  Server.print((String)"{\"lsb\":" + (String)lightStates[1] + (String)"}");

}

void TurnBlueLightsOff() {
  digitalWrite(blueLightPin, LOW);
  lightStates[1] = 0;
  
  Server.print((String)"{\"lsb\":" + (String)lightStates[1] + (String)"}");

}

void TurnPumpOn() {
  digitalWrite(pumpControlPin, HIGH);
  pumpState = 1;
  
  Server.print((String)"{\"ps\":" + (String)pumpState + (String)"}");
}

void TurnPumpOff() {
  digitalWrite(pumpControlPin, LOW);
  pumpState = 0;
  
  Server.print((String)"{\"ps\":" + (String)pumpState + (String)"}");
}


//Time Methods


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

float pHDownAdjustmentCalculation() {
  //will return the ml needed to get lower pH
  float pHDownVolume;

  return pHDownVolume;
}




void pHUpAdjustment(float volume){

}

void pHDownAdjustment(float volume){
  //if(pHDownPump == "EZO")
  //{
    //PrimeEzoPump();
    //EZO.print((String)"D, " + (String)volume);
  //}

}

void PrimeEzoPump()
{
  EZO.print((String)"D, " + (String)VolumeOfEzoTubing);
}

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

