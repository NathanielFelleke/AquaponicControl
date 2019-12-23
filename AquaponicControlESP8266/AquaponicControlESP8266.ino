
#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>

#include <CayenneMQTTESP8266.h>

#include "config.h"

#define CAYENNE_PRINT Serial 

//ESP8266WiFi WiFi;
const int controlID = 0;
const char server[] = SERVER_IP;
int port = 3000;
//String readURI = server + "/control/" + controlID;

SocketIoClient socket;

const char ssid[] = WIFI_SSID;
const char password[] = WIFI_PASSWORD;

const char mqttUsername[] = MQTT_USERNAME;
const char mqttPassword[] = MQTT_PASSWORD;
const char mqttClientId[] = MQTT_CLIENT_ID;

//data to receive

float waterTemperature;
float wantedWaterTemperature;
float allowedLowWaterTemperature;
float allowedHighWaterTemperature;

float insideAirTemperature;
float outsideAirTemperature;
float wantedTemperature;
float allowedLowAirTemperature;
float allowedHighAirTemperature;

float insideHumidity;
float outsideHumidity;
float wantedHumidity;
float allowedLowHumidity;
float allowedHighHumidity;

float pHReading;
float wantedpH;
float allowedLowpH;
float allowedHighpH;


float turbidityVoltage;
float allowedLowTurbidity;
float allowedHighTurbidity;

float tdsValue;
float allowedLowTDS;
float allowedHighTDS;

float waterLevel;

int liquidFilled;

int pumpState;
int mixerState;

float lightStates[] = {100.0,100.0};
String SerialCommand;

void setup() {
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println("nc");
        delay(2000);
    }
    

    socket.begin(server, port);
    socket.on("connect", connectEvent);
    socket.on("WantedpH", UpdateWantedpH);
    socket.on("WantedHumidity", UpdateWantedHumidity);
    socket.on("WantedPumpInterval", UpdateWantedPumpInterval);
    socket.on("WaterDrainTime", UpdateWaterDrainTime);
    socket.on("DistanceSensorHeight", UpdateDistanceSensorHeight);
    socket.on("WantedpHInterval", UpdatepHInterval);
    socket.on("WantedWaterTemperatureInterval", UpdateWaterTemperatureInterval);
    socket.on("WantedAirTemperatureInterval", UpdateAirTemperatureInterval);
    socket.on("WantedTDSInterval", UpdateTDSInterval);
    socket.on("WantedTurbidityInterval", UpdateTurbidityInterval);
    socket.on("CalibrateLowerpH", UpdateLowerpHCalibrate);
    socket.on("CalibrateMiddlepH", UpdateMiddlepHCalibrate);
    socket.on("CalibrateUpperpH", UpdateUpperpHCalibrate);
    socket.on("CalibrateTDS", UpdateTDSCalibrate);
    socket.on("WantedWaterLevel", UpdateWantedWaterLevel);
    socket.on("WaterContainerArea", UpdateWaterContainerArea);
    socket.on("WantedAutomaticBrightness", UpdateWantedAutomaticBrightness);
    socket.on("pHTolerance", UpdatepHTolerance);


    //IPAdress localIP = WiFi.localIP();
    
  
}

void loop() {
    
    while (Serial.available()) {
        delay(4);
        if (Serial.available() > 0) {
            char c = Serial.read();
            SerialCommand += c;
        }
    }
    

    if (SerialCommand.length() > 0) {
      
      char SerialCommandChar[SerialCommand.length()];
      SerialCommand.toCharArray(SerialCommandChar,SerialCommand.length()+1); 
       if(SerialCommand.startsWith("{\"")){
          socket.emit("controlUpdate",SerialCommandChar);
        }
        

        
        //TODO add header that checks for the input and update the matching variable example= .startsWith("wt="")  
        //TODO Create Code that checks for serial coming from Arduino Master COntrol
        SerialCommand = "";
    }
    
  //TODO remove all cayenne stuff from arduino itself and move to server
  
  socket.loop();
}


void initMainControl() {

}

void initServer(){

}

void formatJSON(){

}



//handling methods for socket

void UpdateWantedpH(const char * payload, size_t length)
{
    Serial.print((String)"wph:" + (String)payload);
}

void UpdateWantedHumidity(const char * payload, size_t length){
    Serial.print((String)"wh:" + (String)payload);
}

void UpdateWantedPumpInterval(const char * payload, size_t length){
    Serial.print((String)"wpi:" + (String)payload);
}

void UpdateWaterDrainTime(const char * payload, size_t length){
    Serial.print((String)"wdi:" +  (String)payload);
}

void UpdateDistanceSensorHeight(const char * payload, size_t length){
    Serial.print((String)"dsh:" + (String)payload);

}
//TODO create many more handling methods

void UpdatepHInterval(const char * payload, size_t length){
    Serial.print((String)"phi:" + (String)payload);
}

void UpdateWaterTemperatureInterval(const char * payload, size_t length){
    Serial.print((String)"wti:" + (String)payload);
}

void UpdateAirTemperatureInterval(const char * payload, size_t length){
    Serial.print((String)"ati" + (String)payload);
}

void UpdateTDSInterval(const char * payload, size_t length){
    Serial.print((String)"tdsi:" + (String)payload);
}

void UpdateTurbidityInterval(const char * payload, size_t length){
    Serial.print((String)"ti:" + (String)payload);
}

void UpdateLowerpHCalibrate(const char * payload, size_t length){
    Serial.print((String)"CL" + (String)payload);
}

void UpdateMiddlepHCalibrate(const char * payload, size_t length){
    Serial.print((String)"CM" + (String)payload);
}

void UpdateUpperpHCalibrate(const char * payload, size_t length){
    Serial.print((String)"CU" + (String)payload);
}

void UpdateTDSCalibrate(const char * payload, size_t length){
    Serial.print((String)"CTDS" +(String)payload);
}

void UpdatepHTolerance(const char * payload, size_t length){
    Serial.print((String)"pht:" + (String)payload); //TODO integrate these into the server and the main control
}

void UpdateWantedAutomaticBrightness(const char * payload, size_t length){
    Serial.print((String)"wab:" + (String)payload);
}

void UpdateWantedWaterLevel(const char * payload, size_t length){
    Serial.print((String)"wwl:" + (String)payload);
}

void UpdateWaterContainerArea(const char * payload, size_t length){
    Serial.print((String)"wca:" + (String)payload);
}
void connectEvent(const char * payload, size_t length)
{
    socket.emit("isControlSocket", "1");
}
