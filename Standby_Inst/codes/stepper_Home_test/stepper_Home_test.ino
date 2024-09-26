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
AccelStepper stepperH(1, 47, 35);   // 1 = Easy Driver interface
//VRT
//AccelStepper stepperV(1, 35, 33);   // 1 = Easy Driver interface

// Define the Pins used
#define home_switchH 24 // HOZ
//#define home_switchV 25 // VRT

//#define EN_switchV 49 // VER
#define EN_switchH 43 // HOZ

long initial_homingH = 0; // Used to Home Stepper at startup
//long initial_homingV = 0; // Used to Home Stepper at startup


void setup() {
  Serial.begin(9600);  // Start the Serial monitor with speed of 9600 Bauds

//  pinMode(home_switchV, INPUT_PULLUP);
 pinMode(home_switchH, INPUT_PULLUP);

//  pinMode(EN_switchV, OUTPUT);
  pinMode(EN_switchH, OUTPUT);

  delay(5);  // Wait for EasyDriver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperH.setMaxSpeed(800.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperH.setAcceleration(200.0);  // Set Acceleration of Stepper

 // stepperV.setMaxSpeed(800.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
 // stepperV.setAcceleration(200.0);  // Set Acceleration of Stepper

  // Start Homing procedure of Stepper Motor at startup

  Serial.print("Stepper HOZ is Homing . . . . . . . . . . . ");

  digitalWrite(EN_switchH, LOW);


  while (digitalRead(home_switchH)) {  // Make the Stepper move CCW until the switch is activated
    stepperH.moveTo(initial_homingH);  // Set the position to move to
    initial_homingH--;  // Decrease by 1 for next move if needed
    stepperH.run();  // Start moving the stepper
    delay(1);
  }

  stepperH.setCurrentPosition(-1000);
  Serial.println("Homing HOZ Completed");
  Serial.println("");
  stepperH.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperH.setAcceleration(1000.0);  // Set Acceleration of Steppe  Serial.print("Stepper VER is Homing . . . . . . . . . . . ");


  digitalWrite(EN_switchH, HIGH);


 // digitalWrite(EN_switchV, LOW);
/*

  Serial.print("Stepper VER is Homing . . . . . . . . . . . ");

  while (digitalRead(home_switchV)) {  // Make the Stepper move CCW until the switch is activated
    stepperV.moveTo(initial_homingV);  // Set the position to move to
    initial_homingV++;  // Decrease by 1 for next move if needed
    //initial_homing--;  // Decrease by 1 for next move if needed
    stepperV.run();  // Start moving the stepper
    delay(1);
  }

  stepperV.setCurrentPosition(1000);
  Serial.println("Homing VER Completed");
  Serial.println("");
  stepperV.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperV.setAcceleration(1000.0);  // Set Acceleration of Stepper


  digitalWrite(EN_switchV, HIGH);

*/

  stepperH.setMaxSpeed(1000.0);
  stepperH.setAcceleration(200.0);
  stepperH.moveTo(1800);
  stepperH.enableOutputs();

  //stepperV.setMaxSpeed(1000.0);
 // stepperV.setAcceleration(200.0);
  //stepperV.moveTo(1850);
 // stepperV.enableOutputs();
}
void loop()
{
  digitalWrite(EN_switchH, LOW);
  if (stepperH.distanceToGo() == 0)
    stepperH.moveTo(-stepperH.currentPosition());
  stepperH.run();
  digitalWrite(EN_switchH, HIGH);
 // digitalWrite(EN_switchV, LOW);
//  if (stepperV.distanceToGo() == 0)
//    stepperV.moveTo(-stepperV.currentPosition());
//  stepperV.run();
//  digitalWrite(EN_switchV, HIGH);
}
