// ADD control that cycles through the different ones
// MODIFY code to netter transition sunrise - TO FINALIZE
// ADD ANIMATION LOOPS
// GET RID OF THE BLE FOR THE EXPO





/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <FastLED.h>
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

//#include <Adafruit_NeoPixel.h>

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    PIN                       Which pin on the Arduino is connected to the NeoPixels?
    NUMPIXELS                 How many NeoPixels are attached to the Arduino?
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE     1

    #define PIN                     11
    #define NUM_LEDS                150
/*=========================================================================*/

//Initialise the LED array, the LED Hue (ledh) array, and the LED Brightness (ledb) array.
CRGB leds[NUM_LEDS];


//Global Variables ---------------------------------------------------------------------------------
int top_start=0;              //position of first LED of the top loop
int top_end=NUM_LEDS/2;       //position of last LED of the top loop
int bottom_start=NUM_LEDS/2;  //position of first LED of the bottom loop
int bottom_end=NUM_LEDS;      //position of last LED of the bottom loop 
int time_anim_s=2*60;         //time of animation in seconds 5min * 60s

//initial color - decided to take a nice red
uint8_t red = 235;
uint8_t green = 44;
uint8_t blue = 69;
int animationState;


//Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXELS, PIN); // NeoPixel Object for Visor Strips

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];


/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  //while (!Serial);  // required for Flora & Micro
  delay(500);

    //initialise the LED strip 
    FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS); 
    fill_solid(leds,NUM_LEDS,CRGB(red,green,blue)); 
    FastLED.show();
    

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Neopixel Color Picker Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("***********************"));

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("***********************"));
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void) {
  //setup LEDs
  fill_solid(leds,NUM_LEDS,CRGB(red,green,blue)); 
  FastLED.show(); 
  
  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  /* Got a packet! */
  // printHex(packetbuffer, len);

  // Color
  if (packetbuffer[1] == 'C') {
    red = packetbuffer[2];
    green = packetbuffer[3];
    blue = packetbuffer[4];
    Serial.print ("RGB #");
    if (red < 0x10) Serial.print("0");
    Serial.print(red, HEX);
    if (green < 0x10) Serial.print("0");
    Serial.print(green, HEX);
    if (blue < 0x10) Serial.print("0");
    Serial.println(blue, HEX);

// fill the LED with the color picked
    fill_solid(leds,NUM_LEDS,CRGB(red,green,blue)); 
    FastLED.show(); // This sends the updated pixel color to the hardware.
  }

  // Buttons
  if (packetbuffer[1] == 'B') {
 
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(buttnum);
    animationState = buttnum;
    if (pressed) {
      Serial.println(" pressed");
    } else {
      Serial.println(" released");
    }
    
  if (animationState == 1){
    sunrise();
  }
  
  if (animationState == 2){
    thunder();
  }

  if (animationState == 3){
    sunset();

  }
  
  if (animationState == 4){

    //do a loop over the different animations

  }
 }
}



// ################# CLOUD EFECTS ###################
// ################# CLOUD EFECTS ###################
// ################# CLOUD EFECTS ###################
// ################# CLOUD EFECTS ###################
// ################# CLOUD EFECTS ###################
// ################# CLOUD EFECTS ###################










//===================================================================================================================================================
// sunrise() : Sunrise animation
// HSV values in comment are given in scale of 360,100,100
// bottom = NUM_LEDS/2 - NUM_LEDS
// top = 0 - NUM_LEDS/2
// this animation has 6 loops - the timing has been designed for 10 loops so you can give a higher wait to some loop by going in the loop and change the weight_loop variable
//===================================================================================================================================================
void sunrise(){

//Loop variables
  int weight_loop=1; //to be changed in each loop
  int number_loop=70;
  
  //initial HSV for top
  int init_H_top;
  int init_S_top;
  int init_V_top;

  //final HSV for top
  int final_H_top;
  int final_S_top;
  int final_V_top;
  
  //initial HSV for bottom
  int init_H_bot;
  int init_S_bot;
  int init_V_bot;

  //final HSV for bottom
  int final_H_bot;
  int final_S_bot;
  int final_V_bot;
  
//-------------------------------------------------------------------------------------
//loop0 in 255b HSV  start(top: HSV=178,216,73 / bottom: HSV=178,216,73) -- end(top: HSV=178,216,73 / bottom: HSV=236,147,81)

  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=2;
   
  //initial HSV for top
   init_H_top=178;
   init_S_top=216;
   init_V_top=73;

  //final HSV for top
   final_H_top=178;
   final_S_top=216;
   final_V_top=73;

  //initial HSV for bottom
   init_H_bot=178;
   init_S_bot=216;
   init_V_bot=73;

  //final HSV for bottom
   final_H_bot=236;
   final_S_bot=147;
   final_V_bot=81;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);
     
