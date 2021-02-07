
/*********************************************************************************
* File: PS4_Programming_Challenge.ino
* Date: 26 January 2017
* Author: Eric Krebs (ekrebs@nd.edu)
* Purpose: Standard base code for teaching how to code a robot.
*          Starting on line 107, the task is to program a robot that can drive!
**********************************************************************************/

/***************************************************
 * Include Statements
 ***************************************************/
 
// PS4 Header Files
#include <PS4BT.h>
#include <usbhub.h>

// ND Library
#include <Motors.h>

/***************************************************
* Addressing and Other Constants
***************************************************/

// Baud Rate
#define BAUD_RATE 115200

// Gain Settings
#define THROTTLE_GAIN     140 // for drive wheels
#define STEERING_GAIN     50  // for drive wheels

// Offsets
#define ZERO_OFFSET 0

// SPIN_HARD is used to determine the spin in place for spinning with RB and LB ... 0-255
#define SPIN_HARD   80
#define BOUNCEDELAY 20

// Pins
#define TACKLE_PIN 4 // 
#define LEFT_MOTOR_PIN 5 // interior wire
#define RIGHT_MOTOR_PIN 6 // interior wire
#define LED 13
#define TACKLE_PIN      4 
#define LEFT_MOTOR_PIN  5
#define RIGHT_MOTOR_PIN 6

/***************************************************
 * Function Prototypes and Namespaces
 ***************************************************/
 
void updateDrive(boolean);

using namespace ND;

/***************************************************
* Pre-Setup
***************************************************/

// Setup PS4 Controller
USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd);

/***************************************************
* Global Variables
***************************************************/

// Motor Class
Servo lm; Servo rm;
Motors Wheels(&lm, &rm, THROTTLE_GAIN, STEERING_GAIN,ZERO_OFFSET);


/***************************************************
* Setup
***************************************************/

void setup()
{
  // Begin Serial Communication
  Serial.begin(BAUD_RATE);
  
  // Check to Make Sure USB Initialized
  if (Usb.Init() == -1) 
  {
    Serial.print(F("\r\nOSC did not start"));
    while(1); // Halt
  }

  pinMode(LED, OUTPUT);

  // Setup Motors;
  lm.attach(LEFT_MOTOR_PIN); rm.attach(RIGHT_MOTOR_PIN);
  Wheels.steering_gain = STEERING_GAIN;
  Wheels.throttle_gain = THROTTLE_GAIN;
  Wheels.Stop();
  Serial.println("Wheels Setup");
}

/***************************************************
* Loop
***************************************************/

void loop()
{
  // Complete remainder of code here (add new lines too):
  //////////////////////////////////
  
  
  //////////////////////////////////
}

// Commands you can use:
// *********************
// PS4.getButtonPress( BUTTON );
// PS4.getButtonPress( BUTTON );
//      BUTTON arguments: CROSS, X, SQUARE, CIRCLE, UP, RIGHT, DOWN, LEFT, L1, R1, (more?)
//
// PS4.getAnalogHat( STICK );
//      STICK arguments: LeftHatY, RightHatX, LeftHatX, RightHatX
//
// PS4.getAnalogButton( BUTTON );
//      BUTTON arguments: R2, L2
//
// Wheels.Move(int lmotor, int rmotor);
//      lmotor and rmotor are integers between -255 and 255
// 
// Wheels.Stop()
//
// Wheels.Controller( lHatY/2, rHatX/2 );
//
// Serial.println("This is great for debugging!");
//
//
//
// Commands just for fun:
// **********************
// PS4.setRumbleOn( SETTING );
//      SETTING arguments: RumbleHigh, RumbleLow
//
// PS4.setRumbleOff();
//
// PS4.setLed( COLOR );
//      COLOR arguments: Red, Blue, Yellow, Green, (maybe more?)
//
//
//
// To speed up your coding if you want to:
// ***************************************
// Wheels.Controller( lHatY/2, rHatX/2 );
// This uses a built in controller function.


