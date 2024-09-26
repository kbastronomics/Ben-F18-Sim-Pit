#include <AccelStepper.h>
#define EN_switchH 43 // HOZ
// Define some steppers and the pins the will use
AccelStepper stepper1(1, 35, 33);   // X=1 = Easy Driver interface Y=STEP - Z=DIR
AccelStepper stepper2(1, 41, 39);   // X=1 = Easy Driver interface Y=STEP - Z=DIR
AccelStepper stepper3(1, 29, 31);   // X=1 = Easy Driver interface Y=STEP - Z=DIR

void setup()
{
  pinMode(EN_switchH, OUTPUT);
stepper1.setMaxSpeed(800.0);
stepper1.setAcceleration(200.0);
stepper1.moveTo(10000);
stepper1.enableOutputs();

stepper2.setMaxSpeed(800.0);
stepper2.setAcceleration(200.0);
stepper2.moveTo(10000);
stepper2.enableOutputs();

stepper3.setMaxSpeed(800.0);
stepper3.setAcceleration(200.0);
stepper3.moveTo(10000);
stepper3.enableOutputs();


}

void loop()
{

 digitalWrite(EN_switchH, LOW);
// Change direction at the limits
if (stepper1.distanceToGo() == 0)
stepper1.moveTo(-stepper1.currentPosition());
stepper1.run();

if (stepper2.distanceToGo() == 0)
stepper2.moveTo(-stepper2.currentPosition());
stepper2.run();

if (stepper3.distanceToGo() == 0)
stepper3.moveTo(-stepper3.currentPosition());
stepper3.run();

}
