

#include <Servo.h>
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


Servo  SARIServo;

bool FlagChangeFollowupTask = false;
unsigned long FlagChangeServoOff = 0;
int FlagChangeMoveTime = 1000;

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
  stepperVER.setMaxSpeed(6000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperVER.setAcceleration(3000.0);  // Set Acceleration of Stepper
  stepperHOZ.setMaxSpeed(6000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperHOZ.setAcceleration(3000.0);  // Set Acceleration of Stepper
  stepperW_L.setMaxSpeed(6000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperW_L.setAcceleration(3000.0);  // Set Acceleration of Stepper


  //// _______----------------_________VERTICAL - CENTRE LINE SET UP_______----------------_________////
  stepperVER.setCurrentPosition(0);
  stepperVER.moveTo(10); // LEFT
  while (stepperVER.distanceToGo() != 0) {
    stepperVER.run();
    delay(2);
  }
  delay(100);
  stepperVER.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(VER_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
    stepperVER.move(-50);  // Set the position to move to RIGHT
    stepperVER.run();  // Start moving the stepper
    delay(1);
  }
  stepperVER.setCurrentPosition(0); // SET STEPPER BACK TO 0
  stepperVER.setAcceleration(500.0);  // Set Acceleration of Stepper
  stepperVER.moveTo(600); // LEFT
  while (stepperVER.distanceToGo() != 0) {
    stepperVER.run();
    delay(2);
  }
  digitalWrite(VERenablePin, HIGH); // turn stepper off for now
  //// _______----------------_________VERTICAL - CENTRE LINE SET UP_______----------------_________////

  //// _______---------------_________HORIZONTAL - GLIDESLOPE SET UP_______---------------_________////
  stepperHOZ.setCurrentPosition(0);
  stepperHOZ.moveTo(25); // LEFT
  while (stepperHOZ.distanceToGo() != 0) {
    stepperHOZ.run();
    delay(2);
  }
  delay(100);
  stepperHOZ.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(HOZ_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
    stepperHOZ.move(-50);  // Set the position to move to RIGHT
    stepperHOZ.run();  // Start moving the stepper
    delay(1);
  }
  stepperHOZ.setCurrentPosition(0); // SET STEPPER BACK TO 0
  stepperHOZ.setAcceleration(500.0);  // Set Acceleration of Stepper
  stepperHOZ.moveTo(600); // LEFT //200

  while (stepperHOZ.distanceToGo() != 0) {
    stepperHOZ.run();
    delay(2);
  }
  digitalWrite(HOZenablePin, HIGH); // turn stepper off for now
  //// _______---------------_________HORIZONTAL - GLIDESLOPE SET UP_______---------------_________////

  //// _______---------------------_________WATERLINE - SET UP_______---------------------_________////
  stepperW_L.setCurrentPosition(0);
  stepperW_L.moveTo(20); // LEFT
  while (stepperW_L.distanceToGo() != 0) {
    stepperW_L.run();
    delay(2);
  }
  delay(100);
  stepperW_L.setAcceleration(2000.0);  // Set Acceleration of Stepper
  while (digitalRead(W_L_IR ) == HIGH) { // Make the Stepper move CCW until the switch is activated
    stepperW_L.move(-50);  // Set the position to move to RIGHT
    stepperW_L.run();  // Start moving the stepper
    delay(1);
  }
  stepperW_L.setCurrentPosition(0); // SET STEPPER BACK TO 0
  stepperW_L.setAcceleration(500.0);  // Set Acceleration of Stepper
  stepperW_L.moveTo(300); // 2500
  while (stepperW_L.distanceToGo() != 0) {
    stepperW_L.run();
    delay(2);
  }
  digitalWrite(W_LenablePin, HIGH); // turn stepper off for now
  //// _______---------------------_________WATERLINE - SET UP_______---------------------_________////

  // ################ SARI FLAG SERVO WORKING ################

  SARIServo.attach(5);
  SARIServo.write(60); //down
  delay (500);
  SARIServo.write(150); //up
  delay (500);
  SARIServo.detach();
  digitalWrite(5, LOW);
  delay (500);


} // END OF VOID SETUP

//// STEPPERS MOVE CODE FROM DCS BIOS////

void onSaiPointerVerChange(unsigned int C_LValue) {
  if (C_LValue == 0) {
    valC_Line = 600;
  } else {
    valC_Line =  map(C_LValue, 1, 65535, 400, 20);
  }
  stepperVER.moveTo(valC_Line);
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);

void onSaiPointerHorChange(unsigned int H_LValue) {
  if (H_LValue == 65535) {
    valH_Line = 0;
  } else {
    valH_Line =  map(H_LValue, 1, 65534, 600, 150);
  }
  stepperHOZ.moveTo(valH_Line);
}
DcsBios::IntegerBuffer saiPointerHorBuffer(0x756c, 0xffff, 0, onSaiPointerHorChange);

void onSaiManPitchAdjChange(unsigned int W_LValue) {
  valW_Line = map(W_LValue, 0, 65535, 500 , 100);
  stepperW_L.moveTo(valW_Line);
}
DcsBios::IntegerBuffer saiManPitchAdjBuffer(0x74ea, 0xffff, 0, onSaiManPitchAdjChange);


DcsBios::RotaryEncoder saiSet("SAI_SET", "-1600", "+1600", 28, 26);

DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", A12);

DcsBios::Switch2Pos saiCage("SAI_CAGE", A13);

//DcsBios::ServoOutput saiAttWarningFlag(0x74e8, 5, 700, 1800);

void onSaiAttWarningFlagChange(unsigned int newValue) {
  SARIServo.attach(5);
  if (newValue >= 1) {
    SARIServo.write(155);
  }
  else {
    SARIServo.write(60);
  }


  FlagChangeFollowupTask = true;
  FlagChangeServoOff = millis() + FlagChangeMoveTime;
}
DcsBios::IntegerBuffer saiAttWarningFlagBuffer(0x74e8, 0xffff, 0, onSaiAttWarningFlagChange);

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
  if ((FlagChangeFollowupTask == true) && (millis() >= FlagChangeServoOff)) {

    FlagChangeFollowupTask = false;
    SARIServo.detach();
    digitalWrite(5, LOW);
  }

}// END OF VOID LOOP
