

//#include <Nextion.h>          // Nextion Library
//#include <SoftwareSerial1.h>   // Library for Software Serial Port - not used, program uses Pins 0 and 1 (RX/TX)
//#include <Keyboard.h>         // Keyboard Emulation
//#include "NexTouch.h"
#include <Mouse.h>
//#include <HID.h>
//-----------------------VOLUME***************************************VOLUME------------------------------------
//#include <HID-Project.h>                    //include HID_Project library
//#include <HID-Settings.h>
//-----------------------VOLUME***************************************VOLUME------------------------------------

int horzPin = A0;  // Analog output of horizontal joystick pin
int vertPin = A1;  // Analog output of vertical joystick pin#define WaitOneHour 3600000

int selPin = 6;  // select button pin of joystick

int vertZero, horzZero;  // Stores the initial value of each axis, usually around 512
int vertValue, horzValue;  // Stores current analog output of each axis
const int sensitivity = 100;  // Higher sensitivity value = slower mouse, should be <= about 500
int mouseClickFlag = 0;

//int invertMouse = 1;        //Invert joystick based on orientation
int invertMouse = -1;         //Noninverted joystick based on orientation



// Main Setup
void setup() {
}
// Main Loop
void loop() {
  
  //------------------------MOUSE***************************************MOUSE-------------------------------------
   vertValue = analogRead(vertPin) - vertZero;  // read vertical offset
   horzValue = analogRead(horzPin) - horzZero;  // read horizontal offset

  if (vertValue != 0)
    Mouse.move(0, (invertMouse * (vertValue / sensitivity)), 0); // move mouse on y axis
  if (horzValue != 0)
    Mouse.move((invertMouse * (horzValue / sensitivity)), 0, 0); // move mouse on x axis
  if ((digitalRead(selPin) == 0) && (!mouseClickFlag))  // if the joystick button is pressed
  {
    mouseClickFlag = 1;
    Mouse.press(MOUSE_LEFT);  // click the left button down
  }
  else if ((digitalRead(selPin)) && (mouseClickFlag)) // if the joystick button is not pressed
  {
    mouseClickFlag = 0;
    Mouse.release(MOUSE_LEFT);  // release the left button
  }
  //------------------------MOUSE***************************************MOUSE-------------------------------------
 


}
