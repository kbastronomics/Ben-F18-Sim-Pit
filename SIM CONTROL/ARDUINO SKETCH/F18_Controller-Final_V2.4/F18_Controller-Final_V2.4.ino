/*
   NOTES -
   Thanks for downloading
   The Sketch below required the following library
   you must delete any of the current "Nextion" Library's already installed
   This will not compile or work without this Library
   "ITEADLIB_Arduino_Nextion-master - Version 0.9.0"
   the library is located in the same folder you got this sketch from

   Current #define DEBUG_SERIAL_ENABLE
   Change //#define DEBUG_SERIAL_ENABLE ("//" add to the begining, remark this line out)

   Current #define dbSerial Serial
   Change //#define dbSerial Serial ("//" add to the begining, remark this line out)

   Current #define nexSerial Serial //
   Change #define nexSerial Serial1 // (Add "1" to Serial)

   if you download the file from ITEAD" you will need to make the below 3 changes,
   these have been made in the library located with this Sketch already
   In NexConfig.h

   This file currently works with Version of the Nextion file

   "SIM_CONTROL_V2.1.HMI"

   Arduino need to be an ARDUINO LEONARDO or other ATmega32u4 Board
   it will not work on an ARDUINO MEGA, due to the Keyboard output requirement

   the screen is a 7" Nextion can be purchased from
   https://itead.cc/product/nx8048k070-nextion-7-0-enhanced-series-hmi-touch-display/
   the file has only been compiled for this version

   the "joystick Mouse" is a 5 wire, I used this from Jaycar (Aus)

   https://www.jaycar.com.au/arduino-compatible-x-and-y-axis-joystick-module/p/XC4422

   horzPin = A0;  // Analog output of horizontal joystick pin
   vertPin = A1;  // Analog output of vertical joystick pin
   int selPin = 7;  // select button pin of joystick

   Dimmer is a standard 10K Pot and switch, Dimmer is Pin A4

   Keyboard mapping is standard DCS F18, I have not added any special key bindings

   note this is the early versions and new files will be required

   Mount can be located here, however it needs work and is not perfect yet
   SIM CONTROL\Mount

*/



#include <Nextion.h>          // Nextion Library
//#include <SoftwareSerial1.h>   // Library for Software Serial Port - not used, program uses Pins 0 and 1 (RX/TX)
//#include <Keyboard.h>         // Keyboard Emulation
#include "NexTouch.h"
//#include <Mouse.h>
#include <HID.h>
//-----------------------VOLUME***************************************VOLUME------------------------------------
#include <HID-Project.h>                    //include HID_Project library
#include <HID-Settings.h>
//-----------------------VOLUME***************************************VOLUME------------------------------------
//SoftwareSerial HMISerial(0,11); // Library for Software Serial Port - not used, program uses Pins 0 and 1 (RX/TX)
// delays

int KPD = 20; // KEYPRESS DELAY
int KPR = 50; // KEYPRESS RELEASE

//------------------------MOUSE***************************************MOUSE-------------------------------------

int selPin = 6;  // select button pin of joystick
int mouseClickFlag = 0;
const int xAxis = A0;         // joystick X axis
const int yAxis = A1;         // joystick Y axis

int MouseSw; // for tunring the mouse on or off ---> int state = OFF untill activated but the Nextion screen
int MouseOn;
int MouseState = LOW;
int lastMouseState;
int currentMouseState;

// parameters for reading the joystick:
int range = 12;               // output range of X or Y movement
int responseDelay = 5;        // response delay of the mouse, in ms
int threshold = range / 4;    // resting threshold
int center = range / 2;       // resting position value

//------------------------MOUSE***************************************MOUSE-------------------------------------
//int brightness = analogRead(A4);

#define KEY_PAUSE (76+136)
#define SafetyTimeOut 10000 // 10 Sec
#define WaitOneHour 3600000
long GlobalLastkeyPressed = WaitOneHour;

