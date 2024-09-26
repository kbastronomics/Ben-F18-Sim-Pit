#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

#include <AccelStepper.h>

// AccelStepper Setup
AccelStepper stepperVER(1, 35, 33);   // 1 = Easy Driver interface
AccelStepper stepperHOZ(1, 41, 39);   // 1 = Easy Driver interface
AccelStepper stepperW_L(1, 31, 29);   // 1 = Easy Driver interface

// Define the Pins used
//STEPPER ENABLE / DIABLE PINS
#define VERenablePin 37
#define HOZenablePin 43
#define W_LenablePin 37
//IR SENSOR PINS
#define VER_IR 25
#define HOZ_IR 24
#define W_L_IR 23

unsigned int valC_Line = 0;
//unsigned int posC_Line = 0;

unsigned int valH_Line = 0;
//unsigned int posH_Line = 0;

unsigned int valW_Line = 0;
//unsigned int posW_Line = 0;


// Stepper Travel Variables
//long TravelVER;  // Used to store the X value entered in the Serial Monitor
//int move_VERfinished = 1; // Used to check if move is completed
//long initial_VERhoming = -1; // Used to Home Stepper at startup







void setup() {
  DcsBios::setup();
  pinMode(VERenablePin, OUTPUT);
  digitalWrite(VERenablePin, LOW); // turn stepper ON
  pinMode(VER_IR, INPUT_PULLUP);

  delay(5);  // Wait for EasyDriver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperVER.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperVER.setAcceleration(400.0);  // Set Acceleration of Stepper


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
  stepperVER.moveTo(1240*2); // LEFT
  while (stepperVER.distanceToGo() != 0) {
    stepperVER.run();
    delay(2);
  }
  digitalWrite(VERenablePin, HIGH); // turn stepper off for now


}

void onSaiPointerVerChange(unsigned int C_LValue) {
  valC_Line = map(C_LValue, 0, 65535, 1240*2, 100);
  stepperVER.moveTo(valC_Line);
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);


void loop() {

  DcsBios::loop();

  if (stepperVER.distanceToGo() != 0) {
    digitalWrite(VERenablePin, LOW); // turn stepper ON
    stepperVER.run();

  } else {
    digitalWrite(VERenablePin, HIGH); // turn stepper OFF
  }



}
