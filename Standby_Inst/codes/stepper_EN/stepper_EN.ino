/*  Motor Homing code using AccelStepper and the Serial Monitor

  Created by Yvan / https://Brainy-Bits.com
  This code is in the public domain...
  You can: copy it, use it, modify it, share it or just plain ignore it!
  Thx!

*/

#include "AccelStepper.h"
// Library created by Mike McCauley at http://www.airspayce.com/mikem/arduino/AccelStepper/

// AccelStepper Setup
//HOZ
AccelStepper stepperR(1, 47, 45);   // 1 = Easy Driver interface
//VRT
AccelStepper stepperV(1, 35, 33);   // 1 = Easy Driver interface

// Define the Pins used
///#define home_switchR 24 // HOZ
//#define home_switchV 25 // VRT

#define EN_switchV 37 // VER
#define EN_switchR 49 // HOZ

//long initial_homingR = 0; // Used to Home Stepper at startup
//long initial_homingV = 0; // Used to Home Stepper at startup


void setup() {
  Serial.begin(9600);  // Start the Serial monitor with speed of 9600 Bauds

//  pinMode(home_switchV, INPUT_PULLUP);
 //pinMode(home_switchH, INPUT_PULLUP);

pinMode(EN_switchV, OUTPUT);
  pinMode(EN_switchR, OUTPUT);

  digitalWrite(EN_switchV, HIGH);
  digitalWrite(EN_switchR, LOW);

}
void loop()
{
}
