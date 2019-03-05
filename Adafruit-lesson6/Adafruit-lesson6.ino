/*
Adafruit Arduino - Lesson 6. Inputs
*/

int ledPin = 5;
int buttonApin = 9;
int buttonBpin = 8;
int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
byte leds = 0;

void setup() 
{
  pinMode(ledPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);  
  Serial.begin(9600);
  Serial.println("Serial setup lesson 6");
}

void loop() 
{
  Serial.println("bouton pin9");
  Serial.println(digitalRead(buttonApin));
  photocellReading = analogRead(photocellPin);
  Serial.print("Analog reading = ");
  Serial.println(photocellReading); 
  
  if (photocellReading<500)
  {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON");
  }
  if (digitalRead(buttonBpin) == LOW)
  {
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");
  }
}
