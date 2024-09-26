
#include "AccelStepper.h"
// Library created by Mike McCauley at http://www.airspayce.com/mikem/arduino/AccelStepper/

// AccelStepper Setup
// AccelStepper Setup
//HOZ
AccelStepper stepperH(1, 41, 39);   // 1 = Easy Driver interface
//VRT
AccelStepper stepperV(1, 35, 33);   // 1 = Easy Driver interface

// Define the Pins used
#define HOZ_IR_sen 24 // HOZ
#define VER_IR_sen 25 // VRT

#define EN_switchV 37 // VER
#define EN_switchH 43 // HOZ


// Stepper Travel Variables

long initial_homingH = 0; // Used to Home Stepper at startup
long initial_homingV = 0; // Used to Home Stepper at startup

void setup() {
  Serial.begin(9600);  // Start the Serial monitor with speed of 9600 Bauds

  pinMode(HOZ_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchH, OUTPUT);
  pinMode(VER_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchV, OUTPUT);
  delay(5);  // Wait for EasyDriver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperH.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperH.setAcceleration(1000.0);  // Set Acceleration of Stepper
  stepperV.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperV.setAcceleration(1000.0);  // Set Acceleration of Stepper


  // Start Homing procedure of Stepper Motor at startup

  Serial.print("Stepper is HOZ Homing . . . ");
  digitalWrite(EN_switchH, LOW);
  if (digitalRead(HOZ_IR_sen) == LOW)
    Serial.println("Stepper is IN the SENSOR . . . ");
  if (digitalRead(HOZ_IR_sen) == HIGH)
    Serial.println("Stepper is OUT of the SENSOR  . . . ");
  while (digitalRead(HOZ_IR_sen)) {  // Make the Stepper move CCW until the switch is activated
    stepperH.moveTo(initial_homingH);  // Set the position to move to
    initial_homingH--;  // Decrease by 1 for next move if needed
    stepperH.run();  // Start moving the stepper
    delay(1);
  }
  stepperH.setCurrentPosition(0);  // Set the current position as zero for now
  stepperH.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperH.setAcceleration(1000.0);  // Set Acceleration of Stepper
  initial_homingH = 1;

  while (!digitalRead(HOZ_IR_sen)) { // Make the Stepper move CW until the switch is deactivated
    stepperH.moveTo(initial_homingH);
    stepperH.run();
    initial_homingH++;
    delay(1);
  }

  stepperH.setCurrentPosition(-600);
  Serial.println("Homing Completed");
  Serial.println("");
  stepperH.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperH.setAcceleration(1000.0);  // Set Acceleration of Stepper


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.print("Stepper VER is Homing . . . ");
  digitalWrite(EN_switchV, LOW);
  if (digitalRead(VER_IR_sen) == LOW)
    Serial.println("Stepper is IN the SENSOR . . . ");
  if (digitalRead(VER_IR_sen) == HIGH)
    Serial.println("Stepper is OUT of the SENSOR  . . . ");
  while (digitalRead(VER_IR_sen)) {  // Make the Stepper move CCW until the switch is activated
    stepperV.moveTo(initial_homingV);  // Set the position to move to
    initial_homingV++;  // Decrease by 1 for next move if needed
    stepperV.run();  // Start moving the stepper
    delay(1);
  }
  stepperV.setCurrentPosition(0);  // Set the current position as zero for now
  stepperV.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperV.setAcceleration(1000.0);  // Set Acceleration of Stepper
  initial_homingV = 1;

  while (!digitalRead(VER_IR_sen)) { // Make the Stepper move CW until the switch is deactivated
    stepperV.moveTo(initial_homingV);
    stepperV.run();
    initial_homingV--;
    delay(1);
  }

  stepperV.setCurrentPosition(900); // 0 is full right
  Serial.println("VER Homing Completed");
  Serial.println("");
  stepperV.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperV.setAcceleration(1000.0);  // Set Acceleration of Stepper
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.println("ALL CALIBRATION COMPLETE");
  //digitalWrite(EN_switchH, HIGH);
  //digitalWrite(EN_switchV, HIGH);


  stepperH.setMaxSpeed(1000.0);
  stepperH.setAcceleration(500.0);
  stepperH.moveTo(1800);
  stepperH.enableOutputs();

  stepperV.setMaxSpeed(1000.0);
  stepperV.setAcceleration(500.0);
  stepperV.moveTo(1850); //left deflection
  stepperV.enableOutputs();
}

void loop() {

  digitalWrite(EN_switchH, LOW);
  if (stepperH.distanceToGo() == 0)
    stepperH.moveTo(-stepperH.currentPosition());
  stepperH.run();
  digitalWrite(EN_switchH, HIGH);

  
  digitalWrite(EN_switchV, LOW);
  if (stepperV.distanceToGo() == 0)
    stepperV.moveTo(-stepperV.currentPosition());
  stepperV.run();
  digitalWrite(EN_switchV, HIGH);
}
