
#include <SoftwareSerial.h>

SoftwareSerial gps(3, 2); // RX, TX
String inputString = ""; 
volatile int stringComplete = LOW;

void setup()  
{
  Serial.begin(115200);
  Serial.println("Goodnight moon!");
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);
  gps.begin(9600);
//  attachInterrupt(1, gps_reception, CHANGE);
 
}

void loop() {
  if (gps.available())
    Serial.write(gps.read());

}





