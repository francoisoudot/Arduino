

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue=0;
float sensorVolt=0;

void setup() {
Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(sensorPin); 
  Serial.println(sensorValue);
//  sensorVolt=(float)sensorValue*5/1024;
//  Serial.println(sensorVolt);
  delay(1000);                  
}