//-------------------------------------------------------------------------------------
//loop1 in 255b HSV  start(top: HSV=178,216,73 / bottom: HSV=236,147,81) -- end(top: HSV=236,147,81 / bottom: HSV=0,165,114)
  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=2;

  //initial HSV for top
   init_H_top=178;
   init_S_top=216;
   init_V_top=73;

  //final HSV for top
   final_H_top=236;
   final_S_top=147;
   final_V_top=81;

  //initial HSV for bottom
   init_H_bot=236;
   init_S_bot=147;
   init_V_bot=81;

  //final HSV for bottom
   final_H_bot=255;
   final_S_bot=165;
   final_V_bot=114;
   
   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);


//-------------------------------------------------------------------------------------
//loop2   start(top: HSV=334,58,32 / bottom: HSV=0,65,45) -- end(top: HSV=0,65,45  / bottom: HSV=15,71,65)
//loop2   start(top: HSV=236,147,82 / bottom: HSV=0,166,115) -- end(top: HSV=0,166,115  / bottom: HSV=11,181,166)
  
  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=2;

  //initial HSV for top
   init_H_top=236;
   init_S_top=147;
   init_V_top=82;

  //final HSV for top
   final_H_top=255;
   final_S_top=166;
   final_V_top=115;

  //initial HSV for bottom
   init_H_bot=0;
   init_S_bot=166;
   init_V_bot=115;

  //final HSV for bottom
   final_H_bot=11;
   final_S_bot=181;
   final_V_bot=166;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);

//-------------------------------------------------------------------------------------
//loop3   start(top: HSV=0,65,45 / bottom: HSV=15,71,65) -- end(top: HSV=15,71,65  / bottom: HSV=28,85,77)
//loop3   start(top: HSV=0,166,115 / bottom: HSV=11,181,166) -- end(top: HSV=11,181,166  / bottom: HSV=20,217,196)

  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=2;

  //initial HSV for top
   init_H_top=0;
   init_S_top=166;
   init_V_top=115;

  //final HSV for top
   final_H_top=11;
   final_S_top=181;
   final_V_top=166;

  //initial HSV for bottom
   init_H_bot=11;
   init_S_bot=181;
   init_V_bot=166;

  //final HSV for bottom
   final_H_bot=20;
   final_S_bot=217;
   final_V_bot=196;
  
   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);


//-------------------------------------------------------------------------------------
//loop4   start(top: HSV=15,71,65 / bottom: HSV=28,85,77) -- end(top: HSV=28,85,77  / bottom: HSV=37,99,89)
//loop4   start(top: HSV=11,181,166 / bottom: HSV=20,217,196) -- end(top: HSV=20,217,196  / bottom: HSV=26,248,227)
  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=1;

  //initial HSV for top
   init_H_top=11;
   init_S_top=181;
   init_V_top=166;

  //final HSV for top
   final_H_top=20;
   final_S_top=217;
   final_V_top=196;

  //initial HSV for bottom
   init_H_bot=20;
   init_S_bot=217;
   init_V_bot=196;

  //final HSV for bottom
   final_H_bot=26;
   final_S_bot=248;
   final_V_bot=227;
  
   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);

