// UPDATED TO DCS-BIOS FP EDITION FOR OPEN HORNET


////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = GUAGE CONTROLLER                       ||\\
//||              LOCATION IN THE PIT = LIP LEFT SIDE                ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega                 ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN -       ||\\
//||            PROGRAM PORT CONNECTED COM PORT = COM XX              ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

#include <Servo.h>
#define DCSBIOS_IRQ_SERIAL

#include "LedControl.h"
#include "DcsBios.h"

#include <Servo.h>
#define DCSBIOS_IRQ_SERIAL
//#define DCSBIOS_DEFAULT_SERIAL
#include "LedControl.h"
#include "DcsBios.h"

#define DIMMER 11
#define RAD_GN 9
#define RAD_RD 8



Servo RAD_ALT_servo;
#define RadarAltServoPin 7
#define RAD_ALT_servo_Off_Pos 90 // OFF FLAG IN WINDOW
#define RAD_ALT_servo_Hidden_Pos 50 // OFF FLAG NOT SHOWN

#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)

#define  COILRA1  2 // RA = RAD ALT
#define  COILRA2 3
#define  COILRA3  5
#define  COILRA4  6

int RAD_ALT = 0;
int valRA = 0;
int posRA = 0;


int WarnCautionDimmerValue = 0;
Stepper stepperRA(STEPS, COILRA1, COILRA2, COILRA3, COILRA4); // RAD ALT


int MaxDCSBios = 65535;
int MinDCSBios = 0;
int RAStepVal = 2620;
int RAStepSteps = 144;
int StepSteps = 720;
//###########################################################################################

void onRadaltAltPtrChange(unsigned int newValue) { //2620
  RAD_ALT = map(newValue, 65355, 23, 0, 720);
}
DcsBios::IntegerBuffer radaltAltPtrBuffer(0x751a, 0xffff, 0, onRadaltAltPtrChange);

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  RAD_ALT_servo.attach(7);
  RAD_ALT_servo.write(35);
    delay(250);
  RAD_ALT_servo.write(100);
    delay(250);
  
  RAD_ALT_servo.detach();
  //analogWrite(5, LOW);

  //###########################################################################################
  /// RADAR ALT WORKING ======> SET RADAR ALT STEPPER TO 0 FEET
  stepperRA.setSpeed(40);
  stepperRA.step(-1000);       //Reset FULL ON Position
  stepperRA.step(720);       //Reset FULL OFF Position
  //RAD_ALT = map(0, 65535, 0, 720, 0);   //RAD_ALT = map(newValueRA, 0, 65500, 720, 0);
  delay (2000);
  int RAD_ALT = 0;
  int valRA = 0;
  //int posRA = 0;

  DcsBios::setup();
  delay (2000);
  digitalWrite(DIMMER, HIGH);
  digitalWrite(RAD_GN, HIGH);
  digitalWrite(RAD_RD, HIGH);
  // RAD_ALT_servo.detach();
}


void loop() {

  // **************************** Handle Steppers

  valRA = RAD_ALT;
  if (abs(valRA - posRA) > 2) {      //if diference is greater than 2 steps.
    if ((valRA - posRA) > 0) {
      stepperRA.step(1);      // move one step to the left.
      posRA++;
    }
    if ((valRA - posRA) < 0) {
      stepperRA.step(-1);       // move one step to the right.
      posRA--;
    }
  }

  /// RADAR ALT LOOP WORKING ======<


  DcsBios::loop();
}
