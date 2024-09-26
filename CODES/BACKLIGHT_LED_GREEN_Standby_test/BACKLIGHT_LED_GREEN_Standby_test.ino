// COMM 4 - Current PC Conection for Ref

/*
 * OCT 2021 VERSION 1.1 - TESTED WORKING
DCS BACKLIGHTING CODE BY BEN MELROSE
TO CONTROL OPEN HORNET WS2812B LED BACK LIGHT PANELS BY L.WALKER
CODE IS CURRENTLY WRITTEN FOR 3 CHANNELS OF BACKLIGHTING
CODE WILL BE UPDATED TO A FULL PIT CODE DURING TESTING
NVD / DAY / NIGHT SWITCH ADDED

 */

#include "FastLED.h"
#define DCSBIOS_IRQ_SERIAL

#include "DcsBios.h"
//
#define NUM_LEDS_CHA 7 // Open Hornet SPIN RCVRY Backlight PCB Version 2.0 by L Walker
#define DATA_PIN_CHA 0// ARDUINO PIN that SPIN RCVRY is connected to

#define LED_TYPE    WS2812B


CRGB leds_A[NUM_LEDS_CHA];


void setup() 
{ 
 
FastLED.addLeds<WS2812B, DATA_PIN_CHA, RGB>(leds_A, NUM_LEDS_CHA);   
  

//TEST ALL LEDS -RED-BLUE-BLACK-GREN
FastLED.addLeds<WS2812B, DATA_PIN_CHA, RGB>(leds_A, NUM_LEDS_CHA);   
fill_solid(leds_A, NUM_LEDS_CHA, CRGB::Green); // Set all Console LEDs to ##GREEN##. 
  FastLED.setBrightness(200);
   FastLED.show();
delay (200);
leds_A[1].setRGB(100, 100, 100); 

    FastLED.show();


}

void loop() {


}

//END