//-------------------------------------------------------------------------------------
//loop5   start(top: HSV=28,85,77  / bottom: HSV=37,99,89) -- end(top: HSV=37,99,89  / bottom: HSV=42,70,100)
//loop5   start(top: HSV=20,217,196  / bottom: HSV=26,248,227) -- end(top: HSV=26,248,227  / bottom: HSV=30,150,255)
  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=1;
   number_loop=number_loop*weight_loop;

  //initial HSV for top
   init_H_top=20;
   init_S_top=217;
   init_V_top=196;

  //final HSV for top
   final_H_top=26;
   final_S_top=248;
   final_V_top=227;

  //initial HSV for bottom
   init_H_bot=26;
   init_S_bot=248;
   init_V_bot=227;

  //final HSV for bottom
   final_H_bot=30;
   final_S_bot=150;
   final_V_bot=255;
  
   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);

//-------------------------------------------------------------------------------------
//loop6   start(top: HSV=37,99,89  / bottom: HSV=42,70,100) -- end(top: HSV=42,70,100  / bottom: HSV=42,70,100)
//loop6   start(top: HSV=26,248,227  / bottom: HSV=30,150,255) -- end(top: HSV=30,150,255  / bottom: HSV=30,150,255)
  
  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=1;
   number_loop=number_loop*weight_loop;

  //initial HSV for top
   init_H_top=26;
   init_S_top=248;
   init_V_top=227;

  //final HSV for top
   final_H_top=30;
   final_S_top=150;
   final_V_top=255;

  //initial HSV for bottom
   init_H_bot=30;
   init_S_bot=150;
   init_V_bot=255;

  //final HSV for bottom
   final_H_bot=30;
   final_S_bot=150;
   final_V_bot=255;
  
   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);
  

//end animation sunrise - delay of some seconds and fade back to black
   delay(2000);
   changeLEDs(CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),
     CHSV(final_H_top,0,0),CHSV(final_H_bot,0,0),number_loop*weight_loop);
  delay(2000);
}



//===================================================================================================================================================
// sunset() : Sunset animation
//===================================================================================================================================================
void sunset(){
  
//Loop variables
  int weight_loop=1; //to be changed in each loop
  int number_loop=70;
  
  //initial HSV for top
  int init_H_top;
  int init_S_top;
  int init_V_top;

  //final HSV for top
  int final_H_top;
  int final_S_top;
  int final_V_top;
  
  //initial HSV for bottom
  int init_H_bot;
  int init_S_bot;
  int init_V_bot;

  //final HSV for bottom
  int final_H_bot;
  int final_S_bot;
  int final_V_bot;
  
//-------------------------------------------------------------------------------------
//loop1   start(top: HSV=30,150,255  / bottom: HSV=30,150,255) -- end (top: HSV=30,150,255  / bottom: HSV=26,248,227)
  
  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=1;

  //initial HSV for top
   init_H_top=30;
   init_S_top=150;
   init_V_top=255;
   

  //final HSV for top
   final_H_top=30;
   final_S_top=150;
   final_V_top=255;

  //initial HSV for bottom
   init_H_bot=30;
   init_S_bot=150;
   init_V_bot=255;

  //final HSV for bottom
   final_H_bot=26;
   final_S_bot=248;
   final_V_bot=227;
   
   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);

//-------------------------------------------------------------------------------------
//loop2   start(top: HSV=30,150,255  / bottom: HSV=26,248,227) -- end (top: HSV=26,248,227  / bottom: HSV=20,217,196)

  
  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=1;
   
  //initial HSV for top
   init_H_top=30;
   init_S_top=150;
   init_V_top=255;

  //final HSV for top
   final_H_top=26;
   final_S_top=248;
   final_V_top=227;

  //initial HSV for bottom
   init_H_bot=26;
   init_S_bot=248;
   init_V_bot=227;

  //final HSV for bottom
   final_H_bot=20;
   final_S_bot=217;
   final_V_bot=196;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);

//-------------------------------------------------------------------------------------
//loop3   start(top: HSV=26,248,227 / bottom: HSV=20,217,196) -- end(top: HSV=20,217,196  / bottom: HSV=11,181,166)

  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=1;
   
  //initial HSV for top
   init_H_top=26;
   init_S_top=248;
   init_V_top=227;

  //final HSV for top
   final_H_top=20;
   final_S_top=217;
   final_V_top=196;

  //initial HSV for bottom
   init_H_bot=20;
   init_S_bot=217;
   init_V_bot=196;

  //final HSV for bottom
   final_H_bot=11;
   final_S_bot=181;
   final_V_bot=166;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);
