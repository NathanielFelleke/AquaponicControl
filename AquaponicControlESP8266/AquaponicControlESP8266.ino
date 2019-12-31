
#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>



#include "config.h"

const int controlID = 0;
const char server[] = SERVER_IP;
int port = 3000;
//String readURI = server + "/control/" + controlID;

SocketIoClient socket;

const char ssid[] = WIFI_SSID;
const char password[] = WIFI_PASSWORD;


//data to receive


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
    socket.on("WantedAirSensorInterval", UpdateAirSensorInterval);
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
        else{
            socket.emit("controlUpdate","error");
        }
        SerialCommand = "";
    }
  socket.loop();
}


void initMainControl() {

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

void UpdatepHInterval(const char * payload, size_t length){
    Serial.print((String)"phi:" + (String)payload);
}

void UpdateWaterTemperatureInterval(const char * payload, size_t length){
    Serial.print((String)"wti:" + (String)payload);
}

void UpdateAirSensorInterval(const char * payload, size_t length){
    Serial.print((String)"asi" + (String)payload);
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
    Serial.print("is");
}