//-----------------------VOLUME***************************************VOLUME------------------------------------
//VOLUME
#define REVERSED false                      //if your controller is reversed change it to true
int VolUp;
int VolDn;
int val = 0;
int previousval = 0;
int val2 = 0;
int VolPot = A5;
//-----------------------VOLUME***************************************VOLUME------------------------------------

// - MATCHES NEXTION VERSION "SIm_CONTROL_V2.2.HMI"


//PAGE - MENU
NexButton m_START = NexButton(1, 1, "b0"); //START/FLY
NexButton m_WARP = NexButton(1, 6, "b2"); //WARP

//PAGE - KEYPAD MAP
// B0 - NOT USED INTERNAL "MAIN MENU"
NexButton k_F1 = NexButton(2, 2, "b1"); //F1
NexButton k_F2 = NexButton(2, 3, "b2"); //F2
NexButton k_F3 = NexButton(2, 4, "b3"); //F3
NexButton k_F4 = NexButton(2, 5, "b4"); //F4
NexButton k_F5 = NexButton(2, 6, "b5"); //F5
NexButton k_F6 = NexButton(2, 7, "b6"); //F6
NexButton k_F7 = NexButton(2, 8, "b7"); //F7
NexButton k_F8 = NexButton(2, 9, "b8"); //F8
NexButton k_F9 = NexButton(2, 10, "b9"); //F9
NexButton k_F10 = NexButton(2, 11, "b10"); //F10
NexButton k_F11 = NexButton(2, 12, "b11"); //F11
NexButton k_F12 = NexButton(2, 13, "b12"); //F12
NexButton k_LAB = NexButton(2, 14, "b13"); //LABELS TOGGLE
NexButton k_ATC = NexButton(2, 15, "b14"); //ATC "\"
NexButton k_SAL = NexButton(2, 16, "b15"); // SALUTE
NexButton k_CPT = NexButton(2, 17, "b16"); //COCKPIT "LEFT ALT F1"
NexButton k_SV = NexButton(2, 18, "b17"); //SNAP VIEW
NexButton k_WARP = NexButton(2, 19, "b18"); //WARP
NexButton k_PAUSE = NexButton(2, 20, "b19"); // PAUSE KEY
NexButton k_ESC = NexButton(2, 21, "b20"); // EXIT 'ECS KEY'
NexButton k_MOUSE = NexButton(2, 22, "b21"); // SIM CONTROL MOUSE ON / OFF

// COMPUTER CONTROL
NexButton c_VUP = NexButton(4, 1, "b0"); //VOLUME UP  // VOL not working yet
NexButton c_VMT = NexButton(4, 5, "b4"); //VOLUME MUTE  // VOL not working yet
NexButton c_VDN = NexButton(4, 3, "b2"); //VOLUME DOWN  // VOL not working yet
NexButton c_TAB = NexButton(4, 7, "b7");
NexButton c_ENT = NexButton(4, 9, "b5");
NexButton c_AE = NexButton(4, 10, "b6");
NexButton c_WIN = NexButton(4, 11, "b8");
NexButton c_WARP = NexButton(4, 17, "b14");

// VIEWS (not standard code)
NexButton v_IN = NexButton(5, 9, "b0"); //IN
NexButton v_OUT = NexButton(5, 11, "b4"); //OUT
NexButton v_UP = NexButton(5, 4, "b9"); //UP

NexButton v_LF = NexButton(5, 8, "b13"); //LEFT
NexButton v_RT = NexButton(5, 7, "b12"); //RIGHT
NexButton v_CN = NexButton(5, 5, "b10"); //CENTRE
NexButton v_DN = NexButton(5, 6, "b11"); //DOWN
NexButton v_RS = NexButton(5, 10, "b2"); //RESET // fix later

NexButton v_TL = NexButton(5, 12, "b5"); //TOP LEFT "7"
NexButton v_TR = NexButton(5, 13, "b6"); //TOP RIGHT "9"
NexButton v_BL = NexButton(5, 14, "b7"); //BOT LEFT "1"
NexButton v_BR = NexButton(5, 15, "b8"); //BOT RIGHT "3"

