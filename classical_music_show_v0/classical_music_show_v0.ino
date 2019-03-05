

//==============LED strip part=================================================================

//This project needs the FastLED library - link in the description.
#include "FastLED.h"

//The total number of LEDs being used is 144
#define NUM_LEDS 60

// The data pin for the NeoPixel strip is connected to digital Pin 6 on the Arduino
#define DATA_PIN 6

//Initialise the LED array, the LED Hue (ledh) array, and the LED Brightness (ledb) array.
CRGB leds[NUM_LEDS];
byte ledh[NUM_LEDS];
byte ledb[NUM_LEDS];

//Pin connections
//const int potPin = A0;      // The potentiometer signal pin is connected to Arduino's Analog Pin 0
//const int yPin = A4;        // Y pin on accelerometer is connected to Arduino's Analog Pin 4
                            // The accelerometer's X Pin and the Z Pin were not used in this sketch

//Global Variables ---------------------------------------------------------------------------------
byte potVal=0;                // potVal:      manages color
byte prevPotVal=0;          // prevPotVal:  stores the previous potentiometer value
int LEDSpeed=50;             // LEDSpeed:    stores the "speed" of the LED animation sequence
int maxLEDSpeed = 300;       // maxLEDSpeed: identifies the maximum speed of the LED animation sequence
int LEDAccel=0;             // LEDAccel:    stores the acceleration value of the LED animation sequence (to speed it up or slow it down)
int LEDPosition=29;         // LEDPosition: identifies the LED within the strip to modify (leading LED). The number will be between 0-143.  (Zero to NUM_LEDS-1)
int oldPos=0;               // oldPos:      holds the previous position of the leading LED
byte hue = 0;               // hue:         stores the leading LED's hue value
byte intensity_origin = 100; // intensity_origin:   the base brightness of the leading LED
byte intensity=intensity_origin;//intensity of the LEDs
int changeIntensity=50;     // defines the weight of the intensity variation
byte bright = 80;           // bright:      this variable is used to modify the brightness of the trailing LEDs
int animationDelay = 0;     // animationDelay: is used in the animation Speed calculation. The greater the animationDelay, the slower the LED sequence.
int effect = 2;             // effect:      is used to differentiate and select one out of the four effects
int sparkTest = 0;          // sparkTest:   variable used in the "sparkle" LED animation sequence 
boolean constSpeed = true; // constSpeed:  toggle between constant and variable speed.
int changeColorSpeed=5;   //defines the speed of color change 2 to 255
boolean bounce=false;     //boolean to define if the LED should bounce or start from the LEDPosition

//==============LED strip part- end=================================================================


//==============button part=================================================================

// set pin numbers:
const int buttonPin = 4;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// variables will change:
bool buttonState = HIGH;         // variable for reading the current pushbutton status
bool previous_buttonState=HIGH;
bool lightState =HIGH;                 // variable for light status
bool previous_lightState=HIGH; 

//==============button part - end=================================================================



void setup() {
//==============button part=================================================================  
  pinMode(ledPin, OUTPUT);  // initialize the LED pin as an output:
  pinMode(buttonPin, INPUT_PULLUP);     // initialize the pushbutton pin as an input:

//==============led part=================================================================
  delay(2000);          //Delay for two seconds to power the LEDS before starting the data signal on the Arduino
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);                            //initialise the LED strip 
  
  //DEBUG///
  Serial.begin(9600); 
}

void loop() {
//==============button part=================================================================
//  button_state();
//  light_change();
//  readPotentiometer(); 
  while(true){
    bool lightChange=false;
    lightChange=button_state();
    changeColor();
    changeBrightness();          
    adjustSpeed();
    constrainLEDs();
    cometEffect();
    
      if (lightChange==true){
        delay(100);
        break;
        }
   }
   while(true){
    bool lightChange=false;
    lightChange=button_state();
//    fadeLEDs(100);
        for(int led = 0; led < NUM_LEDS/2; led++) { 
          showLED(NUM_LEDS/2+led, 0, 0, 0); 
          showLED(NUM_LEDS/2-1-led, 0, 0, 0);
        }
      if (lightChange==true){
        delay(100);
        break;
        }
   }
  

}


/////////////////////////////////////////////////////////////////////////////////
// Button state - returns true or false if there is a change in the button state
/////////////////////////////////////////////////////////////////////////////////


