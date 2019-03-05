#include <SoftwareSerial.h>

const int xInput = A0;
const int yInput = A1;
const int zInput = A2;
//const int buttonPin = 2;

// Raw Ranges:
// initialize to mid-range and allow calibration to
// find the minimum and maximum for each axis
int xRawMin = 512;
int xRawMax = 512;
int xMean = 0;

int yRawMin = 512;
int yRawMax = 512;
int yMean = 0;

int zRawMin = 512;
int zRawMax = 512;
int zMean = 0;

// Take multiple samples to reduce noise
const int sampleSize = 10;

//variables for serial comunication
String inputString="";
boolean stringComplete = false;


#define rxPin 10
#define txPin 11

// set up a new serial port
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

int j=0;

//void loop() {
//  // listen for new serial coming in:
//  char someChar = mySerial.read();
//  // print out the character:
//  mySerial.print(someChar);
//  // toggle an LED just so you see the thing's alive.  
//  // this LED will go on with every OTHER character received:
//  toggle(13);
//
//}




void setup() 
{
  analogReference(EXTERNAL);
  Serial.begin(9600);
    // define pin modes for tx, rx, led pins:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

void loop() 
{
  int xRaw = ReadAxis(xInput);
  int yRaw = ReadAxis(yInput);
  int zRaw = ReadAxis(zInput);
while (j<5){
    AutoCalibrate(xRaw, yRaw, zRaw);
 
    Serial.print("Raw Ranges: X: ");
    Serial.print(xRawMin);
    Serial.print("-");
    Serial.print(xRawMax);
    
    Serial.print(", Y: ");
    Serial.print(yRawMin);
    Serial.print("-");
    Serial.print(yRawMax);
    
    Serial.print(", Z: ");
    Serial.print(zRawMin);
    Serial.print("-");
    Serial.print(zRawMax);
    Serial.println();
    Serial.print(xRaw);
    Serial.print(", ");
    Serial.print(yRaw);
    Serial.print(", ");
    Serial.print(zRaw);
    
    // Convert raw values to 'milli-Gs"
    long xScaled = map(xRaw, xRawMin, xRawMax, -1000, 1000);
    long yScaled = map(yRaw, yRawMin, yRawMax, -1000, 1000);
    long zScaled = map(zRaw, zRawMin, zRawMax, -1000, 1000);
  
    // re-scale to fractional Gs
    float xAccel = xScaled / 1000.0;
    float yAccel = yScaled / 1000.0;
    float zAccel = zScaled / 1000.0;
  
    Serial.print(" :: ");
    Serial.print(xAccel);
    Serial.print("G, ");
    Serial.print(yAccel);
    Serial.print("G, ");
    Serial.print(zAccel);
    Serial.println("G");
    j++;
    }
    //set the mean after calibration
    if (j==5){
    xMean=xRawMin+(xRawMax - xRawMin)/2;
    yMean=yRawMin+(yRawMax - yRawMin)/2;
    zMean=zRawMin+(zRawMax - zRawMin)/2;
     Serial.println("Moyennes XYZ");
     Serial.println(xMean);
     Serial.println(yMean);
     Serial.println(zMean);
     j++;
    }
    //sends XACC above the mean
    if (xRaw>(xMean+100)||xRaw<(xMean-100) ){
        char buff[6];
        sprintf(buff,"%de",xRaw);
//        char charEnd="e"
//        xRaw=xRaw+charEnd;
        mySerial.write(buff);
    }
    //reads the serial RX
    if (stringComplete) {
      Serial.println(inputString); 
      // clear the string:
      inputString = "";
      stringComplete = false;
    }
    
}

//
// Read "sampleSize" samples and report the average
//
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}

//
// Find the extreme raw readings from each axis
//
void AutoCalibrate(int xRaw, int yRaw, int zRaw)
{
  Serial.println("Calibrate");
  if (xRaw < xRawMin)
  {
    xRawMin = xRaw;
  }
  if (xRaw > xRawMax)
  {
    xRawMax = xRaw;
  }
  
  if (yRaw < yRawMin)
  {
    yRawMin = yRaw;
  }
  if (yRaw > yRawMax)
  {
    yRawMax = yRaw;
  }

  if (zRaw < zRawMin)
  {
    zRawMin = zRaw;
  }
  if (zRaw > zRawMax)
  {
    zRawMax = zRaw;
  }
}

//detects serial event 
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
        // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == 'e') {
      stringComplete = true;
    } 
    else{
    // add it to the inputString:
    inputString += inChar;
    }
  }
}
