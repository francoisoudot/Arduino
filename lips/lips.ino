#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>

#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>


/* ==================================================================================================================================================
         Project: NeoPixel Playground
Neopixel chipset: ws2812B  (144 LED/m strip)
          Author: Scott C
         Created: 12th June 2015
     Arduino IDE: 1.6.4
         Website: http://arduinobasics.blogspot.com/p/arduino-basics-projects-page.html
     Description: This project will allow you to cycle through and control five LED
                  animation sequences using a potentiometer and an accelerometer
                     Sequence 1:   Cylon with Hue Control                                       Control: Potentiometer only
                     Sequence 2:   Cylon with Brightness Control                                Control: Potentiometer only
                     Sequence 3:   Comet effect with Hue and direction control                  Control: Potentiometer and Accelerometer (Y axis only)
                     Sequence 4:   FireStarter / Rainbow effect with Hue and Direction control  Control: Potentiometer and Accelerometer (Y axis only)
                     Sequence 5:   Digital Spirit Level                                         Control: Accelerometer only (Y axis)
            
                  This project makes use of the FastLED library. Some of the code below was adapted from the FastLED library examples (eg. Cylon routine).
                  The Comet, FireStarter and Digital Spirit Level sequence was designed by ScottC.
                  The FastLED library can be found here: http://fastled.io/
                  You may need to modify the code below to accomodate your specific LED strip. See the FastLED library site for more details.
===================================================================================================================================================== */


//The total number of LEDs being used is 144
#define NUM_LEDS 30

// The data pin for the NeoPixel strip is connected to digital Pin 6 on the Arduino
#define DATA_PIN 7

//Initialise the LED array, the LED Hue (ledh) array, and the LED Brightness (ledb) array.
CRGB leds[NUM_LEDS];
byte ledh[NUM_LEDS];
byte ledb[NUM_LEDS];

//Pin connections
//const int potPin = A0;      // The potentiometer signal pin is connected to Arduino's Analog Pin 0
//const int yPin = A4;        // Y pin on accelerometer is connected to Arduino's Analog Pin 4
                            // The accelerometer's X Pin and the Z Pin were not used in this sketch

//Global Variables ---------------------------------------------------------------------------------
byte potVal=60;                // potVal:      manages color
byte potVal1=60;                // potVal:      manages color1
byte potVal2=60;                // potVal:      manages color2


byte prevPotVal=0;          // prevPotVal:  stores the previous potentiometer value
int LEDSpeed=3;             // LEDSpeed:    stores the "speed" of the LED animation sequence
int maxLEDSpeed = 300;       // maxLEDSpeed: identifies the maximum speed of the LED animation sequence
int LEDAccel=0;             // LEDAccel:    stores the acceleration value of the LED animation sequence (to speed it up or slow it down)
int LEDPosition=29;         // LEDPosition: identifies the LED within the strip to modify (leading LED). The number will be between 0-143.  (Zero to NUM_LEDS-1)
int oldPos=0;               // oldPos:      holds the previous position of the leading LED
byte hue = 0;               // hue:         stores the leading LED's hue value
byte intensity_origin = 255; // intensity_origin:   the base brightness of the leading LED
byte intensity=intensity_origin;//intensity of the LEDs
int changeIntensity=50;     // defines the weight of the intensity variation
byte bright = 80;           // bright:      this variable is used to modify the brightness of the trailing LEDs
int animationDelay = 0;     // animationDelay: is used in the animation Speed calculation. The greater the animationDelay, the slower the LED sequence.
int effect = 2;             // effect:      is used to differentiate and select one out of the four effects
int sparkTest = 0;          // sparkTest:   variable used in the "sparkle" LED animation sequence 
boolean constSpeed = true; // constSpeed:  toggle between constant and variable speed.
int changeColorSpeed=50;   //defines the speed of color change 2 to 255
boolean bounce=false;     //boolean to define if the LED should bounce or start from the LEDPosition
int i;




//===================================================================================================================================================
// setup() : Is used to initialise the LED strip
//===================================================================================================================================================
void setup() {
    delay(2000);          //Delay for two seconds to power the LEDS before starting the data signal on the Arduino
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);                            //initialise the LED strip 
    potVal1=30;
    potVal2=175;
    intensity=255;
}


