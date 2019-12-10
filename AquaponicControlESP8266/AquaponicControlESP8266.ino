
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>

#define STASSID "NattyNaomi" //Put SSID Here
#define STAPSK "dizzypiano750" //Put Password Here

//ESP8266WiFi WiFi;
const int controlID = 0;
char server[] = "192.168.0.102";
int port = 3000;
//String readURI = server + "/control/" + controlID;

SocketIoClient socket;

const char * ssid = STASSID;
const char * password = STAPSK;

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
    socket.on("wantedpH", UpdateWantedpH);
    socket.begin(server, port);
    socket.on("connect", connectEvent);
    //IPAdress localIP = WiFi.localIP();
    Serial.print("c");
    
    



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
      Serial.println(SerialCommandChar);
        socket.emit("controlUpdate",SerialCommandChar);
        
        //TODO add header that checks for the input and update the matching variable example= .startsWith("wt="")  
        //TODO Create Code that checks for serial coming from Arduino Master COntrol
        SerialCommand = "";
    }
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


//TODO create many more handling methods

void UpdatepHInterval(){
    Serial.print((String)"phi:" + (String)payload);
}

void UpdateWaterTemperatureInterval(){
    Serial.print((String)"wti:" + (String)payload);
}

void UpdateAirTemperatureInterval(){
    Serial.print((String)"ati" + (String)payload);
}

void UpdateTDSInterval(){
    Serial.print((String)"tdsi:" + (String)payload);
}

void UpdateTurbidityInterval(){
    Serial.print((String)"ti:" + (String)payload);
}


void connectEvent(const char * payload, size_t length)
{
    socket.emit("isControlSocket", "1");
}