//-------------------------------------------------------------------------------------
//loop4   start(top: HSV=20,217,196 / bottom: HSV=11,181,166) -- end(top: HSV=11,181,166  / bottom: HSV=0,166,115)

  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=1;
   
  //initial HSV for top
   init_H_top=20;
   init_S_top=217;
   init_V_top=196;

  //final HSV for top
   final_H_top=11;
   final_S_top=181;
   final_V_top=166;

  //initial HSV for bottom
   init_H_bot=11;
   init_S_bot=181;
   init_V_bot=166;

  //final HSV for bottom
   final_H_bot=0;
   final_S_bot=166;
   final_V_bot=115;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);


//-------------------------------------------------------------------------------------
//loop5   start(top: HSV=11,181,166 / bottom: HSV=0,166,115) -- end(top: HSV=0,166,115  / bottom: HSV=236,147,82)

  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=2;
   
  //initial HSV for top
   init_H_top=11;
   init_S_top=181;
   init_V_top=166;

  //final HSV for top
   final_H_top=0;
   final_S_top=166;
   final_V_top=115;

  //initial HSV for bottom
   init_H_bot=255;
   init_S_bot=166;
   init_V_bot=115;

  //final HSV for bottom
   final_H_bot=236;
   final_S_bot=147;
   final_V_bot=82;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);

//-------------------------------------------------------------------------------------
//loop6 in 255b HSV  start(top: HSV=0,165,114 / bottom: HSV=236,147,81) -- end(top: HSV=236,147,81 / bottom: HSV=178,216,73)

  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=2;
   
  //initial HSV for top
   init_H_top=255;
   init_S_top=166;
   init_V_top=115;

  //final HSV for top
   final_H_top=236;
   final_S_top=147;
   final_V_top=81;

  //initial HSV for bottom
   init_H_bot=236;
   init_S_bot=147;
   init_V_bot=81;

  //final HSV for bottom
   final_H_bot=178;
   final_S_bot=216;
   final_V_bot=73;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);

//-------------------------------------------------------------------------------------
//loop7 in 255b HSV  start(top: HSV=236,147,81 / bottom: HSV=178,216,73) -- end(top: HSV=178,216,73 / bottom: HSV=178,216,73)

  // weight of the loop - changes the duration of this loop - cf comment on top of the animation
   weight_loop=2;
   
  //initial HSV for top
   init_H_top=236;
   init_S_top=147;
   init_V_top=81;

  //final HSV for top
   final_H_top=178;
   final_S_top=216;
   final_V_top=73;

  //initial HSV for bottom
   init_H_bot=178;
   init_S_bot=216;
   init_V_bot=73;

  //final HSV for bottom
   final_H_bot=178;
   final_S_bot=216;
   final_V_bot=73;

   //show LED 
   changeLEDs(CHSV(init_H_top,init_S_top,init_V_top),CHSV(init_H_bot,init_S_bot,init_V_bot),
     CHSV(final_H_top,final_S_top,final_V_top),CHSV(final_H_bot,final_S_bot,final_V_bot),number_loop*weight_loop);


  //end animation sunset - delay of some seconds and fade back to black
  delay(5000);
  for(int i=0; i<255;i++){
    fadeLEDs(1);
    delay(30);  
  }
  
}

