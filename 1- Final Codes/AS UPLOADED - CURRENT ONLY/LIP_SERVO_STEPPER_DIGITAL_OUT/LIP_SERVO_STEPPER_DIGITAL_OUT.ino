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
//#define DCSBIOS_IRQ_SERIAL
#define DCSBIOS_DEFAULT_SERIAL
#include "LedControl.h"
#include "DcsBios.h"

#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)

#define  COILRA1  1 // RA = RAD ALT
#define  COILRA2  2
#define  COILRA3  3
#define  COILRA4  4

#define  COILBT1_1  22 // BT_1 = BATTERY 1
#define  COILBT1_2  23
#define  COILBT1_3  24
#define  COILBT1_4  25

#define  COILBT2_1  26 // BT_2 = BATTERY 2
#define  COILBT2_2  27
#define  COILBT2_3  28
#define  COILBT2_4  29

#define  COILHPL_1  30 // HP_1 = HYD PRESSURE L
#define  COILHPL_2  31
#define  COILHPL_3  32
#define  COILHPL_4  33

#define  COILHPR_1  34 // HP_2 = HYD PRESSURE R
#define  COILHPR_2  35
#define  COILHPR_3  36
#define  COILHPR_4  37

#define  COILCA1  38 // CA = CAB ALT
#define  COILCA2  39
#define  COILCA3  40
#define  COILCA4  41

#define  COILBP1  42 // BP = BRAKE PRESSURE
#define  COILBP2  43
#define  COILBP3  44
#define  COILBP4  45

#define  COILCP1  46 // CP = COMPASS
#define  COILCP2  47
#define  COILCP3  48
#define  COILCP4  49




int RAD_ALT = 0;
int valRA = 0;
int posRA = 0;

int CAB_ALT;
int valCA = 0;
int CurCABALT = 0;
int posCA = 0;


int BRAKE_PRESSURE = 0;
int valBP = 0;
int posBP = 0;

int BATT1 = 0;
int valBATT1 = 0;
int posBATT1 = 0;

int BATT2 = 0;
int valBATT2 = 0;
int posBATT2 = 0;

int HYDPL = 0;
int valHYDPL = 0;
int posHYDPL = 0;

int HYDPR = 0;
int valHYDPR = 0;
int posHYDPR = 0;



Stepper stepperRA(STEPS, COILRA1, COILRA2, COILRA3, COILRA4); // RAD ALT

Stepper stepperCA(STEPS, COILCA1, COILCA2, COILCA3, COILCA4); // CAB ALT

Stepper stepperBP(STEPS, COILBP1, COILBP2, COILBP3, COILBP4); // BRAKE PRESSURE
#define BrakePressureZeroPoint 0
#define BrakePressureMaxPoint 150

Stepper stepperBT1(STEPS, COILBT1_1, COILBT1_2, COILBT1_3, COILBT1_4); // BATTERY 1
#define Battery_1_ZeroPoint 0
#define Battery_1_ZeroPoint 150
Stepper stepperBT2(STEPS, COILBT2_1, COILBT2_2, COILBT2_3, COILBT2_4); // BATTERY 2
#define Battery_2_ZeroPoint 0
#define Battery_2_ZeroPoint 150

Stepper stepperHPL(STEPS, COILHPL_1, COILHPL_2, COILHPL_3, COILHPL_4); // HYD PRESSURE L
Stepper stepperHPR(STEPS, COILHPR_1, COILHPR_2, COILHPR_3, COILHPR_4); // HYD PRESSURE R



//###########################################################################################

void onRadaltAltPtrChange(unsigned int newValueRA) {
  RAD_ALT = map(newValueRA, 0, 65000, 720, 10);
}
DcsBios::IntegerBuffer radaltAltPtrBuffer(0x751a, 0xffff, 0, onRadaltAltPtrChange);

void onPressureAltChange(unsigned int newValueCA) {
  CAB_ALT = map(newValueCA, 0, 65000, 45, 720);
}
DcsBios::IntegerBuffer pressureAltBuffer(0x7514, 0xffff, 0, onPressureAltChange);


void onHydIndBrakeChange(unsigned int newValueBP) {
  BRAKE_PRESSURE = map(newValueBP, 0, 65000, BrakePressureZeroPoint, BrakePressureMaxPoint);
}
DcsBios::IntegerBuffer hydIndBrakeBuffer(0x7506, 0xffff, 0, onHydIndBrakeChange);

void onHydIndLeftChange(unsigned int newValueHL) {
HYDPL = map(newValueHL, 0, 65000, 720, 10);
}
DcsBios::IntegerBuffer hydIndLeftBuffer(0x751e, 0xffff, 0, onHydIndLeftChange);