NexButton v_WARP = NexButton(5, 16, "b14"); //WARP

//WARNING - THIS IS A PC SHUTDOWN CODE
NexButton p_PCOFF = NexButton(33, 3, "b0"); //TURN OFF PC REMOTE
//WARNING - THIS IS A PC SHUTDOWN CODE


NexTouch *nex_listen_list[] = {

  &m_START, &m_WARP,

  &k_F1, &k_F2, &k_F3, &k_F4, &k_F5, &k_F6, &k_F7, &k_F8, &k_F9, &k_F10, &k_F11, &k_F12,
  &k_LAB, &k_ATC, &k_SAL, &k_CPT, &k_SV, &k_WARP, &k_PAUSE, &k_ESC, &k_MOUSE,

  &c_TAB, &c_ENT,  &c_AE,  &c_WIN, &c_WARP, & c_VUP, & c_VDN, & c_VMT,

  &v_IN, &v_OUT, &v_UP, &v_LF, &v_RT, &v_CN, &v_DN, &v_RS, &v_TL, &v_TR, &v_BL, &v_BR, &v_WARP,

  &p_PCOFF,
  NULL
};

//--------------------------------------------------------------------------------
void m_STARTPressCallback(void *ptr) { //press
  Keyboard.press(208);
  GlobalLastkeyPressed = millis();
}
void m_STARTReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void m_WARPPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void m_WARPReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F1PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void k_F1ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F2PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F2);
  GlobalLastkeyPressed = millis();
}
void k_F2ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F3PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F3);
  GlobalLastkeyPressed = millis();
}
void k_F3ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}//--------------------------------------------------------------------------------
void k_F4PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F4);
  GlobalLastkeyPressed = millis();
}
void k_F4ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}//--------------------------------------------------------------------------------
void k_F5PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F5);
  GlobalLastkeyPressed = millis();
}
void k_F5ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F6PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F6);
  GlobalLastkeyPressed = millis();
}
void k_F6ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F7PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F7);
  GlobalLastkeyPressed = millis();
}
void k_F7ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F8PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F8);
  GlobalLastkeyPressed = millis();
}
void k_F8ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F9PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F9);
  GlobalLastkeyPressed = millis();
}
void k_F9ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F10PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F10);
  GlobalLastkeyPressed = millis();
}
void k_F10ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}

//--------------------------------------------------------------------------------
void k_F11PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F11);
  GlobalLastkeyPressed = millis();
}
void k_F11ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F12PressCallback(void *ptr) { //press
  Keyboard.press(KEY_F12);
  GlobalLastkeyPressed = millis();
}
void k_F12ReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_LABPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F2);
  GlobalLastkeyPressed = millis();
}
void k_LABReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_ATCPressCallback(void *ptr) { //press
  Keyboard.press('\\');
  GlobalLastkeyPressed = millis();
}
void k_ATCReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_SALPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_CTRL); //SALUTE
  delay(10);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press('S');
  GlobalLastkeyPressed = millis();
}
void k_SALReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_CPTPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_ALT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void k_CPTReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_SVPressCallback(void *ptr) { //press
  Keyboard.press(KEY_F5); // -----------------------BM TO CHECK
  GlobalLastkeyPressed = millis();
}
void k_SVReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_WARPPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void k_WARPReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_PAUSEPressCallback(void *ptr) { //press
  Keyboard.press(0x80);
  GlobalLastkeyPressed = millis();
}
void k_PAUSEReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_ESCPressCallback(void *ptr) { //press
  Keyboard.press(KEY_ESC);
  GlobalLastkeyPressed = millis();
}
void k_ESCReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//-------------------------------------------------------------------------------- Mouse Button ON OFF
void k_MOUSEPressCallback(void *ptr) { //press
  MouseSw = HIGH;
  delay(10);

}
void k_MOUSEReleaseCallback(void *ptr) { //release
  MouseSw = LOW;
  Keyboard.releaseAll();
}