//===================================================================================================================================================
// loop() : The Arduino will take readings from the potentiometer and accelerometer to control the LED strip
//===================================================================================================================================================
void loop(){
// colors - red =0 and 250 / orange= 20 / yellow=30/ green=80/ cyan=110/ blue= 150  purple=200 /magenta=220 /rose=240
// colors to use: dark blue=175
    for(int led = 1; led < 31; led++) { 
            showLED(led, potVal1, 255, intensity); 
    }
   i=random(1,31);
   newColor();
   changeColor();
        

}


//===================================================================================================================================================
// changeColor() : Changes color between yellow blue red
//===================================================================================================================================================
void changeColor(){
  if (potVal1=30){ potVal1=175; potVal2=0;}
  if (potVal1=175){ potVal1=0; potVal2=30;}
  if (potVal1=0){ potVal1=30; potVal2=175;}  
}


//===================================================================================================================================================
// newColor() : Randomly makes the LED color evolve
//===================================================================================================================================================
void newColor(){
    
    for(int led = 1; led < 16; led++) {
         int i1=led+i;
         int i2=led+i+15;
         if (i1>30){
          i1= i1-30;
           }
         if (i2>30){
          i2= i2-30;
           }
       
          showLED(i1, potVal2, 255, intensity); 
          showLED(i2, potVal2, 255, intensity); 
          delay(1000);
     }
}

//===================================================================================================================================================
// changeBrightness() : Change the brightness of the LED
//===================================================================================================================================================

void changeBrightness(){
    long delta=random(-50,50);
    intensity=intensity+delta;
    bright=bright+delta;
}

//===================================================================================================================================================
// downBrightness() : Decrease the brightness of the LED
//===================================================================================================================================================


void downBrightness(){
   for(int i_dB=intensity/10; i_dB > 1; i_dB--) { 
        intensity=i_dB*10;
        for(int led = 0; led < NUM_LEDS/2; led++) { 
          showLED(NUM_LEDS/2+led, potVal, 255, intensity); 
          showLED(NUM_LEDS/2-1-led, potVal, 255, intensity);
          delay(10);
        }
    }
}

//===================================================================================================================================================
// upBrightness() : Decrease the brightness of the LED
//===================================================================================================================================================


void upBrightness(){
   for(int i_dB=intensity/10; i_dB < 15; i_dB++) { 
        intensity=i_dB*10;
        for(int led = 0; led < NUM_LEDS/2; led++) { 
          showLED(NUM_LEDS/2+led, potVal, 255, intensity); 
          showLED(NUM_LEDS/2-1-led, potVal, 255, intensity);
          delay(10);
        }
    }
}



//===================================================================================================================================================
// constrainLEDs() : This ensures that the LED animation sequence remains within the boundaries of the various arrays (and the LED strip)
//                   and it also creates a "bouncing" effect at both ends of the LED strip.
//===================================================================================================================================================
void constrainLEDs(){
  
  LEDPosition = constrain(LEDPosition, 0, NUM_LEDS-1);    // Make sure that the LEDs stay within the boundaries of the LED strip
  if(bounce){
    if(LEDPosition == 0 || LEDPosition == NUM_LEDS-1) {  
        LEDSpeed = (LEDSpeed * -0.9);                         // Reverse the direction of movement when LED gets to end of strip. This creates a bouncing ball effect.
      }
  }
  else{
    if(LEDPosition==0){
      LEDPosition = NUM_LEDS-1;
    }
    if(LEDPosition==NUM_LEDS-1){
      LEDPosition = 0;
    }
  }
}




//===================================================================================================================================================
// fadeLEDs(): This function is used to fade the LEDs back to black (OFF) 
//===================================================================================================================================================
void fadeLEDs(int fadeVal){
  for (int i = 0; i<NUM_LEDS; i++){
    leds[i].fadeToBlackBy( fadeVal );
  }
}



//===================================================================================================================================================
// showLED() : is used to illuminate the LEDs 
//===================================================================================================================================================
void showLED(int pos, byte LEDhue, byte LEDsat, byte LEDbright){
  leds[pos] = CHSV(LEDhue,LEDsat,LEDbright);
  FastLED.show();
}


//===================================================================================================================================================
// setDelay() : is where the speed of the LED animation sequence is controlled. The speed of the animation is controlled by the LEDSpeed variable.
//              and cannot go faster than the maxLEDSpeed variable.
//===================================================================================================================================================
void setDelay(int LSpeed){
  animationDelay = maxLEDSpeed - abs(LSpeed);
  delay(animationDelay);
}