void onHydIndRightChange(unsigned int newValueHR) {
  HYDPR = map(newValueHR, 0, 65000, 720, 10);
}
DcsBios::IntegerBuffer hydIndRightBuffer(0x7520, 0xffff, 0, onHydIndRightChange);


Servo RAD_ALT_servo;
#define RadarAltServoPin 5
DcsBios::ServoOutput radaltOffFlag(0x751c, RadarAltServoPin, RAD_ALT_servo_Hidden_Pos, RAD_ALT_servo_Off_Pos);
//###########################################################################################

void onConsoleIntLtChange(unsigned int newValue) {
  int ConsolesDimmerValue = 0;

  ConsolesDimmerValue = map(newValue, 0, 65000, 0, 100);

}

DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);



void onWarnCautionDimmerChange(unsigned int newValue) {
  int WarnCautionDimmerValue = 0;

  WarnCautionDimmerValue = map(newValue, 0, 65000, 0, 255);

}
DcsBios::IntegerBuffer warnCautionDimmerBuffer(0x754c, 0xffff, 0, onWarnCautionDimmerChange);



void setup() {
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);

  RAD_ALT_servo.attach(RadarAltServoPin);
  RAD_ALT_servo.writeMicroseconds(RAD_ALT_servo_Off_Pos);  // set servo to "Off Point"
  delay(1000);
  RAD_ALT_servo.writeMicroseconds(RAD_ALT_servo_Hidden_Pos);  // set servo to min
  delay(500);
  RAD_ALT_servo.detach();


  //###########################################################################################
  /// RADAR ALT WORKING ======> SET RADAR ALT STEPPER TO 0 FEET
  stepperRA.setSpeed(50);
  stepperRA.step(720);       //Reset FULL ON Position
  stepperRA.step(-720);       //Reset FULL OFF Position
  posRA = 0;
  RAD_ALT = map(0, 0, 65000, 720, 10);
  /// RADAR ALT WORKING ======< SET RADAR ALT STEPPER TO 0 FEET

  /// CABIN ALT WORKING ======> SET CABIN ALT STEPPER TO 0 FEET
  stepperCA.setSpeed(60);
  stepperCA.step(700);       //Reset FULL ON Position
  stepperCA.step(-720);       //Reset FULL OFF Position
  stepperCA.step(30);       //Reset FULL OFF Position
  posCA = 0;
  CAB_ALT = map(0, 0, 65000, 40, 720);
  /// CABIN ALT WORKING ======< SET CABIN ALT STEPPER TO 0 FEET



  // BRAKE PRESSURE

  stepperBP.setSpeed(60);
  stepperBP.step(BrakePressureMaxPoint);       //Reset FULL ON Position
  delay(1000);
  stepperBP.step(-(BrakePressureMaxPoint + 100) );       //Reset FULL OFF Position
  posBP = 0;
  BRAKE_PRESSURE = map(0, 0, 65000, BrakePressureZeroPoint, BrakePressureMaxPoint);
  /// BRAKE PRESSURE



  DcsBios::setup();

}


void loop() {

  // **************************** Handle Steppers

  valRA = RAD_ALT;
  if (abs(valRA - posRA) > 2) {      //if diference is greater than 2 steps.
    if ((valRA - posRA) > 0) {
      stepperRA.step(-1);      // move one step to the left.
      posRA++;
    }
    if ((valRA - posRA) < 0) {
      stepperRA.step(1);       // move one step to the right.
      posRA--;
    }
  }

  /// RADAR ALT LOOP WORKING ======<
  //###########################################################################################
  {
    valCA = CAB_ALT;
    //
    if (abs(valCA - posCA) > 2) {      //if diference is greater than 2 steps.
      if ((valCA - posCA) > 0) {
        stepperCA.step(1);      // move one step to the left.
        posCA++;
      }
      if ((valCA - posCA) < 0) {
        stepperCA.step(-1);       // move one step to the right.
        posCA--;
      }
      /// CABIN ALT LOOP WORKING ======<
      //###########################################################################################
    }

  }


  /// BRAKE PRESSURE
  //###########################################################################################
  {
    valBP = BRAKE_PRESSURE;
    //
    if (abs(valBP - posBP) > 2) {      //if diference is greater than 2 steps.
      if ((valBP - posBP) > 0) {
        stepperBP.step(1);      // move one step to the left.
        posBP++;
      }
      if ((valBP - posBP) < 0) {
        stepperBP.step(-1);       // move one step to the right.
        posBP--;
      }
    }

  }



  DcsBios::loop();
}
