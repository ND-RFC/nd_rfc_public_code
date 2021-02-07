/*********************************************************************************
* NOTE:
* Please set the pins for the 3 center servos prior to using.
**********************************************************************************/
/*********************************************************************************
* File: Center.ino
* Version: StdBase_1.0
* Date: 2018
**********************************************************************************/

/***************************************************
 * Include Statements
 ***************************************************/
 
// PS4 Header Files
#include "PS4Parser.h"
#include <PS4BT.h>
#include <usbhub.h>

// ND Library
#include <Motors.h>

// Needed for Compilation
#ifdef dobogusinclude
  #include <spi4teensy3.h>
  #include <SPI.h>
#endif

/***************************************************
* Addressing and Other Constants
***************************************************/

// Baud Rate
#define BAUD_RATE 115200

// Capable of Automatic Mode
#define IS_AUTOMATIC false

// Gain Settings
#define THROTTLE_GAIN     140 // for drive wheels
#define STEERING_GAIN     50  // for drive wheels

// Limits for alignment servo (in microseconds)
#define PWM1_MIN 1675 // (the leftmost)
#define PWM1_MID 2023
#define PWM1_MAX 2379 // (the rightmost)

// Limits for extension servo (in degrees)
#define EXTEND_MIN 108 // (higher value)
#define EXTEND_COLLISION 80 // where the drop arm would hit the cage
#define EXTEND_MAX 38   // (lower value)

//Limits for drop servo (in degrees)
#define DROP_MIN 800  // (lower value)
#define DROP_MAX 2100   // (higher value)

// Offsets
#define ZERO_OFFSET 0

// Controller Mode constants
#define MANUAL_MODE       1
#define AUTOMATIC_MODE    2
#define CALIBRATION_MODE  3
#define TUTORIAL_MODE     4

// SPIN_HARD is used to determine the spin in place for spinning with RB and LB ... 0-255
#define SPIN_HARD   80
#define BOUNCEDELAY 20

// Pins
#define TACKLE_PIN 4
#define LEFT_MOTOR_PIN 5 // interior wire
#define RIGHT_MOTOR_PIN 6 // interior wire
#define ALIGNMENT_PIN 7
#define EXTENSION_PIN 8
#define DROP_PIN 9
#define LED 13
#define TACKLE_PIN      4 

#define CONTROLLER_TIMEOUT_MILLIS 50

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
PS4BT PS4(&Btd); // to pair: PS4BT PS4(&Btd, PAIR);

// Setup BT connection monitoring variables
int lastReportCounter = 0;
unsigned long timeOfLastReportCounterChange = millis();

/***************************************************
* Global Variables
***************************************************/

// Motor Class
Servo lm; Servo rm;
Motors Wheels(&lm,&rm,THROTTLE_GAIN,STEERING_GAIN,ZERO_OFFSET);

// Arm servos
Servo alignment;
Servo extension;
Servo drop_arm;

int align = PWM1_MID;
double extend = EXTEND_MAX;
unsigned int drop = DROP_MIN;
int dropped = 0;

// Tutorial Mode (for kids)
int mode = MANUAL_MODE;

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
  // must initialize USB prior to attaching servos
  
  // Setup Motors;
  lm.attach(LEFT_MOTOR_PIN);
  rm.attach(RIGHT_MOTOR_PIN);
  alignment.attach(ALIGNMENT_PIN);
  extension.attach(EXTENSION_PIN);
  drop_arm.attach(DROP_PIN);

  alignment.writeMicroseconds(align);
  extension.write((int) extend);
  Serial.print("About to write drop_arm");
  drop_arm.writeMicroseconds(DROP_MIN);

  Wheels.steering_gain = STEERING_GAIN;
  Wheels.throttle_gain = THROTTLE_GAIN;
  Wheels.Stop();
  Serial.println("Wheels Setup");

  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
  pinMode(LED, OUTPUT);

  // Set Controller Color
  PS4.setLed(Green);
}

/***************************************************
* Loop
***************************************************/

void loop()
{
  // Get data from the controller
  Usb.Task();
  
  // PS4 Controller Connected
  if (PS4.connected()) {
    updateDrive(false);

    // If the controller hasn't output a report coutner in a while, reset the input buffer
    if (PS4.getReportCounter() != lastReportCounter) {
      
      timeOfLastReportCounterChange = millis();
      lastReportCounter = PS4.getReportCounter();
      
    } else if (millis() - timeOfLastReportCounterChange > CONTROLLER_TIMEOUT_MILLIS) {
      
      timeOfLastReportCounterChange = millis();
      PS4.resetInputBuffer();
      
    }
    // For more info on PS4 output visit: http://www.psdevwiki.com/ps4/DS4-USB
    
  }
  else 
  {
    Wheels.Stop();
    Serial.print("PS4 Controller Not Connected\n");
  }
}

/***************************************************
 * Drive Function
 ***************************************************/
 
void updateDrive(boolean blnTutorialMode)
{

  int lHatY = -PS4.getAnalogHat(LeftHatY); // Forward, backward
  int rHatX = PS4.getAnalogHat(RightHatX); // Right, left
  int lHatX = PS4.getAnalogHat(LeftHatX);

  // Check Button Combinations and decide what movement to send to speed controller
  if (mode == TUTORIAL_MODE)
    Wheels.Controller(lHatY/2,rHatX/2); // regular control
  else if (mode == AUTOMATIC_MODE)
    Wheels.Stop(); // Temporary until automatic mode is built in
  else if (PS4.getAnalogButton(R2))
    Wheels.TurnRight(-lHatY,PS4.getAnalogButton(R2)); // high speed turning right
  else if (PS4.getAnalogButton(L2))
    Wheels.TurnLeft(-lHatY,PS4.getAnalogButton(L2)); // high speed turning left
  else if (PS4.getButtonPress(R1)) 
    Wheels.Move(SPIN_HARD,-SPIN_HARD); // spin fast right (CW looking at ground)
  else if (PS4.getButtonPress(L1))
    Wheels.Move(-SPIN_HARD,SPIN_HARD); // spin fast left (CCW looking at ground)
  // =============================================================================
  else if (PS4.getButtonClick(RIGHT))
  {
    align += 50;
    if (align > PWM1_MAX) align = PWM1_MAX;
    alignment.writeMicroseconds(align);
  }
  else if (PS4.getButtonClick(LEFT))
  {
    align -= 50;
    if (align < PWM1_MIN) align = PWM1_MIN;
    alignment.writeMicroseconds(align);
  }
  else if (PS4.getButtonPress(UP))
  {
    
    extend += .025;
    // if arm is "dropped", do no move more than halfway
    if (dropped && (extend > EXTEND_COLLISION))
    {
      extend = EXTEND_COLLISION;
    }
    else if (extend > EXTEND_MIN)
    {
      extend = EXTEND_MIN;
    }
    extension.write((int) extend);
  }
  else if (PS4.getButtonPress(DOWN))
  {
    
    extend -= .025;
    if (extend < EXTEND_MAX)  extend = EXTEND_MAX;
    extension.write((int) extend);
  }
  // Update drop arm:
  else if (PS4.getButtonClick(CROSS))
  {
    // Only drop if arm is more than halfway extended
    if (dropped == 0 && (extend < EXTEND_COLLISION))
    {
      drop = DROP_MAX;
      dropped = 1;
      drop_arm.writeMicroseconds(drop);
    }
    else
    {
      drop = DROP_MIN;
      dropped = 0;
      drop_arm.writeMicroseconds(drop);
    }
  }
  else
  {
    Wheels.Controller(lHatY,rHatX); // regular control
  }
}
