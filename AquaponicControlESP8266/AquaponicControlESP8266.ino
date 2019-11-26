#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "" //Put SSID Here
#define STAPSK  "" //Put Password Here
#endif

const int controlID = 0
String server = "http://192.168.0.151:3000" ;
String readURI = server + "/control/" + controlID;

const char* ssid     = STASSID;
const char* password = STAPSK;

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
 
int isFilled;
int pumpState;
int mixerState                                                                                                    ;



void setup(){
    Serial.begin(9600);

    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
        delay(1000);
        Serial.print("Connecting");
    }

    Serial.println("Connected");

    IPAdress localIP = WiFi.localIP();


}

void loop(){

}

String postRequest(){
    String readData;
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

