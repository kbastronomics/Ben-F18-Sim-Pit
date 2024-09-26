#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

#include <AccelStepper.h>
// Library created by Mike McCauley at http://www.airspayce.com/mikem/arduino/AccelStepper/

// AccelStepper Setup
AccelStepper stepperVER(1, 35, 33);   // 1 = Easy Driver interface
AccelStepper stepperHOZ(1, 41, 39);   // 1 = Easy Driver interface
unsigned int valC_Line = 0;
unsigned int posC_Line = 0;
unsigned int valH_Line = 0;
unsigned int posH_Line = 0;

#define VERT_IR 25
#define HOZ_IR 24

#define VERenablePin 37
#define HOZenablePin 43

// Stepper Travel Variables
long TravelVER;  // Used to store the X value entered in the Serial Monitor
int move_VERfinished = 1; // Used to check if move is completed
long initial_VERhoming = -1; // Used to Home Stepper at startup

long TravelHOZ;  // Used to store the X value entered in the Serial Monitor
int move_HOZfinished = 1; // Used to check if move is completed
long initial_HOZhoming = -1; // Used to Home Stepper at startup




void setup() {
  DcsBios::setup();
  
  pinMode(VERenablePin, OUTPUT);
  digitalWrite(VERenablePin, LOW); // turn stepper ON
  pinMode(VERT_IR, INPUT_PULLUP);

    pinMode(HOZenablePin, OUTPUT);
  digitalWrite(HOZenablePin, LOW); // turn stepper ON
  pinMode(HOZ_IR, INPUT_PULLUP);

  delay(5);  // Wait for EasyDriver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperVER.setMaxSpeed(1000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperVER.setAcceleration(400.0);  // Set Acceleration of Stepper

  stepperHOZ.setMaxSpeed(500.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperHOZ.setAcceleration(200.0);  // Set Acceleration of Stepper
/*
  stepperVER.setCurrentPosition(0);
  stepperVER.moveTo(10); // LEFT
  while (stepperVER.distanceToGo() != 0) {
    stepperVER.run();
    delay(20);
  }
*/
    stepperHOZ.setCurrentPosition(0);
  stepperHOZ.moveTo(-20); // LEFT
  while (stepperHOZ.distanceToGo() != 0) {
    stepperHOZ.run();
    delay(10);
  }

  delay(1000);

/*
  stepperVER.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(VERT_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
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


*/
  stepperHOZ.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(HOZ_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
    stepperHOZ.move(-2);  // Set the position to move to RIGHT
    stepperHOZ.run();  // Start moving the stepper
    delay(5);
  }
  stepperHOZ.setCurrentPosition(0); // SET STEPPER BACK TO 0
  stepperHOZ.setAcceleration(1000.0);  // Set Acceleration of Stepper
  stepperHOZ.moveTo(600); // LEFT
  while (stepperHOZ.distanceToGo() != 0) {
    stepperHOZ.run();
    delay(2);
  }
  digitalWrite(HOZenablePin, HIGH); // turn stepper off for now




}
/*
void onSaiPointerVerChange(unsigned int C_LValue) {
  valC_Line = map(C_LValue, 0, 65535, 1240*2, 100);
  stepperVER.moveTo(valC_Line);
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);
*/
void onSaiPointerHorChange(unsigned int H_LValue) {
  valH_Line = map(H_LValue, 0, 65535, 1240, 100);
  stepperHOZ.moveTo(valH_Line);
}
DcsBios::IntegerBuffer saiPointerHorBuffer(0x756c, 0xffff, 0, onSaiPointerHorChange);




void loop() {

  DcsBios::loop();
/*
  if (stepperVER.distanceToGo() != 0) {
    digitalWrite(VERenablePin, LOW); // turn stepper ON
    stepperVER.run();

  } else {
    digitalWrite(VERenablePin, HIGH); // turn stepper OFF
  }
*/
  if (stepperHOZ.distanceToGo() != 0) {
    digitalWrite(HOZenablePin, LOW); // turn stepper ON
    stepperHOZ.run();

  } else {
    digitalWrite(HOZenablePin, HIGH); // turn stepper OFF
  }

}
