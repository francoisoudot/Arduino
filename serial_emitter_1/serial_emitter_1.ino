
#include <SoftwareSerial.h>

SoftwareSerial serialsoft(3, 2); // RX, TX
String inputString = ""; 
volatile int stringComplete = LOW;

void setup()  
{
  Serial.begin(9600);
//  Serial.println("Goodnight moon!");
  pinMode(3, INPUT);
  pinMode(2, OUTPUT);
  serialsoft.begin(9600);
//  attachInterrupt(1, gps_reception, CHANGE);
 
}

void loop() {
  delay(1000);
  serialsoft.write("hello");


  
//  if (serialsoft.available())
//    Serial.write(serialsoft.read());

}





