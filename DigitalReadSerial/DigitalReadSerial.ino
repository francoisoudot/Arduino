/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor 
 
 This example code is in the public domain.
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pin1 = 8;
int pin2=7;
int sensorPin=A0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pin1, INPUT);
//  pinMode(pin2, INPUT_PULLUP);

}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int pin1_state = digitalRead(pin1);
//  int pin2_state = digitalRead(pin2);
  // print out the state of the button:
  Serial.println(pin1_state);
//  Serial.println(pin2_state);
//  int sensorValue = analogRead(sensorPin); 
//  Serial.println(sensorValue);
  delay(50);
}