//===================================================================================================================================================
// thunder() : Thunder animation
// The whole LED stripe is designed into a matrix and saves as the picture of what is displayed. This way we know in real time where we are on the animation
//===================================================================================================================================================
void thunder(){

//loop variables

//base color for the tunder
  //dark blue
  int H_DB=168;
  int S_DB=171;
  int V_DB=30;
  //setup the color - should use changeLEDS after
  setLED(0,NUM_LEDS,H_DB,S_DB,V_DB);
  FastLED.show();
  delay(2000);
  CHSV hsv_DB(H_DB,S_DB,V_DB);
  CRGB rgb_DB;
  hsv2rgb_rainbow(hsv_DB,rgb_DB);

  
  //mid blue
  int H_MB=168;
  int S_MB=137;
  int V_MB=40;
  CHSV hsv_MB(H_MB,S_MB,V_MB);
  //white blue
  int H_WB=168;
  int S_WB=112;
  int V_WB=140;
  CHSV hsv_WB(H_WB,S_WB,V_WB);
  //white
  int H_W=168;
  int S_W=30;
  int V_W=255;
  CHSV hsv_W(H_W,S_W,V_W);

//duration of the animation 
  unsigned long howLong=20000; //this value must be in milli seconds
  unsigned long finishAt = millis() + howLong;

// threshold of the animation
  int intensity_level=3; //the lower the more intense -- intensity is a value between 0-10
  int spread_level=14; //the lower the more spreaded -- spread is a value between 0-20


//-----------------------------------------    
//beginning of the loop
  while(millis() < finishAt)
  {
  //-----------------------------------------    

    int frequency=intensity_level*random(1,5); //frequency of the thunder appearence
    int fade_speed=5; //speed of the fading - can be configured here or setup randomly in the future - the lower the faster

    
  //-----------------------------------------    
  //loop to fade the LEDS after the thunder
    EVERY_N_MILLISECONDS(30) {
      Serial.println("30ms loop");
      for (int x=0; x<NUM_LEDS;x++){
//        checks if LED is already in the DB color
        if(rgb_DB.r-3<leds[x].r<rgb_DB.r+3 && rgb_DB.g-3<leds[x].g<rgb_DB.g+3 && rgb_DB.b-3<leds[x].b<rgb_DB.b+3){
          leds[x]=hsv_DB;
          }
        else{
          CRGB currentColor(leds[x].r,leds[x].g,leds[x].b);
          CHSV hsv_led=rgb2hsv_approximate(currentColor);
          int H=hsv_led.h+((hsv_DB.h-hsv_led.h)/(fade_speed));
          int S=hsv_led.s+((hsv_DB.s-hsv_led.s)/(fade_speed));
          int V=hsv_led.v+((hsv_DB.v-hsv_led.v)/(fade_speed));
          leds[x] =CHSV(H,S,V);


          }
      }
    }
    
  //-----------------------------------------    
  //loop to create the thunder
    EVERY_N_MILLISECONDS(100) {
            Serial.println("100ms loop");
      // init loop values
      int location=random(0,NUM_LEDS); //location of the thunder
      int intensity=random(0,10); //intensity of the thunder
      int spread=random(0,10)+intensity; //spreading of the thunder
      int W_LEDS=0; //# of white LEDs
      int WB_LEDS=0; //# of whiteblue LEDS 
      int MB_LEDS=0; //# of mid blue LEDS
      bool top_bottom=0; //boolean if top and bottom is involved
      int total_LEDS=spread;//total number of LEDS in the spread
      int pos_start;
      int pos_end;
      int spread_start;
      int spread_end;
            Serial.println(intensity);

      //calculation of the color of LEDS
      //white color
      if(intensity>(intensity_level+intensity_level/2)){ 
          Serial.println("if1");
        W_LEDS=intensity-(intensity_level+intensity_level/2);
        //set LEDS for the white color
        pos_start=constrain(location-(W_LEDS/2),0,NUM_LEDS);
        pos_end=constrain(location+(W_LEDS/2),0,NUM_LEDS);
        setLED(pos_start,pos_end,H_W,S_W,V_W);
        //fill in gradient
        spread=spread+W_LEDS;
        spread_start=constrain(pos_start-spread/2,0,NUM_LEDS);
        spread_end=constrain(pos_start+spread/2,0,NUM_LEDS);
        fill_gradient(leds,spread_start,hsv_DB,pos_start,hsv_W,SHORTEST_HUES);
        fill_gradient(leds,pos_end,hsv_W,spread_end,hsv_DB,SHORTEST_HUES);
      }
      
      //white blue color
      if(intensity>intensity_level) { 
        WB_LEDS=intensity-intensity_level;
        //set LEDS for the white blue color
        pos_start=constrain(location-(W_LEDS/2),0,NUM_LEDS);
        pos_end=constrain(location+(W_LEDS/2),0,NUM_LEDS);
        setLED(pos_start,pos_end,H_WB,S_WB,V_WB);
        //fill in gradient
        spread=spread+WB_LEDS;
        spread_start=constrain(pos_start-spread/2,0,NUM_LEDS);
        spread_end=constrain(pos_start+spread/2,0,NUM_LEDS);
        fill_gradient(leds,spread_start,hsv_DB,pos_start,hsv_WB,SHORTEST_HUES);
        fill_gradient(leds,pos_end,hsv_WB,spread_end,hsv_DB,SHORTEST_HUES);      
        }
      //medium blue
      else
      {
        MB_LEDS=intensity;
        //set LEDS for the white blue color
        pos_start=constrain(location-(W_LEDS/2),0,NUM_LEDS);
        pos_end=constrain(location+(W_LEDS/2),0,NUM_LEDS);
        setLED(pos_start,pos_end,H_MB,S_MB,V_MB);
        //fill in gradient
        spread=spread+MB_LEDS;
        spread_start=constrain(pos_start-spread/2,0,NUM_LEDS);
        spread_end=constrain(pos_start+spread/2,0,NUM_LEDS);
        fill_gradient(leds,spread_start,hsv_DB,pos_start,hsv_MB,SHORTEST_HUES);
        fill_gradient(leds,pos_end,hsv_MB,spread_end,hsv_DB,SHORTEST_HUES);  
        }      
    }

      FastLED.show();    



  }

//  end animation thunder - fade back to black
  for(int i=0; i<255;i++){
    fadeLEDs(1);
    delay(30);  
  }

}







