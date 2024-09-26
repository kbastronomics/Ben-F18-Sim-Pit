#include <Servo.h>
#include <AccelStepper.h>

//#define DCSBIOS_IRQ_SERIAL
#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

// Define the Pins used

#define Check_LED_R 8
#define Check_LED_G 9

#define HOZ_IR_sen 24 // HOZ
#define VER_IR_sen 25 // VRT
#define WTR_IR_sen 22 // WTR

#define EN_switchV 37 // VER
#define EN_switchH 43 // HOZ
#define EN_switchW 27 // WTR

Servo  SARIServo;
//SARIServo.attach(7);

//HOZ
AccelStepper stepperH(1, 41, 39);   // 1 = Easy Driver interface
//VRT
AccelStepper stepperV(1, 35, 33);   // 1 = Easy Driver interface
//WTR
AccelStepper stepperW(1, 29, 31);   // 1 = Easy Driver interface
long valWAT = 0;
long posWAT = 0;

long valHOZ = 0;
long posHOZ = 0;

long valVER = 0;
long posVER = 0;


// Stepper Travel Variables

long initial_homingH = 0; // Used to Home Stepper at startup
long initial_homingV = 0; // Used to Home Stepper at startup
long initial_homingW = 0; // Used to Home Stepper at startup

int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;

int SARI_Flag = 0;
#define SARI_Flag_Pin 5 // SARI FLAG PIN

long dcsMillis;

// SARI

struct SARIStepperConfig {
  unsigned int SARImaxSteps;
  unsigned int SARIacceleration;
  unsigned int maxSpeed;
};

void onUpdateCounterChange(unsigned int newValue) {
  DCS_State = newValue;
}
DcsBios::IntegerBuffer UpdateCounterBuffer(0xfffe, 0x00ff, 0, onUpdateCounterChange);

//  OFF FLAG
//DcsBios::LED saiAttWarnFlagL(0x74d6, 0x0100, 5);

void onSaiAttWarnFlagLChange(unsigned int newValue) {
  SARI_Flag = newValue;
}
DcsBios::IntegerBuffer saiAttWarnFlagLBuffer(0x74d6, 0x0100, 8, onSaiAttWarnFlagLChange);

//////SARI - TEST - BEN --------------------------------------------------------------------------------------------------------------
//----------ROLL SERVO----------
DcsBios::ServoOutput saiPitch(0x74e4, 7, 2400, 544);
//Servo ROLL_servo;
//----------ROLL SERVO----------

const long zeroTimeout = 50000;
const int SARIenablePin = 49;


class Nema8Stepper : public DcsBios::Int16Buffer {
  private:

    AccelStepper& stepper;
    SARIStepperConfig& SARIstepperConfig;
    inline bool SARIzeroDetected() {
      return digitalRead(SARIirDetectorPin) == 0;
    }
    unsigned int (*map_function)(unsigned int);
    unsigned char initState;
    long SARIcurrentStepperPosition;
    long SARIlastAccelStepperPosition;
    unsigned char SARIirDetectorPin;
    long SARIzeroOffset;
    bool SARImovingForward;
    bool SARIlastZeroDetectState;

    long SARIzeroPosSearchStartTime = 0;

    long SARInormalizeStepperPosition(long pos) {
      if (pos < 0) return pos + SARIstepperConfig.SARImaxSteps;
      if (pos >= SARIstepperConfig.SARImaxSteps) return pos - SARIstepperConfig.SARImaxSteps;
      return pos;
    }

