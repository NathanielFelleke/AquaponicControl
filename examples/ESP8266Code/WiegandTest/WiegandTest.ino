#include <WiegandNG.h>

#include <Wiegand.h>

WIEGAND wg;

void setup() {
	Serial.begin(115200);  
	Serial.print("alive");
	// default Wiegand Pin 2 and Pin 3 see image on README.md
	// for non UNO board, use wg.begin(pinD0, pinD1) where pinD0 and pinD1 
	// are the pins connected to D0 and D1 of wiegand reader respectively.
	wg.begin();
	}

void loop() {
 

  
	if(wg.available())
	{
Serial.println("Je");
		Serial.print(wg.getCode(),HEX);

	   
	}

}
