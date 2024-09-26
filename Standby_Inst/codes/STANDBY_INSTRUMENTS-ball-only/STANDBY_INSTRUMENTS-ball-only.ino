#include <Servo.h>
#include <AccelStepper.h>

#include <FastLED.h>
#include "LedControl.h"
#include <Arduino.h>
#define NUM_LEDS_CHA 10
#define DATA_PIN_CHA 0
#define LED_TYPE    WS2812B
CRGB leds_A[NUM_LEDS_CHA];
//#define DCSBIOS_IRQ_SERIAL
#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

// Define the Pins used

#define Check_LED_R 8
#define Check_LED_G 9



Servo  SARIServo;

int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;

int SARI_Flag = 1;
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
        stepper.setSpeed(800); //400
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
          stepper.runToNewPosition(stepper.currentPosition() );
          // tell the AccelStepper library that we are at position zero
          stepper.setCurrentPosition(800);
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

Nema8Stepper SARIRoll(0x74e6,               // address of stepper data
                      SARIstepperRoll,      // name of AccelStepper instance
                      SARIstepperConfig,    // SARIStepperConfig struct instance
                      23,                   // IR Detector Pin (must be LOW in zero position)
                      800,                  // zero offset (SET TO 50% of MaX Steps)
                      // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
                      // SARI will be upsidedown after Setup, this will correct in Bios
[](unsigned int newValue) -> unsigned int {
  newValue = newValue & 0xffff;
  return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);

});



DcsBios::RotaryEncoder saiSet("SAI_SET", "-800", "+800", 28, 26);

DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", A12);
DcsBios::Switch2Pos saiCage("SAI_CAGE", A13);

//--------------------------------------------------------------------------------SETUP--------------------------------------------------------------------------------\\

void setup() {
  DcsBios::setup();

  delay(10);
  FastLED.addLeds<WS2812B, DATA_PIN_CHA, RGB>(leds_A, NUM_LEDS_CHA);
  fill_solid(leds_A, NUM_LEDS_CHA, CRGB::Red); // Set all Console LEDs to ##GREEN##.
  FastLED.setBrightness(250);
  FastLED.show();
  delay(10);

  pinMode(SARI_Flag_Pin, OUTPUT);

  pinMode(Check_LED_G, OUTPUT);
  pinMode(Check_LED_R, OUTPUT);
  pinMode(SARIenablePin, OUTPUT);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // WATER START Homing procedure of Stepper Motor at startup

  

}

void loop() {


  DcsBios::loop();


}