    void updateSARIcurrentStepperPosition() {
      // adjust SARIcurrentStepperPosition to include the distance our stepper motor
      // was moved since we last updated it
      long SARImovementSinceLastUpdate = stepper.currentPosition() - SARIlastAccelStepperPosition;
      SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIcurrentStepperPosition + SARImovementSinceLastUpdate);
      SARIlastAccelStepperPosition = stepper.currentPosition();
    }
  public:
    Nema8Stepper(unsigned int address, AccelStepper& stepper, SARIStepperConfig& SARIstepperConfig, unsigned char SARIirDetectorPin, long SARIzeroOffset, unsigned int (*map_function)(unsigned int))
      : Int16Buffer(address), stepper(stepper), SARIstepperConfig(SARIstepperConfig), SARIirDetectorPin(SARIirDetectorPin), SARIzeroOffset(SARIzeroOffset), map_function(map_function), initState(0), SARIcurrentStepperPosition(0), SARIlastAccelStepperPosition(0) {
    }

    virtual void loop() {
      if (initState == 0) { // not initialized yet
        pinMode(SARIirDetectorPin, INPUT);
        stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
        stepper.setSpeed(600); //400
        digitalWrite(SARIenablePin, LOW);
        initState = 1;
        SARIzeroPosSearchStartTime = millis();
        digitalWrite(SARIenablePin, HIGH);
      }

      if (initState == 1) {
        // move off zero if already there so we always get movement on reset
        // (to verify that the stepper is working)
        if (SARIzeroDetected()) {
          digitalWrite(SARIenablePin, LOW);
          stepper.runSpeed();
        } else {
          initState = 2;
          digitalWrite(SARIenablePin, HIGH);
        }
      }

      if (initState == 2) { // zeroing
        if (!SARIzeroDetected()) {

          if (millis() >= (zeroTimeout + SARIzeroPosSearchStartTime)) {
            digitalWrite(SARIenablePin, HIGH);
            initState == 99;
          }
          else
            digitalWrite(SARIenablePin, LOW);
          stepper.runSpeed();

        } else {
          stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
          stepper.runToNewPosition(stepper.currentPosition());
          // tell the AccelStepper library that we are at position zero
          stepper.setCurrentPosition(SARIzeroOffset);
          SARIlastAccelStepperPosition = 0;
          // set stepper SARIacceleration in steps per second per second
          // (default is zero)
          stepper.setAcceleration(SARIstepperConfig.SARIacceleration);

          SARIlastZeroDetectState = true;
          initState = 3;
          digitalWrite(SARIenablePin, HIGH);
        }
      }
      //    digitalWrite(enablePin, HIGH);
      if (initState == 3) { // running normally

        // recalibrate when passing through zero position
        bool SARIcurrentZeroDetectState = SARIzeroDetected();
        if (!SARIlastZeroDetectState && SARIcurrentZeroDetectState && SARImovingForward) {
          // we have moved from left to right into the 'zero detect window'
          // and are now at position 0
          SARIlastAccelStepperPosition = stepper.currentPosition();
          SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIzeroOffset);
        } else if (SARIlastZeroDetectState && !SARIcurrentZeroDetectState && !SARImovingForward) {
          // we have moved from right to left out of the 'zero detect window'
          // and are now at position (SARImaxSteps-1)
          SARIlastAccelStepperPosition = stepper.currentPosition();
          SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIstepperConfig.SARImaxSteps + SARIzeroOffset);
        }
        SARIlastZeroDetectState = SARIcurrentZeroDetectState;


        if (hasUpdatedData()) {
          // convert data from DCS to a target position expressed as a number of steps
          long targetPosition = (long)map_function(getData());

          updateSARIcurrentStepperPosition();

          long delta = targetPosition - SARIcurrentStepperPosition;

          // if we would move more than 180 degree counterclockwise, move clockwise instead

          if (delta < -((long)(SARIstepperConfig.SARImaxSteps / 2))) delta += SARIstepperConfig.SARImaxSteps; //2
          // if we would move more than 180 degree clockwise, move counterclockwise instead
          if (delta > (SARIstepperConfig.SARImaxSteps / 2)) delta -= (long)SARIstepperConfig.SARImaxSteps; //2

          SARImovingForward = (delta >= 0);

          // Turn off Stepper when not in use
          if (delta != 0) {
            digitalWrite(SARIenablePin, LOW);
          } // LOW  = stepper ON drive current available
          else {
            digitalWrite(SARIenablePin, HIGH);
          } // HIGH  = stepper OFF no drive current

          // tell AccelStepper to move relative to the current position
          stepper.move(delta);

        }
        stepper.run();

      }
      if (initState == 99) { // Timed out looking for zero do nothing
        digitalWrite(SARIenablePin, HIGH);
      }
    }

};

