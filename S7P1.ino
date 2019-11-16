#include <EEPROM.h>

#define LowpHPin 2
#define HighpHPin 3
#define TurnOffSensorPin 4
#define pHInputPin A2
float TooLow = 6.7;
float TooHigh = 7.1;
float PHReading;
float PHReading1;
float PHReading2;
float PHReading3;
int AV;

String CalCommand;
unsigned int CalLow = 292;
float CalLowpH = 4;
unsigned int CalMid = 497;
float CalMidpH = 6.8;
unsigned int CalHigh = 672;
float CalHighpH = 9.2;
byte HighByte;
byte LowByte;
byte CalStatus;
unsigned int TempInt;
bool PHEnabled = true;

void setup(){
   pinMode(LowpHPin, OUTPUT);
   digitalWrite(LowpHPin, LOW );
   pinMode(HighpHPin, OUTPUT);
   digitalWrite(HighpHPin, LOW );
   pinMode(TurnOffSensorPin, OUTPUT);
   digitalWrite(TurnOffSensorPin, LOW );
   Serial.begin(9600);
   CalStatus = EEPROM.read(3);
   if (CalStatus > 7) {
    CalStatus = 0;
    EEPROM.write(3, CalStatus);
   }
   Serial.println(CalStatus);      
   if ((CalStatus & 1) == 1){
     HighByte = EEPROM.read(4);
     LowByte =  EEPROM.read(5);
     if (HighByte + LowByte < 510) {
      CalLow = (HighByte << 8) + LowByte;
     }
    HighByte = EEPROM.read(6);
    LowByte =  EEPROM.read(7);
    if (HighByte + LowByte < 510) {
       TempInt = (HighByte << 8) + LowByte;
       CalLowpH = TempInt / 100.0;
    }
   }
   Serial.print("Low pH: ");
   Serial.print(CalLow);
   Serial.print("   ");
   Serial.println(CalLowpH);
     
   if ((CalStatus & 2) == 2){
    HighByte = EEPROM.read(8);
     LowByte =  EEPROM.read(9);
     if (HighByte + LowByte < 510) {
       CalMid = (HighByte << 8) + LowByte;
     }
     HighByte = EEPROM.read(10);
     LowByte =  EEPROM.read(11);
     if (HighByte + LowByte < 510) {
       TempInt = (HighByte << 8) + LowByte;
       CalMidpH = TempInt / 100.0;
     }
   }
   Serial.print("Mid pH: ");
   Serial.print(CalMid);
   Serial.print("   ");
   Serial.println(CalMidpH);
   if ((CalStatus & 4) == 4){
     HighByte = EEPROM.read(12);
     LowByte =  EEPROM.read(13);
     if (HighByte + LowByte < 510) {
       CalHigh = (HighByte << 8) + LowByte;
     }
     Serial.println(CalHigh);
     HighByte = EEPROM.read(14);
     LowByte =  EEPROM.read(15);
     if (HighByte + LowByte < 510) {
       TempInt = (HighByte << 8) + LowByte;
       CalHighpH = TempInt / 100.0;
     }
   }
   Serial.print("High pH: ");
   Serial.print(CalHigh);
   Serial.print("   ");
   Serial.println(CalHighpH);

}//  End setup

void loop() {
  if (PHEnabled) {
      while (Serial.available()) {
        delay(4);  //delay to allow buffer to fill 
        if (Serial.available() >0) {
          char c = Serial.read();  //gets one byte from serial buffer
          CalCommand += c; // Add bytes to string
        } 
      } //End of while loop that reads string
      if (CalCommand.length() >0 ){
        CalCommand.toUpperCase();
        Serial.println(CalCommand);
        if (CalCommand.startsWith("CL")) {
          CalCommand = CalCommand.substring(2);
          CalLowpH = CalCommand.toFloat();
          CalLow = GetMiddleAnalog();
          CalStatus = CalStatus | 1;
          EEPROM.write(3, CalStatus);
          EEPROM.write(4, highByte(CalLow));
          EEPROM.write(5, lowByte(CalLow));
          TempInt = CalLowpH * 100;
          EEPROM.write(6, highByte(TempInt));
          EEPROM.write(7, lowByte(TempInt));
        }
    
        if (CalCommand.startsWith("CM")) {
          CalCommand = CalCommand.substring(2);
          CalMidpH = CalCommand.toFloat();
          CalMid = GetMiddleAnalog();
          CalStatus = CalStatus | 2;
          EEPROM.write(3, CalStatus);
          EEPROM.write(8, highByte(CalMid));
          EEPROM.write(9, lowByte(CalMid));
          TempInt = CalMidpH * 100;
          EEPROM.write(10, highByte(TempInt));
          EEPROM.write(11, lowByte(TempInt));
        }

        if (CalCommand.startsWith("CU")) {
          CalCommand = CalCommand.substring(2);
          CalHighpH = CalCommand.toFloat();
          CalHigh = GetMiddleAnalog();
          CalStatus = CalStatus | 4;
          EEPROM.write(3, CalStatus);
          EEPROM.write(12, highByte(CalHigh));
          EEPROM.write(13, lowByte(CalHigh));
          TempInt = CalHighpH * 100.0;
          EEPROM.write(14, highByte(TempInt));
          EEPROM.write(15, lowByte(TempInt));
        }
        CalCommand = "";
      } // End of if (CalCommand.length() >0 )
    digitalWrite(TurnOffSensorPin,LOW);
    AV = GetMiddleAnalog();
    if (AV > CalMid) {
      PHReading = CalMidpH + (CalHighpH - CalMidpH)/(CalHigh - CalMid) *(AV - CalMid);
    }
    if (AV <= CalMid) {
      PHReading = CalMidpH - (CalMidpH - CalLowpH)/(CalMid - CalLow ) *(CalMid- AV);
    }
    Serial.print("pH = ");
    Serial.println(PHReading);
      if (PHReading > 2 && PHReading < 12){
      PHReading3 = PHReading2;
      PHReading2 = PHReading1;  
      PHReading1= PHReading;
      if (PHReading1 > TooHigh && PHReading2 > TooHigh && PHReading3 > TooHigh) {
        digitalWrite(HighpHPin,HIGH);
      }
      else {
        digitalWrite(HighpHPin,LOW);
      }
      if (PHReading1 < TooLow && PHReading2 < TooLow && PHReading3 < TooLow) {
        digitalWrite(LowpHPin,HIGH);
      }
      else {
        digitalWrite(LowpHPin,LOW);
      }
      } // End of if PHReading > 2 && PHReading < 12
    Serial.print("pH = ");
    Serial.print(PHReading1,4);
    Serial.print("   ");
    Serial.print(PHReading2,4);
    Serial.print("   ");
    Serial.println(PHReading3,4);
    delay(1000);
  }
  else{
    digitalWrite(TurnOffSensorPin, HIGH);
  }
}  //  End main loop

int GetMiddleAnalog(){
  int i, j;
  int n = 5;
  int temp;
  int arr[5];
  for (i = 0; i < n; i++){
    arr[i] = analogRead(pHInputPin);
    // Important: If using circuit board from Figure 7.2, you must change above line to 
    //  arr[i] = 1023 - analogRead(pHInputPin);

    delay(20);
  }
  for (i = 0; i < n; i++){
    for (j = 0; j < n-1; j++) {
      if (arr[j] > arr[j+1]) {
        int temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }
  return arr[2];
}// End GetMiddle
 
