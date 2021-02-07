/*********************************************************************************
* File: StdBase.ino
* Version: StdBase_1.2 (with calibration mode & watchdog)
* Date: 02 October 2017
**********************************************************************************/

/***************************************************
 * Include Statements
 ***************************************************/

#include <avr/wdt.h>
 
// PS4 Header Files
#include "PS4Parser.h"
#include <PS4BT.h>
#include <usbhub.h>

// ND Library
#include <Motors.h>

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

// Calibration Addresses
#define L_ADDRESS 12
#define R_ADDRESS 13

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
#define TACKLE_PIN 4 // 
#define LEFT_MOTOR_PIN 5 // interior wire
#define RIGHT_MOTOR_PIN 6 // interior wire
#define LED 13
#define TACKLE_PIN      4 
#define LEFT_MOTOR_PIN  5
#define RIGHT_MOTOR_PIN 6

#define CONTROLLER_TIMEOUT_MILLIS 50

/***************************************************
 * Function Prototypes and Namespaces
 ***************************************************/
 
void updateDrive(boolean);

void updateCalibration();

using namespace ND;

/***************************************************
* Pre-Setup
***************************************************/

// Setup PS4 Controller
USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd);

// Setup BT connection monitoring variables
int lastReportCounter = 0;
unsigned long timeOfLastReportCounterChange = millis();

/***************************************************
* Global Variables
***************************************************/

// Motor Class
Servo lm; Servo rm;
Motors Wheels(&lm,&rm,THROTTLE_GAIN,STEERING_GAIN,ZERO_OFFSET);
// Tutorial Mode (for kids)
int mode = MANUAL_MODE;

// Calibration Constants
int dL = 0;
int dR = 0;

/***************************************************
* Setup
***************************************************/

void setup()
{
  // disable the watchdog timer
  MCUSR = 0x00;
  wdt_disable();
  //Serial.flush();
  // Begin Serial Communication
  Serial.begin(BAUD_RATE);
  
  // Setup Motors;
  lm.attach(LEFT_MOTOR_PIN); rm.attach(RIGHT_MOTOR_PIN);
  Wheels.steering_gain = STEERING_GAIN;
  Wheels.throttle_gain = THROTTLE_GAIN;
  Wheels.Stop();
  Serial.println("Wheels Setup");
  // Check to Make Sure USB Initialized
  if (Usb.Init() == -1) 
  {
    Serial.print(F("\r\nOSC did not start"));
    while(1) {Wheels.Stop();} // Halt
  }
  
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
  pinMode(LED, OUTPUT);

  // Load calibration from memory
  dL = EEPROM.read(L_ADDRESS) - 128;
  dR = EEPROM.read(R_ADDRESS) - 128;
  if (abs(dL) > 50) dL = 0;
  if (abs(dR) > 50) dR = 0;
  Wheels.Calibrate(0, dL);
  Wheels.Calibrate(1, dR);

  Wheels.Stop();
}

/***************************************************
* Loop
***************************************************/
void loop()
{
  
  wdt_reset(); // reset the watchdog timer
  // Get data from the controller
  Usb.Task();
  
  // PS4 Controller Connected
  if (PS4.connected()) {
    
    wdt_enable(WDTO_30MS);
    wdt_reset(); // reset the watchdog timer
    
    // PS4 Controller Connected
    if (mode == CALIBRATION_MODE) {
      updateCalibration();
    }
    else
    {
      PS4.setLed(Green);
    }
    
    // Update the drive
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
    wdt_disable();
  }
}

/***************************************************
 * Drive Function
 ***************************************************/
 
void updateDrive(boolean blnTutorialMode)
{
  // Variables
  bool doStop = false;

  // Check for button presses
  if(PS4.getButtonPress(SQUARE))
  {
    Serial.println("SQUARE Pressed");
    doStop = true;
  }
  // Select which mode to use robot
  if(PS4.getButtonClick(OPTIONS))
  {
    if (mode == CALIBRATION_MODE) {
      mode = MANUAL_MODE;
      PS4.setLedFlash(0, 0); // Stop flashing light
      // Save the calibration settings when leaving calibration mode:
      EEPROM.write(L_ADDRESS, Wheels.get_zero_offset(0)+128);
      EEPROM.write(R_ADDRESS, Wheels.get_zero_offset(1)+128);
    }
    else {
      mode = CALIBRATION_MODE;
      PS4.setLed(Blue);
      PS4.setLedFlash(50, 10);
    }
    
  }
  if(PS4.getButtonClick(PS) && IS_AUTOMATIC)
  {
    Serial.println("PS Pressed");
    if(mode == AUTOMATIC_MODE) mode = MANUAL_MODE;
    else mode = AUTOMATIC_MODE;
  }
   
  /* The PS4 Controller returns a value between 0 and 255, with ~127 being center.*/ 
   
  int lHatY = PS4.getAnalogHat(LeftHatY); // Forward, backward
  int rHatX = PS4.getAnalogHat(RightHatX); // Right, left
  /*
  if(lHatY > 0)   lHatY = 0;
  if(lHatY < -255) lHatY = -255;
  if(rHatX < 0)   rHatX = 0;
  if(rHatX > 255) rHatX = 255;
  */


  // Check Button Combinations and decide what movement to send to speed controller
   
  if (doStop)
    Wheels.Stop();
  else if (mode == TUTORIAL_MODE)
    Wheels.Controller(-lHatY/2,rHatX/2); // regular control
  else if (mode == AUTOMATIC_MODE)
    Wheels.Stop(); // Temporary until automatic mode is built in
  else if (PS4.getAnalogButton(R2))
    Wheels.TurnRight(-lHatY + 127,PS4.getAnalogButton(R2)); // high speed turning right
  else if (PS4.getAnalogButton(L2))
    Wheels.TurnLeft(-lHatY + 127,PS4.getAnalogButton(L2)); // high speed turning left
  else if (PS4.getButtonPress(R1)) 
    Wheels.Move(SPIN_HARD,-SPIN_HARD); // spin fast right (CW looking at ground)
  else if (PS4.getButtonPress(L1))
    Wheels.Move(-SPIN_HARD,SPIN_HARD); // spin fast left (CCW looking at ground)
  else
    Wheels.Controller(-lHatY,rHatX); // regular control
}

/***************************************************
 * Calibration Functions
 ***************************************************/

void updateCalibration() {
  if(PS4.getButtonClick(UP)) {
    Wheels.Calibrate(0, 1);
  }
  else if(PS4.getButtonClick(DOWN)) {
    Wheels.Calibrate(0, -1);
  }
  else  if(PS4.getButtonClick(TRIANGLE)) {
    Wheels.Calibrate(1, 1);
  }
  else if(PS4.getButtonClick(CROSS)) {
    Wheels.Calibrate(1, -1);
  }
}