struct SARIStepperConfig SARIstepperConfig = {
  1600,  // SARImaxSteps //200STEPS X 1/8 MICRO STEPPING
  3600, // maxSpeed //3200
  3200 // SARIacceleration
};
const int SARIstepPin = 47;
const int SARIdirectionPin = 45;

// define AccelStepper instance

AccelStepper SARIstepperRoll(AccelStepper::DRIVER, SARIstepPin, SARIdirectionPin);

Nema8Stepper SARIRoll(0x74e6,                     // address of stepper data
                      SARIstepperRoll,      // name of AccelStepper instance
                      SARIstepperConfig,    // SARIStepperConfig struct instance
                      23,                   // IR Detector Pin (must be LOW in zero position)
                      800,                  // zero offset (SET TO 50% of MaX Steps) 1650
                      // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
                      // SARI will be upsidedown after Setup, this will correct in Bios
[](unsigned int newValue) -> unsigned int {


  newValue = newValue & 0xffff;
  return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);

});




void onSaiManPitchAdjChange(unsigned int newValue) {
  valWAT = map(newValue, 0, 65535, 0, 1500);
  stepperW.moveTo(valWAT);
}
DcsBios::IntegerBuffer saiManPitchAdjBuffer(0x74ea, 0xffff, 0, onSaiManPitchAdjChange);

void onSaiPointerHorChange(unsigned int newValue) {
  if (newValue == 65535) {
    valHOZ = 0;
    stepperH.moveTo(valHOZ);
  } else {
    valHOZ = map(newValue, 1, 65494, 3600, 1600);
    stepperH.moveTo(valHOZ);
  }
}
DcsBios::IntegerBuffer saiPointerHorBuffer(0x756c, 0xffff, 0, onSaiPointerHorChange);

void onSaiPointerVerChange(unsigned int newValue) {
  if (newValue == 0) {
    valVER = 0;
    stepperV.moveTo(valVER);
  } else {
    valVER = map(newValue, 1, 65535, 1500, 3600);
    stepperV.moveTo(valVER);
  }
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);

DcsBios::RotaryEncoder saiSet("SAI_SET", "-800", "+800", 28, 26);

DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", A12);
DcsBios::Switch2Pos saiCage("SAI_CAGE", A13);

//--------------------------------------------------------------------------------SETUP--------------------------------------------------------------------------------\\

void setup() {

   pinMode(SARI_Flag_Pin, OUTPUT);
  DcsBios::setup();
 

  pinMode(Check_LED_G, OUTPUT);
  pinMode(Check_LED_R, OUTPUT);
  pinMode(SARIenablePin, OUTPUT);

  digitalWrite(EN_switchW, LOW);

  pinMode(HOZ_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchH, OUTPUT);

  pinMode(VER_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchV, OUTPUT);

  pinMode(WTR_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchW, OUTPUT);
  delay(5);  // Wait for EasyDriver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperH.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperH.setAcceleration(2000.0);  // Set Acceleration of Stepper

  stepperV.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperV.setAcceleration(2000.0);  // Set Acceleration of Stepper

  stepperW.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperW.setAcceleration(2000.0);  // Set Acceleration of Stepper
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // WATER START Homing procedure of Stepper Motor at startup

  Serial.print("Water Stepper is Homing . . . ");
  digitalWrite(EN_switchW, LOW);
  if (digitalRead(WTR_IR_sen) == LOW)
    Serial.println("Water Stepper is IN the SENSOR . . . ");
  if (digitalRead(WTR_IR_sen) == HIGH)
    Serial.println("Water Stepper is OUT of the SENSOR  . . . ");
  while (digitalRead(WTR_IR_sen)) {  // Make the Stepper move CCW until the switch is activated
    stepperW.moveTo(initial_homingW);  // Set the position to move to
    initial_homingW++;  // Decrease by 1 for next move if needed
    stepperW.run();  // Start moving the stepper
    delay(1);
  }
  initial_homingW = 1;

  while (!digitalRead(WTR_IR_sen)) { // Make the Stepper move CW until the switch is deactivated
    stepperW.moveTo(initial_homingW);
    stepperW.run();
    initial_homingW--;
    delay(1);
  }

  stepperW.setCurrentPosition(600);//400
  Serial.println("Water Homing Completed");
  Serial.println("");
  stepperW.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperW.setAcceleration(2000.0);  // Set Acceleration of Stepper
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // HOR Start Homing procedure of Stepper Motor at startup

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
  initial_homingH = 1;

  while (!digitalRead(HOZ_IR_sen)) { // Make the Stepper move CW until the switch is deactivated
    stepperH.moveTo(initial_homingH);
    stepperH.run();
    initial_homingH++;
    delay(1);
  }

  stepperH.setCurrentPosition(800);
  Serial.println("Homing Completed");
  Serial.println("");
  stepperH.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperH.setAcceleration(2000.0);  // Set Acceleration of Stepper
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
  initial_homingV = 1;

  while (!digitalRead(VER_IR_sen)) { // Make the Stepper move CW until the switch is deactivated
    stepperV.moveTo(initial_homingV);
    stepperV.run();
    initial_homingV--;
    delay(1);
  }

  stepperV.setCurrentPosition(3200); // 0 is full LEFT <-> HIGHER IS RIGHT
  Serial.println("VER Homing Completed");
  Serial.println("");
  stepperV.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperV.setAcceleration(2000.0);  // Set Acceleration of Stepper
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  Serial.println("ALL CALIBRATION COMPLETE");

  stepperW.moveTo(1000);//  --- HIGHER IS UP    <->   LOWER IS DOWN---
  stepperW.run();

  stepperV.moveTo(0); //    --- HIGHER IS RIGHT  <->   LOWER IS LEFT ---
  stepperV.run();

  stepperH.moveTo(0); //    --- HIGHER IS DOWN   <->   LOWWER IS UP ---
  stepperH.run();


}