bool button_state(){
    // read the state of the pushbutton value:
  previous_buttonState=buttonState;
  buttonState = digitalRead(buttonPin);


    // check if the button state changed
  if (buttonState==LOW && previous_buttonState==HIGH) {
    previous_buttonState=buttonState;
      return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Light change
/////////////////////////////////////////////////////////////////////////////////


void light_change(){
  if (lightState==HIGH) {
    digitalWrite(ledPin, HIGH);
      previous_lightState=HIGH;
//      adjustSpeed();
   while(true){
    digitalWrite(ledPin, HIGH);
      constrainLEDs();                                              // 3rd effect : Comet effect. Hue controlled by potentiometer, direction by accelerometer
      cometEffect();
      delay(100);
      digitalWrite(ledPin, LOW);
      if (button_state()==true){
        break;
        }
   }
  }
  if (lightState==LOW)  {
    digitalWrite(ledPin, LOW);
    previous_lightState=LOW;
    fadeLEDs(100);
  }
  delay(100);
}

/////////////////////////////////////////////////////////////////////////////////

//===================================================================================================================================================
// lightOff() : Randomly makes the LED color evolve
//===================================================================================================================================================
//void changeColor(){
//  long delta=random(0,changeColorSpeed);    //random number
//  potVal=potVal+delta;                      //add to the potVal which defines color in the functions
//  if(potVal>255){
//    potVal=0+delta;
//  }
//}




//===================================================================================================================================================
// readPotentiometer() : Take a potentiometer reading. This value will be used to control various LED animations, and to choose the animation sequence to display.
//===================================================================================================================================================
void readPotentiometer(){
  //Take a reading from the potentiometer and convert the value into a number between 0 and 255
//  potVal = map(analogRead(potPin), 0, 1023 , 0, 255);
  
  // If the potentiometer reading is equal to zero, then move to the next effect in the list.
  if(potVal==0){
    if(prevPotVal>0){   // This allows us to switch effects only when the potentiometer reading has changed to zero (from a positive number). Multiple zero readings will be ignored.
      prevPotVal = 0;   // Set the prev pot value to zero in order to ignore replicate zero readings.
      effect++;         // Go to the next effect.
      if(effect>4){
        effect=0;       // Go back to the first effect after the fifth effect.
      }
    }
  }
  prevPotVal=potVal;    // Keep track of the previous potentiometer reading
}

//===================================================================================================================================================

//===================================================================================================================================================
// changeColor() : Randomly makes the LED color evolve
//===================================================================================================================================================
void changeColor(){
  long delta=random(0,changeColorSpeed);    //random number
  potVal=potVal+delta;                      //add to the potVal which defines color in the functions
  if(potVal>255){
    potVal=0+delta;
  }
}

//===================================================================================================================================================
// changeBrightness() : Change the brightness of the LED
//===================================================================================================================================================
int i_cB=0;

void changeBrightness(){
  i_cB++;
  if(i_cB==10){
    i_cB=0;
    long delta=random(-100,100);
    intensity=intensity+delta;
    bright=bright+delta;
  }

}




//===================================================================================================================================================
// adjustSpeed() : use the Y axis value of the accelerometer to adjust the speed and the direction of the LED animation sequence
//===================================================================================================================================================
void adjustSpeed(){
  // Take a reading from the Y Pin of the accelerometer and adjust the value so that 
  // positive numbers move in one direction, and negative numbers move in the opposite diraction. 
  // We use the map function to convert the accelerometer readings, and the constrain function to ensure that it stays within the desired limits
  // The values of 230 and 640 were determined by trial and error and are specific to my accelerometer. You will need to adjust these numbers to suit your module.
  
  LEDAccel = 0;   //constrain(map(analogRead(yPin), 230, 640 , maxLEDSpeed, -maxLEDSpeed),-maxLEDSpeed, maxLEDSpeed);
  
  
  // If the constSpeed variable is "true", then make sure that the speed of the animation is constant by modifying the LEDSpeed and LEDAccel variables.
  if(constSpeed){
    LEDAccel=0; 
    if(LEDSpeed>0){
      LEDSpeed = maxLEDSpeed/1.1;     // Adjust the LEDSpeed to half the maximum speed in the positive direction
    } 
    if (LEDSpeed<0){
      LEDSpeed = -maxLEDSpeed/1.1;    // Adjust the LEDSpeed to half the maximum speed in the negative direction
    }
  } 
 
  // The Speed of the LED animation sequence can increase (accelerate), decrease (decelerate) or stay the same (constant speed)
  LEDSpeed = LEDSpeed + LEDAccel;                        
  
  //The following lines of code are used to control the direction of the LED animation sequence, and limit the speed of that animation. 
  if (LEDSpeed>0){
    LEDPosition++;                                       // Illuminate the LED in the Next position
    if (LEDSpeed>maxLEDSpeed){
      LEDSpeed=maxLEDSpeed;                              // Ensure that the speed does not go beyond the maximum speed in the positive direction
    }
  }
  
  if (LEDSpeed<0){
    LEDPosition--;                                       // Illuminate the LED in the Prior position
    if (LEDSpeed<-maxLEDSpeed){
      LEDSpeed = -maxLEDSpeed;                           // Ensure that the speed does not go beyond the maximum speed in the negative direction
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
// cylonWithHueControl() :  This is the 1st LED effect. The cylon colour is controlled by the potentiometer. The speed is constant.
//===================================================================================================================================================
void cylonWithHueControl(){
      constSpeed = true;                                  // Make the LED animation speed constant
      showLED(LEDPosition, potVal, 255, intensity);       // Illuminate the LED
      fadeLEDs(8);                                        // Fade LEDs by a value of 8. Higher numbers will create a shorter tail.
      setDelay(LEDSpeed);                                 // The LEDSpeed is constant, so the delay is constant
}


//===================================================================================================================================================
// cylonWithBrightnessControl() : This is the 2nd LED effect. The cylon colour is red (hue=0), and the brightness is controlled by the potentiometer
//===================================================================================================================================================
void cylonWithBrightnessControl(){
      constSpeed = true;                                  // Make speed constant
      showLED(LEDPosition, 0, 255, intensity);               // Brightness is controlled by potentiometer.
      fadeLEDs(16);                                       // Fade LEDs by a value of 16
      setDelay(LEDSpeed);                                 // The LEDSpeed is constant, so the delay is constant
}


//===================================================================================================================================================
// cometEffect() :  This is the 3rd LED effect. The random brightness of the trailing LEDs produces an interesting comet-like effect.
//===================================================================================================================================================
void cometEffect(){
      constSpeed = true;                                  // The speed will be controlled by the slope of the accelerometer (y-Axis)
      showLED(LEDPosition, potVal, 255, intensity);        // Hue will change with potentiometer.
      
      //The following lines create the comet effect 
      bright = random(50, 100);                            // Randomly select a brightness between 50 and 100
      leds[LEDPosition] = CHSV((potVal+40),255, bright);   // The trailing LEDs will have a different hue to the leading LED, and will have a random brightness
      fadeLEDs(8);                                         // This will affect the length of the Trailing LEDs
      setDelay(LEDSpeed);                                  // The LEDSpeed will be affected by the slope of the Accelerometer's y-Axis
}


//===================================================================================================================================================
// fireStarter() : This is the 4th LED effect. It starts off looking like a ball of fire, leaving a trail of little fires. But as you
//                 turn the potentiometer, it becomes more like a shooting star with a rainbow-sparkle trail.
//===================================================================================================================================================
void fireStarter(){
      constSpeed = false;                                  // The speed will be controlled by the slope of the accelerometer (y-Axis)
      ledh[LEDPosition] = potVal;                          // Hue is controlled by potentiometer
      showLED(LEDPosition, ledh[LEDPosition], 255, intensity); 
      
      //The following lines create the fire starter effect
      bright = random(50, 100);                            // Randomly select a brightness between 50 and 100
      ledb[LEDPosition] = bright;                          // Assign this random brightness value to the trailing LEDs
      sparkle(potVal/5);                                   // Call the sparkle routine to create that sparkling effect. The potentiometer controls the difference in hue from LED to LED.
      fadeLEDs(1);                                         // A low number creates a longer tail
      setDelay(LEDSpeed);                                  // The LEDSpeed will be affected by the slope of the Accelerometer's y-Axis
}


//===================================================================================================================================================
// levelSense() : This is the 5th and final LED effect. The accelerometer is used in conjunction with the LED strip to create a digital "Spirit" Level.
//                You can use the illuminated LEDs to identify the angle of the LED strip
//===================================================================================================================================================
//void levelSense(){
//      constSpeed = true;
//      LEDPosition = constrain(map(analogRead(yPin), 230, 640, 1, NUM_LEDS-1), 0 , NUM_LEDS-1);
//      
//      //Jitter correction: this will reduce the amount of jitter caused by the accelerometer reading variability
//      if(abs(LEDPosition-oldPos) < 2){
//        LEDPosition = oldPos;
//      }
//      
//      //The following lines of code will ensure the colours remain within the red to green range, with green in the middle and red at the ends.
//      hue = map(LEDPosition, 0, NUM_LEDS-1, 0, 200);
//      if (hue>100){
//         hue = 200 - hue;
//      }
//      
//      //Illuminate 2 LEDs next to each other
//      showLED(LEDPosition, hue, 255, intensity); 
//      showLED(LEDPosition-1, hue, 255, intensity);              
//      
//      //If the position moves, then fade the old LED positions by a factor of 25 (high numbers mean shorter tail)
//      fadeLEDs(25);                               
//      oldPos = LEDPosition; 
//}


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


//===================================================================================================================================================
// sparkle() : is used by the fireStarter routine to create a sparkling/fire-like effect
//             Each LED hue and brightness is monitored and modified using arrays  (ledh[]  and ledb[])
//===================================================================================================================================================
void sparkle(byte hDiff){
  for(int i = 0; i < NUM_LEDS; i++) {
    ledh[i] = ledh[i] + hDiff;                // hDiff controls the extent to which the hue changes along the trailing LEDs
    
    // This will prevent "negative" brightness.
    if(ledb[i]<3){
      ledb[i]=0;
    }
    
    // The probability of "re-igniting" an LED will decrease as you move along the tail
    // Once the brightness reaches zero, it cannot be re-ignited unless the leading LED passes over it again.
    if(ledb[i]>0){
      ledb[i]=ledb[i]-2;
      sparkTest = random(0,bright);
      if(sparkTest>(bright-(ledb[i]/1.1))){
        ledb[i] = bright;
      } else {
        ledb[i] = ledb[i] / 2;                  
      }
    }
    leds[i] = CHSV(ledh[i],255,ledb[i]);
  }
}




