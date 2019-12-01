#include <ESP8266HTTPClient.h>

#include <ESP8266WiFi.h>

#ifndef 
#define STASSID "" //Put SSID Here
#define STAPSK "" //Put Password Here
#endif

const int controlID = 0
String server = "http=//192.168.0.151";
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

float lightStates = {100.0,100.0};
String SerialCommand;

void setup() {
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    socket.on("wantedpH", UpdateWantedpH);
    socket.begin(server, port);
    delay(1000);
    IPAdress localIP = WiFi.localIP();
    Serial.print("c");
    socket.emit("isControl", "1");
    



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
        socket.emit("controlUpdate",SerialCommand);
        if(SerialCommand.startsWith("ph="))
        {
            socket.emit("")
            SerialCommand.substring(3);
            pHReading = SerialCommand.toFloat();
        } 
        else if(SerialCommand.startsWith("wt="))
        {
            
            SerialCommand.substring(3);
            waterTemperature = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWith("tds="))
        {
        
            SerialCommand.substring(4);
            tdsValue = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWith("iat="))
        {
           
        SerialCommand.substring(4);
        insideAirTemperature = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWith("ih="))
        {
           
            SerialCommand.substring(3);
            insideHumidity = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWith("oat="))
        {
          
            SerialCommand.substring(4);
            outsideAirTemperature = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWith("oh="))
        {
          
            SerialCommand.substring(3);
            outsideHumidity = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWIth("tv="))
        {
       
            SerialCommand.substring(3);
            turbidityVoltage = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWith("ps="))
        {
        
            SerialCommand.substring(3);
            pumpState = SerialCommand.toInt();
        }
        else if(SerialCommand.startsWIth("lf="))
        {
           
            SerialCommand.substring(3);
            liquidFilled = SerialCommand.toInt();
        }
        else if(SerialCommand.startsWith("ls="))
        {
           
            SerialCommand.substring(3);
            lightStates[0] = SerialCommand.toFloat();
            lightStates[1] = SerialCommand.toFloat();

        }
        else if(SerialCommand.startsWith("lsr="))
        {
            
            SerialCommand.substring(4);
            lightStates[0] = SerialCommand.toFloat();
        }
        else if(SerialCommand.startsWith("lsb="))
        {
            socket.emit();
            SerialCommand.substring(4);
            lightStates[1] = SerialCommand.toFLoat();
            //
        }
        //TODO add header that checks for the input and update the matching variable example= .startsWith("wt="")  
        //TODO Create Code that checks for serial coming from Arduino Master COntrol
        SerialCommand = "";
    }
    socket.loop();
}


// will use sockets as they are easeir to code and faster https://github.com/timum-viw/socket.io-client
/*String postRequest(String readData) {
   // String readData;
    String payload;
    int httpCode;

    HTTPClient http;

    //TODO Create readData

    http.begin(readURI);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    httpCode = http.POST(readData);
    payload = http.getString();
    http.end();

}
*/
void initMainControl() {

}

void initServer(){

}

void formatJSON(){

}


//handling methods for socket

void event(const char * payload, size_t length)
{

    Serial.print(payload);
}