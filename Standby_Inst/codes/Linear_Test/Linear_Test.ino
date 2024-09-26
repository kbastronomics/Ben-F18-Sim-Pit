
//22, 23, 24, 25
//IR SENSORS SARI

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

#include <AccelStepper.h>

const int C_LinestepPin = 35;
const int C_LinedirectionPin = 33;
const int C_LineenablePin = 37;

#define C_Line_ZERO_SENSE_PIN 25
#define C_Line_OFFSET_TO_ZERO_POINT 300 // Ben to hone


int STANDBY_C_Line = 0;
int LastSTANDBY_C_Line = 0;
unsigned int valC_Line = 0;
unsigned int posC_Line = 0;

AccelStepper stepperSTANDBY_C_Line(1, C_LinestepPin, C_LinedirectionPin);

void setup() {

  DcsBios::setup();

  pinMode(C_Line_ZERO_SENSE_PIN,  INPUT_PULLUP);
  stepperSTANDBY_C_Line.setSpeed(60);
  stepperSTANDBY_C_Line.move(1000);

  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_C_Line.move(1);
    if (digitalRead(C_Line_ZERO_SENSE_PIN) == false) {
      stepperSTANDBY_C_Line.move(C_Line_OFFSET_TO_ZERO_POINT);
      posC_Line = 0;
      break;
    }
  }
}
void onSaiPointerVerChange(unsigned int C_LValue) {
  valC_Line = map(C_LValue, 0, 65535, 0, 660);
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);

void loop() {
  DcsBios::loop();

  // Centre Line (Vertical)
  //###########################################################################################

  if (valC_Line > posC_Line) {
    stepperSTANDBY_C_Line.move(1);      // move one step to the left.
    posC_Line++;
  }
  else if (valC_Line < posC_Line) {
    stepperSTANDBY_C_Line.move(-1);       // move one step to the right.
    posC_Line--;
  }
  //###########################################################################################

}