void loop() {


  DcsBios::loop();

  //###########################################################################################
  // SARI_ROLL SET IN CODE ABOVE
  //###########################################################################################


  if (stepperW.distanceToGo() != 0) {
    digitalWrite(EN_switchW, LOW); // turn stepper ON
    stepperW.run();
  } else {
    digitalWrite(EN_switchW, HIGH); // turn stepper OFF
  }

  if (stepperH.distanceToGo() != 0) {
    digitalWrite(EN_switchH, LOW); // turn stepper ON
    stepperH.run();
  } else {
    digitalWrite(EN_switchH, HIGH); // turn stepper OFF
  }

  if (stepperV.distanceToGo() != 0) {
    digitalWrite(EN_switchV, LOW); // turn stepper ON
    stepperV.run();
  } else {
    digitalWrite(EN_switchV, HIGH); // turn stepper OFF
  }


  //******************************************************************************************************************************************
  //         STEPPER SAFETY CHECK CODE, WILL NOT TUNR STEPPERS ON IF DCS BIOS NOT RUNNING, OR IF GAME PAUSED AFTER 1 SECOND   \\

  if (DCS_State != previous_DCS_State)
  {
    //restart the TIMER
    dcsMillis = millis();
    digitalWrite(Check_LED_R, HIGH);              // RED LED ON PCB TO SHOW ENABLE SWITCHES ARE ACTIVE AND POWER IS ON TO THE STEPPERS IF REQUIRED - SET BY CODE
    digitalWrite(Check_LED_G, LOW);
    previous_DCS_State = DCS_State;
  }


  if (millis() - dcsMillis >= (1000) )            // 1 SECOND DELAY BEFORE POWER OFF STEPPERS
  {
    digitalWrite(EN_switchV, HIGH);
    digitalWrite(EN_switchH, HIGH);
    digitalWrite(EN_switchW, HIGH);
    digitalWrite(SARIenablePin, HIGH);
    digitalWrite(Check_LED_G, HIGH);              // GREEN LED ON PCB TO SHOW **ALL** ENABLE SWITCHES ARE SAFE - OFF
    digitalWrite(Check_LED_R, LOW);
  }

  //         STEPPER SAFETY CHECK CODE, WILL NOT TUNR STEPPERS ON IF DCS BIOS NOT RUNNING, OR IF GAME PAUSED AFTER 1 SECOND   \\
  //******************************************************************************************************************************************

  if (SARI_Flag == 1)
  {
    digitalWrite(SARI_Flag_Pin, LOW);
  } else {
    digitalWrite(SARI_Flag_Pin, HIGH);
  }
}
