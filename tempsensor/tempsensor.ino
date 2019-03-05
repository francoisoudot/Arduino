#include <SPI.h>

#include <RFM69.h>
#include <RFM69registers.h>


/* sensor
node = 12
device ID
2 = 1222 = smoke or not
3 = 1232 = flame detected or not
4 = 1242 = human motion present or not
5 = 1252 = barking or not
6 = 1262, 1263 = temperature, humidity

*/




//RFM69  --------------------------------------------------------------------------------------------------
#include <RFM69.h>
#include <SPI.h>
#define NODEID        10    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MHZ
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "my-EncryptionKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack
#define LED           9  // Moteinos have LEDs on D9
#define SERIAL_BAUD   9600  //must be 9600 for GPS, use whatever if no GPS

boolean debug = 1;

//struct for wireless data transmission
typedef struct {		
  int       nodeID; 		//node ID (1xx, 2xx, 3xx);  1xx = basement, 2xx = main floor, 3xx = outside
  int       deviceID;		//sensor ID (2, 3, 4, 5)
  unsigned long   var1_usl; 		//uptime in ms
  float     var2_float;   	//sensor data?
  float     var3_float;		//battery condition?
} Payload;
Payload theData;

char buff[20];
byte sendSize=0;
boolean requestACK = false;
RFM69 radio;

//end RFM69 ------------------------------------------


// Light sensor ===============================================
int lightAnalogInput = A0;    //analog input for photo resistor
int lightValue = -50;
int lightValue_previous = -50;

unsigned long light_time;
unsigned long light_time_send;


void setup()
{
  Serial.begin(9600);          //  setup serial
Serial.println("start");
  Serial.print((FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915));

  //RFM69-------------------------------------------
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  Serial.println("init");
  #ifdef IS_RFM69HW
    radio.setHighPower(); //uncomment only for RFM69HW!
    Serial.println("radio set");
  #endif
  radio.encrypt(ENCRYPTKEY);
  Serial.println("buff");

  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  theData.nodeID = NODEID;  //this node id should be the same for all devices in this node
  //end RFM--------------------------------------------
  
  
//  //temperature / humidity sensor
//  dht.begin();
//  
//  //sound/noise
//  pinMode(soundInput, INPUT);


  //initialize times
  light_time = millis();
//  flame_time = millis();
//  pir_time = millis();
//  sound_time = millis();
//  temperature_time = millis();
// 
//  //PIR sensor
//  pinMode(PirInput, INPUT);
}

void loop()
{
  
  unsigned long time_passed = 0;
  
  //===================================================================
  //===================================================================
  //device #7
  //light

  time_passed = millis() - light_time;

  if (time_passed < 0)
  {
    light_time = millis();
    light_time_send = -70000;
  }
  if (time_passed > 2000)  //how often to examine the sensor analog value
  {
    light_time = millis();		//update time when sensor value last read
    lightValue = 0;

    //analog value:  Less than 100 is dark.  greater than 500 is room lighting
    lightValue = analogRead(lightAnalogInput);
    if ((lightValue < (lightValue_previous - 50)) || ((lightValue > (lightValue_previous + 100)) || (705000 < (millis() - light_time_send))) )
    {
      light_time_send = millis();  //update gas_time_send with when sensor value last transmitted
      theData.deviceID = 7;
      theData.var1_usl = millis();
      theData.var2_float = lightValue;
      theData.var3_float = lightValue + 20;
      radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData));
      lightValue_previous = lightValue;
      Serial.print("light RFM =");
      Serial.println(lightValue);
      Serial.println("ack");
    }	
	
    //start debug code
    if (debug)
    {
      Serial.print("light analog = ");
      Serial.println(lightValue);
    }
    //analog value:  usually 1023 for no fire, lower for fire.
 
  }// end if millis time_passed >
  
}//end loop