//-------------------------------------------------------------------------------- Mouse Button ON OFF

// &c_TAB, &c_ENT,  &c_AE,  &c_WIN,


//--------------------------------------------------------------------------------
void c_VUPPressCallback(void *ptr) { //press
  VolUp = HIGH;
}
void c_VUPReleaseCallback(void *ptr) { //release
  VolUp = LOW;
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_VDNPressCallback(void *ptr) { //press
  VolDn = HIGH;
}
void c_VDNReleaseCallback(void *ptr) { //release
  VolDn = LOW;
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_VMTPressCallback(void *ptr) { //press
  Keyboard.press(0x7F);
  GlobalLastkeyPressed = millis();
}
void c_VMTReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_TABPressCallback(void *ptr) { //press
  Keyboard.press(179);
  GlobalLastkeyPressed = millis();
}
void c_TABReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_ENTPressCallback(void *ptr) { //press
  Keyboard.press(176);
  GlobalLastkeyPressed = millis();
}
void c_ENTReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_AEPressCallback(void *ptr) { //press
  Keyboard.press(134);
  GlobalLastkeyPressed = millis();
}
void c_AEReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_WINPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_GUI);
  GlobalLastkeyPressed = millis();
}
void c_WINReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_WARPPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void c_WARPReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------




