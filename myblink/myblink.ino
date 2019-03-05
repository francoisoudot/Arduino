/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo
  Serial.println("Serial setup");
}

// the loop function runs over and over again forever
void loop() {
  if (Serial.available())
  {
    String s = Serial.readString();
    Serial.println(s);
    digitalWrite(13, HIGH);
    Serial.println("LED on");  // turn the LED on (HIGH is the voltage level)
    delay(1000);              // wait for a second
    digitalWrite(13, LOW); 
    Serial.println("LED off");  // turn the LED off by making the voltage LOW
    delay(1000);       // wait for a second
  }
    
}
