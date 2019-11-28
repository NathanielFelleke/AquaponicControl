#include <ESP8266HTTPClient.h>

#include <ESP8266WiFi.h>

#
ifndef STASSID# define STASSID "" //Put SSID Here
# define STAPSK "" //Put Password Here
# endif

const int controlID = 0
String server = "http://192.168.0.151:3000";
String readURI = server + "/control/" + controlID;

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

String SerialCommand;

void setup() {
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    IPAdress localIP = WiFi.localIP();
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
        if(SerialCommand.startsWith("")); //TODO add header that checks for the input and update the matching variable example: .startsWith("wt:"")  
        //TODO Create Code that checks for serial coming from Arduino Master COntrol
        SerialCommand = "";
    }
}

String postRequest() {
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

void initMainControl() {

}