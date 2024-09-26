#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

#include <AccelStepper.h>

// AccelStepper Setup
AccelStepper stepperVER(1, 35, 33);   // X=1 = Easy Driver interface Y=STEP - Z=DIR
AccelStepper stepperHOZ(1, 41, 39);   // X=1 = Easy Driver interface Y=STEP - Z=DIR
AccelStepper stepperW_L(1, 29, 31);   // X=1 = Easy Driver interface Y=STEP - Z=DIR

// Define the Pins used

//STEPPER ENABLE / DIABLE PINS
#define VERenablePin 37
#define HOZenablePin 43
#define W_LenablePin 27

//IR SENSOR PINS
#define VER_IR 25
#define HOZ_IR 24
#define W_L_IR 22 // NOTE ISSUE ON PCB // 23 FOR SARI BALL

unsigned int valC_Line = 0;
unsigned int valH_Line = 0;
unsigned int valW_Line = 0;

void setup() {
  DcsBios::setup();

  pinMode(VERenablePin, OUTPUT);
  digitalWrite(VERenablePin, LOW); // turn stepper ON
  pinMode(VER_IR, INPUT_PULLUP);

  pinMode(HOZenablePin, OUTPUT);
  digitalWrite(HOZenablePin, LOW); // turn stepper ON
  pinMode(HOZ_IR, INPUT_PULLUP);

  pinMode(W_LenablePin, OUTPUT);
  digitalWrite(W_LenablePin, LOW); // turn stepper ON
  pinMode(W_L_IR, INPUT_PULLUP);


  delay(5);  // Wait for EasyDriver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperVER.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperVER.setAcceleration(400.0);  // Set Acceleration of Stepper
  stepperHOZ.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperHOZ.setAcceleration(400.0);  // Set Acceleration of Stepper
  stepperW_L.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperW_L.setAcceleration(400.0);  // Set Acceleration of Stepper


  //// _______----------------_________VERTICAL - CENTRE LINE SET UP_______----------------_________////
  stepperVER.setCurrentPosition(0);
  stepperVER.moveTo(10); // LEFT
  while (stepperVER.distanceToGo() != 0) {
    stepperVER.run();
    delay(20);
  }
  delay(100);
  stepperVER.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(VER_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
    stepperVER.move(-20);  // Set the position to move to RIGHT
    stepperVER.run();  // Start moving the stepper
    delay(1);
  }
  stepperVER.setCurrentPosition(0); // SET STEPPER BACK TO 0
  stepperVER.setAcceleration(1000.0);  // Set Acceleration of Stepper
  stepperVER.moveTo(1150 * 2); // LEFT
  while (stepperVER.distanceToGo() != 0) {
    stepperVER.run();
    delay(2);
  }
  digitalWrite(VERenablePin, HIGH); // turn stepper off for now
  //// _______----------------_________VERTICAL - CENTRE LINE SET UP_______----------------_________////

  //// _______---------------_________HORIZONTAL - GLIDESLOPE SET UP_______---------------_________////
  stepperHOZ.setCurrentPosition(0);
  stepperHOZ.moveTo(10); // LEFT
  while (stepperHOZ.distanceToGo() != 0) {
    stepperHOZ.run();
    delay(20);
  }
  delay(100);
  stepperHOZ.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(HOZ_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
    stepperHOZ.move(-20);  // Set the position to move to RIGHT
    stepperHOZ.run();  // Start moving the stepper
    delay(1);
  }
  stepperHOZ.setCurrentPosition(0); // SET STEPPER BACK TO 0
  stepperHOZ.setAcceleration(1000.0);  // Set Acceleration of Stepper
  stepperHOZ.moveTo(10); // LEFT
  
  while (stepperHOZ.distanceToGo() != 0) {
    stepperHOZ.run();
    delay(2);
  }
  digitalWrite(HOZenablePin, HIGH); // turn stepper off for now
  //// _______---------------_________HORIZONTAL - GLIDESLOPE SET UP_______---------------_________////

  //// _______---------------------_________WATERLINE - SET UP_______---------------------_________////
  stepperW_L.setCurrentPosition(0);
  stepperW_L.moveTo(10); // LEFT
  while (stepperW_L.distanceToGo() != 0) {
    stepperW_L.run();
    delay(20);
  }
  delay(100);
  stepperW_L.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(W_L_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
    stepperW_L.move(-20);  // Set the position to move to RIGHT
    stepperW_L.run();  // Start moving the stepper
    delay(1);
  }
  stepperW_L.setCurrentPosition(0); // SET STEPPER BACK TO 0
  stepperW_L.setAcceleration(500.0);  // Set Acceleration of Stepper
  stepperW_L.moveTo(1000 * 2); // LEFT
  while (stepperW_L.distanceToGo() != 0) {
    stepperW_L.run();
    delay(2);
  }
  digitalWrite(W_LenablePin, HIGH); // turn stepper off for now
  //// _______---------------------_________WATERLINE - SET UP_______---------------------_________////


} // END OF VOID SETUP

//// STEPPERS MOVE CODE FROM DCS BIOS////

void onSaiPointerVerChange(unsigned int C_LValue) {
  valC_Line = map(C_LValue, 0, 65535, 1500 * 2, 100);
  stepperVER.moveTo(valC_Line);
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);

void onSaiPointerHorChange(unsigned int H_LValue) {
  valH_Line = map(H_LValue, 0, 65535, 1500 * 2, 10);
  stepperHOZ.moveTo(valH_Line);
}
DcsBios::IntegerBuffer saiPointerHorBuffer(0x756c, 0xffff, 0, onSaiPointerHorChange);

void onSaiManPitchAdjChange(unsigned int W_LValue) {
  valW_Line = map(W_LValue, 0, 65535, 800 * 2, 300);
  stepperW_L.moveTo(valW_Line);
}
DcsBios::IntegerBuffer saiManPitchAdjBuffer(0x74ea, 0xffff, 0, onSaiManPitchAdjChange);

void loop() {

  DcsBios::loop();

  //STEPPER DISABLE WHEN NOT IN USE
  if (stepperVER.distanceToGo() != 0) {
    digitalWrite(VERenablePin, LOW); // turn stepper ON
    stepperVER.run();
  } else {
    digitalWrite(VERenablePin, HIGH); // turn stepper OFF
  }
  //////////////////////////////////////////////////////////
  if (stepperHOZ.distanceToGo() != 0) {
    digitalWrite(HOZenablePin, LOW); // turn stepper ON
    stepperHOZ.run();
  } else {
    digitalWrite(HOZenablePin, HIGH); // turn stepper OFF
  }
  //////////////////////////////////////////////////////////
  if (stepperW_L.distanceToGo() != 0) {
    digitalWrite(W_LenablePin, LOW); // turn stepper ON
    stepperW_L.run();
  } else {
    digitalWrite(W_LenablePin, HIGH); // turn stepper OFF
  }
  //////////////////////////////////////////////////////////


}// END OF VOID LOOP
