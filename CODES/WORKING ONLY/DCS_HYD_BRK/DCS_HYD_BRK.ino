/* #####################################################
  DCS BIOS CODE for OPEN HORNET HYD BRAKE GAUGE
  ######################################################*/


#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)
#define  COIL1a 38
#define  COIL2a 40
#define  COIL3a 39
#define  COIL4a 41

#define  COIL1b 42
#define  COIL2b 44
#define  COIL3b 43
#define  COIL4b 45
int IRSensor = 0;

int HYD_BRK = 0;
int val = 0;

Stepper stepperA(STEPS, COIL1a, COIL2a, COIL3a, COIL4a);
Stepper stepperB(STEPS, COIL1b, COIL2b, COIL3b, COIL4b);


void setup() {
  Serial.begin(9600);
  Serial.print("ONLINE");
  //pinMode (IRSensor, INPUT);
  stepperA.setSpeed(50);
  stepperA.step(-600);       //Reset Position(250 steps counter-clockwise (just over the Max travel).
  stepperA.step(600);
  stepperA.step(-20);
    stepperB.setSpeed(50);
  stepperB.step(-600);       //Reset Position(250 steps counter-clockwise (just over the Max travel).
  stepperB.step(600);
  stepperB.step(-20);
}
void loop() {
 /*
  * stepperA.setSpeed(50);
  stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
   delay(100);
    stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
  delay(100);

 stepperA.setSpeed(50);
  stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
   delay(100);
    stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
  delay(100);



   stepperA.setSpeed(50);
  stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
   delay(100);
    stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
  delay(100);


   stepperA.setSpeed(50);
  stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
   delay(100);
    stepperA.step(-2000); 
  delay(100);
  stepperA.step(2000);
  delay(100);


   stepperB.setSpeed(50);
  stepperB.step(-2000); 
  delay(100);
  stepperB.step(2000);
   delay(100);
    stepperB.step(-2000); 
  delay(100);
  stepperB.step(2000);
  delay(100);
  */

}
