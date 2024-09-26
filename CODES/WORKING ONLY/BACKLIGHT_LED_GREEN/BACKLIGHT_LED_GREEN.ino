#include <FastLED.h>


#define NUM_LEDS_CHA 14
#define DATA_PIN_CHA A4
#define LED_TYPE    WS2812

#define MAX_BRIGHTNESS 100    // Thats full on, watch the power!
#define MIN_BRIGHTNESS 100       // set to a minimum of 25%
//const int brightnessInPin = A4;  // The Analog input pin that the brightness control potentiometer is attached to.

// Define the array of leds
CRGB leds_A[NUM_LEDS_CHA];




void setup() 
{ 
  

FastLED.addLeds<WS2812B, DATA_PIN_CHA, RGB>(leds_A, NUM_LEDS_CHA);   
fill_solid(leds_A, NUM_LEDS_CHA, CRGB::Red); // Set all Console LEDs to ##GREEN##. 
  FastLED.setBrightness(250);
   FastLED.show();
}

void loop() 
{ 
FastLED.addLeds<WS2812B, DATA_PIN_CHA, RGB>(leds_A, NUM_LEDS_CHA);   
fill_solid(leds_A, NUM_LEDS_CHA, CRGB::Green); // Set all Console LEDs to ##GREEN##. 
  FastLED.setBrightness(250);
   FastLED.show();
   delay(1000);
   FastLED.addLeds<WS2812B, DATA_PIN_CHA, RGB>(leds_A, NUM_LEDS_CHA);   
fill_solid(leds_A, NUM_LEDS_CHA, CRGB::Red); // Set all Console LEDs to ##GREEN##. 
  FastLED.setBrightness(250);
   FastLED.show();
   delay(1000);
   
}
