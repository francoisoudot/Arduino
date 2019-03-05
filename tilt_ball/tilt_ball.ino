/* Better Debouncer
 * 
 * This debouncing circuit is more rugged, and will work with tilt switches!
 *
 * http://www.ladyada.net/learn/sensor/tilt.html
 */
 
int inPin = 2;         // the number of the input pin
int outPin = 9;       // the number of the output pin
 
int LEDstate = HIGH;      // the current state of the output pin
int tilt_reading;           // the current reading from the input pin
int tilt_previous = LOW;    // the previous reading from the input pin
int count=0; //counts the number of change to adapt accuracy of the event
 
// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 50;   // the debounce time, increase if the output flickers
long debounce_up = 600;   // the debounce time, increase if the output flickers

// activation/deactivation=!activation/force on
int activation=HIGH;
int force_on=LOW;


//ethernet
#include <Ethernet.h>
#include <SPI.h>
boolean reading = false;

  // if need to change the MAC address (Very Rare)
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

  EthernetServer server = EthernetServer(80); //port 80

void setup()
{
  pinMode(inPin, INPUT_PULLUP);
//  digitalWrite(inPin, HIGH);   // turn on the built in pull-up resistor
  pinMode(outPin, OUTPUT);
  Serial.begin(9600);
  
  Ethernet.begin(mac);
  //Ethernet.begin(mac, ip, gateway, subnet); //for manual setup

  server.begin();
  Serial.println(Ethernet.localIP());

}
 
void loop()
{
  int switchstate=LOW;
 
  tilt_reading = digitalRead(inPin);
 
  // If the switch changed, due to bounce or pressing...
  if (tilt_reading xor tilt_previous) {
    count++;
    
    if (count>20){
      count=0;
      switchstate=HIGH;
      Serial.println("LED state");
      Serial.println(LEDstate);
      LEDstate=!LEDstate;
      Serial.println(LEDstate);
      digitalWrite(outPin, LEDstate);
      delay(2000);  
    }

  }
 
  // Save the last reading so we keep a running tally
  if (switchstate==LOW){
    tilt_previous=tilt_reading;
  }
  else{
    tilt_previous=digitalRead(inPin);
  }
  
  checkForClient();

}


void checkForClient(){

  EthernetClient client = server.available();

  if (client) {

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean sentHeader = false;

    while (client.connected()) {
      if (client.available()) {
                        if(!sentHeader){
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();

          sentHeader = true;
        }


        char c = client.read();
        

        if(reading && c == ' ') reading = false;
        if(c == '?') {
          reading = true; //found the ?, begin reading the info
          Serial.println("characther in ethernet");
        Serial.println(c);
        }

        if(reading){
                    Serial.println("characther in ethernet2");

          Serial.println(c);

           switch (c) {
            case '2':
            //deactivate light
              activation=LOW;
              HTMLcontent(client);
              break;
            case '3':
            //activate light
              activation=HIGH;
              HTMLcontent(client);
              break;
            case '4':
            //switch light off
              triggerLED(LOW);
              HTMLcontent(client);
              break;
            case '5':
            //switch light on
              triggerLED(HIGH);
              HTMLcontent(client);
              break;
          }

        }

        if (c == '\n' && currentLineIsBlank)  break;

        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }
        



      }
    }

    delay(1); // give the web browser time to receive the data
    client.stop(); // close the connection:

  } 

}


void triggerLED(int state){
  LEDstate=state;
  digitalWrite(outPin, LEDstate);
}


void HTMLcontent(EthernetClient client){
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("Activation ");
          client.print(activation);
          client.println("LED state ");
          client.print(LEDstate);
          client.println("</html>");
}

