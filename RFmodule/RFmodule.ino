
// ------  radio --------
#include <RFM69.h>
#include <SPI.h>
#define NODEID        41    //unique for each node on same network
#define NETWORKID     101  //the same on all nodes that talk to each other
#define GATEWAYID     1
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "xxxxxxxxxxxxxxxx" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack

#define SERIAL_BAUD   9600  //must be 9600 for GPS, use whatever if no GPS
//deviceID's
char buff[20];
byte sendSize=0;
boolean requestACK = false;
RFM69 radio;
// ------  end radio --------



int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue=0;
float sensorVolt=0;

void setup() {
Serial.begin(9600);
Serial.print("let's start");

//RFM69-------------------------------------------
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  Serial.print("init");
//  radio.setHighPower(); //uncomment only for RFM69HW!
//  radio.encrypt(ENCRYPTKEY);
//  char buff[50];
//  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
//  Serial.println(buff);
  Serial.println("finished setup");
 // end of setup
}

void loop() {
  sensorValue = analogRead(sensorPin); 
  Serial.println(sensorValue);
//  sensorVolt=(float)sensorValue*5/1024;
//  Serial.println(sensorVolt);
  delay(1000);                  
}
