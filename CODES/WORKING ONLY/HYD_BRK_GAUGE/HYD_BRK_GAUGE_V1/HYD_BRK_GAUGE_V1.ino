/* #####################################################
DCS BIOS CODE for OPEN HORNET HYD BRAKE GAUGE
######################################################*/

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"
#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)
#define  COIL1  8
#define  COIL2  9
#define  COIL3  10
#define  COIL4  11

int HYD_BRK = 0;
int val = 0;

Stepper stepper(STEPS, COIL1, COIL2, COIL3, COIL4);

void onHydIndBrakeChange(unsigned int newValue) {
HYD_BRK = map(newValue, 0, 65000, 0, 1000);
}
DcsBios::IntegerBuffer hydIndBrakeBuffer(0x7506, 0xffff, 0, onHydIndBrakeChange);


void setup(){
  stepper.setSpeed(30);    // set the motor speed to 30 RPM (360 PPS aprox.).
  stepper.step(250);       //Reset Position(250 steps counter-clockwise (just over the Max travel). 
  stepper.step(-17);       //Reset Position(177 steps to the 0 Point clockwise). 
  DcsBios::setup();
}
int pos=0;                 //Position in steps(0-150)

void loop(){
  val = HYD_BRK;   //get the potentiometer value (range 0-1023)
  val = map(HYD_BRK,0,100,0,150);    // map Steper Needle 0-4. 0=0 - 4=150
  if(abs(val - pos)> 2){         //if diference is greater than 2 steps.
      if((val - pos)> 0){
          stepper.step(-1);      // move one step to the left.
          pos++;
          }
      if((val - pos)< 0){
          stepper.step(1);       // move one step to the right.
          pos--;
          }
      }
  DcsBios::loop();
}
