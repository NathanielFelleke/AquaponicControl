#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#include <Arduino.h>
#include <SPI.h>

#include <Adafruit_ILI9341esp.h>
#include <Adafruit_GFX.h>
#include <XPT2046.h>
#include <Wiegand.h>

WIEGAND wg;


#ifndef STASSID
#define STASSID "" //Put SSID Here
#define STAPSK  "" //Put Password Here
#endif


#define TFT_DC 2
#define TFT_CS 15


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
XPT2046 touch(/*cs=*/ 4, /*irq=*/ 5);

Adafruit_GFX_Button button;


const char* ssid     = STASSID;
const char* password = STAPSK;
int scannerId = 0;

String hidCode = "0";
String server = "http://192.168.0.151:3000" ;
String readURI =server + "/scanner/read/" + scannerId;

String readData;

String lightURI = server + "/scanner/light/" + scannerId;

String lightData = "scannerId=0&room=Bedroom";

extern uint8_t door[];

extern uint8_t LightON[];
extern uint8_t LightOFF[];

boolean lightState;
boolean prevLightState;

void setup() {
    delay(5000);
  
  Serial.begin(115200);


   SPI.setFrequency(ESP_SPI_FREQ);

  tft.begin();
  touch.begin(tft.width(), tft.height());  // Must be done before setting rotation
  Serial.print("tftx ="); Serial.print(tft.width()); Serial.print(" tfty ="); Serial.println(tft.height());
  tft.fillScreen(ILI9341_BLACK);
  // Replace these for your screen module
  touch.setCalibration(1824, 264, 266, 1744);


  
 WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 


  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
tft.setRotation(3);
  
lightState = false;
prevLightState = lightState;


  wg.begin(0,3);
}



void loop() {

tft.drawBitmap(-30,40,door,240,160,ILI9341_WHITE);
  uint16_t x, y;
  if (touch.isTouching()) {
    touch.getPosition(x, y);
Serial.print("x ="); Serial.print(x); Serial.print(" y ="); Serial.println(y);
    if(y>160){
      Serial.println("Door");
     
    }
    
    else{

        HTTPClient http;
  http.begin(lightURI);
  http.addHeader("Content-Type","application/x-www-form-urlencoded");
  int httpCode = http.POST(lightData);
  String payload = http.getString();
 Serial.println(httpCode);
 Serial.println(payload);
 if(payload=="1"){

  lightState=true;
 }
 else{

  lightState=false;
 }
 http.end();
    
      Serial.println("Light");
    }
  } 
  



       
 if(Serial.available()>2){
  Serial.println(Serial.println());
  hidCode = Serial.read();

  readData = "hidCode=" + hidCode + "&scannnerId="+scannerId;
Serial.println(readURI);
  Serial.println(readData);
 
  HTTPClient http;
  http.begin(readURI);
  http.addHeader("Content-Type","application/x-www-form-urlencoded");
  int httpCode = http.POST(readData);
  String payload = http.getString();
 Serial.println(httpCode);
 Serial.println(payload);
 http.end();
 
        
}
if(lightState){


  tft.drawBitmap(130,40,LightON,240,160,ILI9341_YELLOW);
}
else{
tft.drawBitmap(130,40,LightOFF,240,160,tft.color565(169,169,169));
  
}
if(prevLightState!=lightState){
  tft.fillRect(160,0,160,240,ILI9341_BLACK);
}
prevLightState=lightState;
}
