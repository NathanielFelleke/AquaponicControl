#include <WiegandNG.h>

WiegandNG wg;
#include <WiFi.h>
const char* ssid     = "Cool";
const char* password = "esp8266rfid";
WiFiServer server(80);
String header;

void PrintBinary(WiegandNG &tempwg) {
	volatile unsigned char *buffer=tempwg.getRawData();
	unsigned int bufferSize = tempwg.getBufferSize();
	unsigned int countedBits = tempwg.getBitCounted();

	unsigned int countedBytes = (countedBits/8);
	if ((countedBits % 8)>0) countedBytes++;
	// unsigned int bitsUsed = countedBytes * 8;
	
	for (unsigned int i=bufferSize-countedBytes; i< bufferSize;i++) {
		unsigned char bufByte=buffer[i];
		for(int x=0; x<8;x++) {
			if ( (((bufferSize-i) *8)-x) <= countedBits) {
				if((bufByte & 0x80)) {
					Serial.print("1");
				}
				else {
					Serial.print("0");
				}
			}
			bufByte<<=1;
		}
	}
	Serial.println();
}

void setup() {
	Serial.begin(115200);

	// for UNO just use wg.begin(), will default to Pin 2 and Pin 3 connected to D0 and D1 respectively
	// initialize Wiegand ND for 48 bit data, every 8 bits take up 1 byte of Arduino memory
	// as long as there is still memory, user can even capture 1024 bit Wiegand by calling wg.begin(1024)

	unsigned int pinD0 = 2;
	unsigned int pinD1 = 4;
	unsigned int wiegandbits = 48;
	unsigned int packetGap = 15;			// 25 ms between packet
	
	if(!wg.begin(pinD0, pinD1, wiegandbits, packetGap)) {
		Serial.println("Out of memory!");
	}
	Serial.println("Ready...");
 Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin(); 
}

void loop() {
	if(wg.available()) {
		wg.pause();							// pause Wiegand pin interrupts
		Serial.print("Bits=");
		Serial.println(wg.getBitCounted());	// display the number of bits counted
		Serial.print("RAW Binary=");
		PrintBinary(wg);					// display raw data in binary form, raw data inclusive of PARITY
		wg.clear();							// compulsory to call clear() to enable interrupts for subsequent data
	}
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