//--------------------------------------------------------------------------------
void v_INPressCallback(void *ptr) { //press 221 '\335' Keypad *
  Keyboard.press(KEYPAD_MULTIPLY);
  GlobalLastkeyPressed = millis();
}
void v_INReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_OUTPressCallback(void *ptr) { //press 220 '\334' Keypad /
  Keyboard.press(KEYPAD_DIVIDE);
  GlobalLastkeyPressed = millis();
}
void v_OUTReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_UPPressCallback(void *ptr) { //press keypad 8 and Up Arrow 232
  Keyboard.press(KEYPAD_8);
  GlobalLastkeyPressed = millis();
}
void v_UPReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_LFPressCallback(void *ptr) { //press Keypad 4 and Left Arrow
  Keyboard.press(KEYPAD_4);
  GlobalLastkeyPressed = millis();
}
void v_LFReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_RTPressCallback(void *ptr) { //press Keypad 6 and Right Arrow
  Keyboard.press(KEYPAD_6);
  GlobalLastkeyPressed = millis();
}
void v_RTReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_CNPressCallback(void *ptr) { //press Keypad 5
  Keyboard.press(KEYPAD_5);
  GlobalLastkeyPressed = millis();
}
void v_CNReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_DNPressCallback(void *ptr) { //press Keypad 2 and Down Arrow 226
  Keyboard.press(KEYPAD_2);
  GlobalLastkeyPressed = millis();
}
void v_DNReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_RSPressCallback(void *ptr) { //press Keypad 6 and Right Arrow 231
  Keyboard.press(KEYPAD_5);
  GlobalLastkeyPressed = millis();
}
void v_RSReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_TLPressCallback(void *ptr) { //press Keypad 7 and Home 231
  Keyboard.press(KEYPAD_7);
  GlobalLastkeyPressed = millis();
}
void v_TLReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_TRPressCallback(void *ptr) { //press Keypad 9 and PageUp 233
  Keyboard.press(KEYPAD_9);
  GlobalLastkeyPressed = millis();
}
void v_TRReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_BLPressCallback(void *ptr) { //press Keypad 1 and End 226 '\342'
  Keyboard.press(KEYPAD_1);
  GlobalLastkeyPressed = millis();
}
void v_BLReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_BRPressCallback(void *ptr) { //press
  Keyboard.press(KEYPAD_3);
  GlobalLastkeyPressed = millis();
}
void v_BRReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_WARPPressCallback(void *ptr) { //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void v_WARPReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------

//----------------------------SHUTDOWN CODE------------------------------------------
void p_PCOFFPressCallback(void *ptr) { //press

  //WARNING - THIS IS A PC SHUTDOWN CODE

  Keyboard.press(KEY_LEFT_GUI);
  delay(10);
  Keyboard.press(KEY_X);
  Keyboard.releaseAll();
  delay(20);
  Keyboard.press(KEY_U);
  delay(10);
  Keyboard.releaseAll();
  delay(10);
  Keyboard.press(KEY_U);
  delay(10);
  Keyboard.releaseAll();

  GlobalLastkeyPressed = millis();
}
void p_PCOFFReleaseCallback(void *ptr) { //release
  Keyboard.releaseAll();
}

//WARNING - THIS IS A PC SHUTDOWN CODE
//----------------------------SHUTDOWN CODE------------------------------------------


// Main Setup
//------------------------SETUP***************************************SETUP-------------------------------------
void setup() {
  //------------------------MOUSE***************************************MOUSE-------------------------------------
  currentMouseState = MouseSw;
  Mouse.begin();
  //------------------------MOUSE***************************************MOUSE-------------------------------------

  //-----------------------VOLUME***************************************VOLUME------------------------------------
  Consumer.begin();                         //initialize computer connection
  delay(250);                              //wait for computer to connect
  for (int a = 0; a < 52; a++) {
    Consumer.write(MEDIA_VOLUME_UP);      //set the volume to 0
    delay(2);
  }
  //-----------------------VOLUME***************************************VOLUME------------------------------------

  Serial1.begin(256000);  // Start Software Serial at 9600bauds

  nexInit();  //  Nextion Display initalize

  m_START.attachPop(m_STARTReleaseCallback, &m_START);
  m_START.attachPop(m_STARTReleaseCallback, &m_START);

  k_F1.attachPush(k_F1PressCallback, &k_F1);
  k_F1.attachPop(k_F1ReleaseCallback, &k_F1);

  k_F2.attachPush(k_F2PressCallback, &k_F2);
  k_F2.attachPop(k_F2ReleaseCallback, &k_F2);

  k_F3.attachPush(k_F3PressCallback, &k_F3);
  k_F3.attachPop(k_F3ReleaseCallback, &k_F3);

  k_F4.attachPush(k_F4PressCallback, &k_F4);
  k_F4.attachPop(k_F4ReleaseCallback, &k_F4);

  k_F5.attachPush(k_F5PressCallback, &k_F5);
  k_F5.attachPop(k_F5ReleaseCallback, &k_F5);

  k_F6.attachPush(k_F6PressCallback, &k_F6);
  k_F6.attachPop(k_F6ReleaseCallback, &k_F6);

  k_F7.attachPush(k_F7PressCallback, &k_F7);
  k_F7.attachPop(k_F7ReleaseCallback, &k_F7);

  k_F8.attachPush(k_F8PressCallback, &k_F8);
  k_F8.attachPop(k_F8ReleaseCallback, &k_F8);

  k_F9.attachPush(k_F9PressCallback, &k_F9);
  k_F9.attachPop(k_F9ReleaseCallback, &k_F9);

  k_F10.attachPush(k_F10PressCallback, &k_F10);
  k_F10.attachPop(k_F10ReleaseCallback, &k_F10);

  k_F11.attachPush(k_F11PressCallback, &k_F11);
  k_F11.attachPop(k_F11ReleaseCallback, &k_F11);

  k_F12.attachPush(k_F12PressCallback, &k_F12);
  k_F12.attachPop(k_F12ReleaseCallback, &k_F12);

  k_LAB.attachPush(k_LABPressCallback, &k_LAB);
  k_LAB.attachPop(k_LABReleaseCallback, &k_LAB);

  k_ATC.attachPush(k_ATCPressCallback, &k_ATC);
  k_ATC.attachPop(k_ATCReleaseCallback, &k_ATC);

  k_SAL.attachPush(k_SALPressCallback, &k_SAL);
  k_SAL.attachPop(k_SALReleaseCallback, &k_SAL);

  k_CPT.attachPush(k_CPTPressCallback, &k_CPT);
  k_CPT.attachPop(k_CPTReleaseCallback, &k_CPT);

  k_SV.attachPush(k_SVPressCallback, &k_SV);
  k_SV.attachPop(k_SVReleaseCallback, &k_SV);

  k_WARP.attachPush(k_WARPPressCallback, &k_WARP);
  k_WARP.attachPop(k_WARPReleaseCallback, &k_WARP);

  k_PAUSE.attachPush(k_PAUSEPressCallback, &k_PAUSE);
  k_PAUSE.attachPop(k_PAUSEReleaseCallback, &k_PAUSE);

  k_ESC.attachPush(k_ESCPressCallback, &k_ESC);
  k_ESC.attachPop(k_ESCReleaseCallback, &k_ESC);

  k_MOUSE.attachPush(k_MOUSEPressCallback, &k_MOUSE);
  k_MOUSE.attachPop(k_MOUSEReleaseCallback, &k_MOUSE);

  c_VUP.attachPush(c_VUPPressCallback, &c_VUP);
  c_VUP.attachPop(c_VUPReleaseCallback, &c_VUP);

  c_VDN.attachPush(c_VDNPressCallback, &c_VDN);
  c_VDN.attachPop(c_VDNReleaseCallback, &c_VDN);

  c_VMT.attachPush(c_VMTPressCallback, &c_VMT);
  c_VMT.attachPop(c_VMTReleaseCallback, &c_VMT);

  c_TAB.attachPush(c_TABPressCallback, &c_TAB);
  c_TAB.attachPop(c_TABReleaseCallback, &c_TAB);

  c_ENT.attachPush(c_ENTPressCallback, &c_ENT);
  c_ENT.attachPop(c_ENTReleaseCallback, &c_ENT);

  c_AE.attachPush(c_AEPressCallback, &c_AE);
  c_AE.attachPop(c_AEReleaseCallback, &c_AE);

  c_WIN.attachPush(c_WINPressCallback, &c_WIN);
  c_WIN.attachPop(c_WINReleaseCallback, &c_WIN);

  c_WARP.attachPush(c_WARPPressCallback, &c_WARP);
  c_WARP.attachPop(c_WARPReleaseCallback, &c_WARP);

  v_IN.attachPush(v_INPressCallback, &v_IN);
  v_IN.attachPop(v_INReleaseCallback, &v_IN);

  v_OUT.attachPush(v_OUTPressCallback, &v_OUT);
  v_OUT.attachPop(v_OUTReleaseCallback, &v_OUT);

  v_UP.attachPush(v_UPPressCallback, &v_UP);
  v_UP.attachPop(v_UPReleaseCallback, &v_UP);

  v_LF.attachPush(v_LFPressCallback, &v_LF);
  v_LF.attachPop(v_LFReleaseCallback, &v_LF);

  v_RT.attachPush(v_RTPressCallback, &v_RT);
  v_RT.attachPop(v_RTReleaseCallback, &v_RT);

  v_CN.attachPush(v_CNPressCallback, &v_CN);
  v_CN.attachPop(v_CNReleaseCallback, &v_CN);

  v_DN.attachPush(v_DNPressCallback, &v_DN);
  v_DN.attachPop(v_DNReleaseCallback, &v_DN);

  v_RS.attachPush(v_RSPressCallback, &v_RS);
  v_RS.attachPop(v_RSReleaseCallback, &v_RS);

  v_TL.attachPush(v_TLPressCallback, &v_TL);
  v_TL.attachPop(v_TLReleaseCallback, &v_TL);

  v_TR.attachPush(v_TRPressCallback, &v_TR);
  v_TR.attachPop(v_TRReleaseCallback, &v_TR);

  v_BL.attachPush(v_BLPressCallback, &v_BL);
  v_BL.attachPop(v_BLReleaseCallback, &v_BL);

  v_BR.attachPush(v_BRPressCallback, &v_BR);
  v_BR.attachPop(v_BRReleaseCallback, &v_BR);

  v_WARP.attachPush(v_WARPPressCallback, &v_WARP);
  v_WARP.attachPop(v_WARPReleaseCallback, &v_WARP);

  p_PCOFF.attachPush(p_PCOFFPressCallback, &p_PCOFF);
  p_PCOFF.attachPop(p_PCOFFReleaseCallback, &p_PCOFF);

  Keyboard.releaseAll(); // INCASE A LOCKED ON KEY

  pinMode(selPin, INPUT);  // set button select pin as input
  digitalWrite(selPin, HIGH);  // Pull button select pin high
  delay(5);  // short delay to let outputs settle

  Serial1.print("dim=100"); // SET ON TO 100% BRIGHT
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  delay(500);

}
// Main Loop
void loop() {

  {
    int brightness = analogRead(A4);
    int bright = map(brightness, 0, 1023, 10, 100);
    String dim = "dim=" + String(bright);
    brightness = bright;
    Serial1.print(dim.c_str());
    Serial1.write("\xFF\xFF\xFF");
  }



  //-----------------------VOLUME***************************************VOLUME------------------------------------


// Volume KY58 Volume Knob Control - Added V2.4 14/5/2023

  val = analogRead(A5);                     //read potentiometer value
  val = map(val, 0, 1023, 0, 101);          //map it to 102 steps
  if(REVERSED) {
    val = 101 - val;
  }
  if(abs(val - previousval) > 1) {          //check if potentiometer value has changed
    previousval = val;
    val /= 2;                               //divide it by 2 to get 51 steps
    while(val2 < val) {
      Consumer.write(MEDIA_VOLUME_UP);      //turn volume up to appropiate level
      val2++;
      delay(2);
    }
    while(val2 > val) {
      Consumer.write(MEDIA_VOLUME_DOWN);    //turn volume down to appropiate level
      val2--;
      delay(2);
    }
  }

// Volume Nextion Control

  if (VolUp == HIGH) {         //Soft Key input from Nextion Screen --Volume Up--
    Consumer.write(MEDIA_VOLUME_UP);
    delay(1);
  }
  if (VolDn == HIGH) {         //Soft Key input from Nextion Screen --Volume Dn--
    Consumer.write(MEDIA_VOLUME_DOWN);
    delay(1);
  }

  delay(5);
  //-----------------------VOLUME***************************************VOLUME------------------------------------

  
  if (millis() >= (GlobalLastkeyPressed + SafetyTimeOut)) {
    Keyboard.releaseAll();
    GlobalLastkeyPressed = millis() + WaitOneHour;
  }

  nexLoop(nex_listen_list);  // Check for any touch event and run the associated function


  //------------------------MOUSE***************************************MOUSE-------------------------------------
  // for tunring the mouse on or off ---> int state = OFF untill activated but the Nextion screen
  lastMouseState =  currentMouseState;      // save the last state
  currentMouseState = MouseSw;
  if (lastMouseState == HIGH && currentMouseState == LOW) {

    MouseState = !MouseState;
    MouseOn = MouseState;
  }
  // read and scale the two axes:
  int xReading = readAxis(A0);
  int yReading = readAxis(A1);

  //  if the mouse control state is active, move the mouse:
  if (MouseOn == HIGH) {
    Mouse.move(xReading, yReading, 0);
 
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
  delay(responseDelay);
}
 // for tunring the mouse on or off ---> int state = OFF untill activated but the Nextion screen

}
int readAxis(int thisAxis) {
  // read the analog input:
  int reading = analogRead(thisAxis);

  // map the reading from the analog input range to the output range:
  reading = map(reading, 0, 1023, 0, range);

  // if the output reading is outside from the rest position threshold, use it:
  int distance = reading - center;

  if (abs(distance) < threshold) {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;



  //------------------------MOUSE***************************************MOUSE-------------------------------------



}