//===================================================================================================================================================
// fadeLEDs(): This function is used to fade all LEDs back to black (OFF) following a fade value
//===================================================================================================================================================
void fadeLEDs(int fadeVal){
    fadeToBlackBy(leds, NUM_LEDS, fadeVal);
    FastLED.show();
}



//===================================================================================================================================================
// setLED() : is used to illuminate the LEDs 
// + does the for loop to get the LED light up
//===================================================================================================================================================
void setLED(int pos_start, int pos_end, int LEDhue, int LEDsat, int LEDbright){
  pos_start=constrain(pos_start,0,NUM_LEDS);
  pos_end=constrain(pos_end,0,NUM_LEDS);
  for (int x=pos_start; x<pos_end;x++){
   leds[x] = CHSV(LEDhue, LEDsat, LEDbright);
  } 
}



//===================================================================================================================================================
// changeLEDs(): This function is used to change the LEDs from one HSV value to another HSV value in a certain period of time. IT creates a soft transition
// durantion in ms, HSV values in 255by
// to make sure the loop works smoothly the min time of it is 30ms
// /!\ WARNING you have to be careful if you are going through the 255<->0, this function will not necessarily go the way you want it!!!
//===================================================================================================================================================
void changeLEDs(CHSV hsv_top_start,CHSV hsv_bot_start,CHSV hsv_top_end,CHSV hsv_bot_end,int number_loop){

  //speed of change
  float H_top=(long)hsv_top_start.h;
  float S_top=(long)hsv_top_start.s;
  float V_top=(long)hsv_top_start.v;

  long H_bot=(long)hsv_bot_start.h;
  long S_bot=(long)hsv_bot_start.s;
  long V_bot=(long)hsv_bot_start.v;

  float coeff_H_top=((float)hsv_top_end.h-(float)hsv_top_start.h)/(float)number_loop; 
  float coeff_S_top=((float)hsv_top_end.s-(float)hsv_top_start.s)/(float)number_loop;
  float coeff_V_top=((float)hsv_top_end.v-(float)hsv_top_start.v)/(float)number_loop;
  
  for(int i=0; i<number_loop; i++)
  {
    //init loop
    int loop_left=constrain(number_loop-(2*i),1,number_loop);
    H_top=(long)hsv_top_start.h+(float)i*(float)coeff_H_top; 
    S_top=(long)hsv_top_start.s+(float)i*(float)coeff_S_top;
    V_top=(long)hsv_top_start.v+(float)i*(float)coeff_V_top;
    
    //show LED
    fill_gradient(leds,0,CHSV((int)H_top,(int)S_top,(int)V_top),NUM_LEDS,CHSV(H_bot/100,S_bot/100,V_bot/100),SHORTEST_HUES);
    FastLED.show();

    //delay to keep the loop at 50ms
    delay(50);

  }

}


