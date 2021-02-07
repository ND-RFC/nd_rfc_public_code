/* 
*-----------------------------------------------------
* This is the main receiving code for a lineman.  What you may need to change is the CONTROLLER_ADDRESS found in the "Addressing and Other Constants"
* There are 3 functions: setup(), loop(), and control()
*-----------------------------------------------------
* Author : Joe Rudy
* Date  :  3/15/2013
*/
///////////////////////////////////////////////////////////////////// FUNCTIONS//////////////////////////////////////////////////////


/*  Title    :  setup
*   Purpose  :  provides setup for the rxdata class and the ndbee class as well as serial printing
*               ...delay 2 seconds at the end of setup to make sure everything is setup well :)
*/

/*   Title    :  loop
*    Purpose  :  this is the main loop of the code 
*                The main structure goes something like this:
*                  1) read the tackle sensor ...
*                  2) read any packets 
*                      2a) if we got a packet, update our rxdata
*                      2b) if we don't have a packet, increment a counter and continue ... 
*                      after the counter reaches a certain limit, it will cut off power to the motors until a packet is received
*                  3) Select the proper mode (0 - 3)
*                      3a) Mode 0: uses the Left Joystick Y value as throttle and Right Joystick X value as steering
*                      3b) Mode 1: Same as 0 but with different gains
*                      3c) Mode 2: Gain Calibration :)
*                      3d) Mode 3: Calibration mode ... used to calibrate the motor offsets
* enjoy !
*/

/*   Title    :  control
*    Purpose  :  this gives the main control code for driving a robot
*                if the robot is tackled then stop motors until untackled
*/

/* CALIBRATION MODE (Mode == 3) for setting the motor offsets
* Here is how you calibrate ...
* There are forward and reverse motor offsets for when you are going forward or backward and they are on 1 OF THE 2 MOTORS!!
* First, let's calibrate forward.  Go forward and see which motor is faster. If it goes straight, no calibration necessary :) ... move on to calibrate reverse.
* However, if it fades to the right (on a smooth surface) then choose the forward motor as the left motor because the left motor is faster and we need to knock it down.
* The opposite is true - if it fades to the left, select the right motor as the offset motor for forward.
* Then just keep increasing the offset until you get reasonable performance at high speeds
* Do the same thing for reverse... 
* Afterwards, save these values by pressing START ... try not to save too often as the arduino only has limited writes to the EEPROM before it's memory is done :) (~100,000 to be exact)
*
* Now here are the controls
*
* Forward: (DO NOT have Right Trigger pressed!!)
* Picking a side: 2 choices
*            1) X button : chooses left motor as offset motor (faster motor)
*            2) B button : chooses right motor as offset motor (faster motor)
* Incrementing and Decrementing offset:
*            1) Y Button : Increases offset (offset can go from 0 to 255) ... you should not be up to 255
*            2) A Button : Decreases offset (offset can go from 0 to 255) ... you should not be up to 255
* Reverse: (Same as forward but just press the Right Trigger down
*/
/* Get which offset we are calibrating ... either forward or reverse */

/***************************************************
* Addressing and Other Constants
***************************************************/
#define BAUD_RATE 38400
#define CONTROLLER_ADDRESS 0x9009
#define THROTTLE_GAIN 200
#define STEERING_GAIN 150
#define THROTTLE_GAIN2 135
#define STEERING_GAIN2 45
#define ZERO_OFFSET 0
#define SPIN_HARD 200 // this value is to determine the spin in place for spinning with RB and LB ... 0-255

/***************************************************
* Pin Numbers
***************************************************/
#define TACKLE_SENSOR_PIN 8 // first interrupt ... this is digital pin 2
#define LEFT_MOTOR_PIN 10 // green
#define RIGHT_MOTOR_PIN 11 // red
#define PIN_IN 3
#define PIN_OUT 4

/***************************************************
* Include Files
***************************************************/
#include <ND/Xbox/RXDATA.h>
#include <ND/Actuation/Motors.h>
#include <ND/Xbee/ND_XBEE.h>
#include <ND/Sensors/Sensor.h>

/***************************************************
* Function Prototypes and Namespaces
***************************************************/
void control(signed char throttle, signed char steering,signed char side2side);

using namespace ND;

/***************************************************
* Debugging Variables
***************************************************/
//#define DEBUG_TIME
#ifdef DEBUG_TIME
  long time1, time2;
#endif

#define DEBUG
#ifdef DEBUG // variables used in debugging
  int * vals1, *vals2;
  motor_offsets mo;
  uint8_t val;
#endif

/***************************************************
* Global Variables
***************************************************/
// Motor Class
Servo lm; Servo rm;
Motors Wheels(&lm,&rm,THROTTLE_GAIN,STEERING_GAIN,ZERO_OFFSET); // used to control a pair of steering motors
// Data Classes
ND_XBEE ndbee; // used for RF communication
Rx16Response rx16 = Rx16Response();
RXDATA rxdata; // used to handle the incoming data from the xbox controller... button presses, releases, etc.
// Sensor Class
bool tackled = false; // determines if you are tackled or not
Tackle_Sensor tackle_sensor(TACKLE_SENSOR_PIN);
uint8_t message[1] = {1};
/***************************************************
* setup
***************************************************/
void setup() {
  // Setup motors;
  lm.attach(LEFT_MOTOR_PIN); rm.attach(RIGHT_MOTOR_PIN);
  Wheels.Stop();
  Serial.begin(BAUD_RATE);
  // setup the RF communication
  ndbee = ND_XBEE();
  ndbee.xbee.setSerial(Serial);
  // setup the a way to receive the transmission packets
 // attachInterrupt(TACKLE_SENSOR_PIN-2,tackle_isr,FALLING);
  rxdata = RXDATA();
  
  
  pinMode(PIN_IN, OUTPUT);
  pinMode(PIN_OUT, OUTPUT);
  digitalWrite(PIN_IN, HIGH);
  digitalWrite(PIN_OUT, HIGH);
  
  Serial.println("START...");
  delay(3000); // delay 2 seconds
}
int count;
long inTime;
long outTime;

/***************************************************
* loop
***************************************************/
void loop() {
#ifdef DEBUG_TIME
  time1 = micros();
#endif
  //tackle_sensor.Read();
/************************************************************
* PART 1. Read the Tackle Sensor
************************************************************/
  tackle_sensor.Read();
/************************************************************
* PART 2. Read the Packet
************************************************************/
  if (ndbee.Read()) {
    // got something -- Check the Address
    if (ndbee.Address16() == CONTROLLER_ADDRESS) {
      rxdata.set(ndbee.getData());
#ifdef DEBUG
      Serial.print("Succes");
      rxdata.debugprint(&Serial);
#endif
    }
#ifdef DEBUG
Serial.print(ndbee.Address16());
Serial.print(" ADDR\t");
    Serial.println("gotsomething");
#endif
  } else {
      rxdata.NoPacket = true;
      if (rxdata.TimeOut()) rxdata.ZERO(); // after so many cycles of no packets cut the power...
  }  
  
/************************************************************
* PART 3. Select Your Mode
************************************************************/
  switch (rxdata.get(MODE,2))
  {
    case 0:
      
        //Moves Pin In and Out
        if(rxdata.get(A) && millis() - inTime > 500)
        {
           digitalWrite(PIN_IN, LOW); 
           inTime = millis();
        }
        else if(millis() - inTime > 250)
        {
          digitalWrite(PIN_IN, HIGH); 
        }
        if(rxdata.get(X) && millis() - outTime > 500)
        {
           digitalWrite(PIN_OUT, LOW); 
           outTime = millis();
        }
        else if(millis() - outTime > 250)
        {
          digitalWrite(PIN_OUT, HIGH); 
        }
      
      
        Wheels.steering_gain = STEERING_GAIN;
        Wheels.throttle_gain = THROTTLE_GAIN;
      if (!rxdata.get(B)) {
        if (!rxdata.get(R1) && !rxdata.get(L1) && !rxdata.getTrigger(R2) && !rxdata.getTrigger(L2)) { // use controller
          control(rxdata.getJoystick(LY), rxdata.getJoystick(RX),rxdata.getJoystick(LX)); // control
        } else if (rxdata.get(R1)) { // spin fast right (CW looking at ground)
          if (!tackle_sensor.tackled()) { 
            Wheels.Move(SPIN_HARD,-SPIN_HARD);
          } else Wheels.Stop();
        } else if (rxdata.get(L1)) {
          if (!tackle_sensor.tackled()) { 
            Wheels.Move(-SPIN_HARD,SPIN_HARD);
          } else Wheels.Stop();
        } else if (rxdata.getTrigger(R2)) {
          if (!tackle_sensor.tackled()) { 
            Wheels.TurnRight(rxdata.getJoystick(LY),rxdata.getTrigger(R2));
          } else Wheels.Stop();
        } else if (rxdata.getTrigger(L2)) {
          if (!tackle_sensor.tackled()) { 
            Wheels.TurnLeft(rxdata.getJoystick(LY),rxdata.getTrigger(L2));
          } else Wheels.Stop();
        } else Wheels.Stop();
      }
      else if (Wheels.moving) Wheels.Stop(); // brake 
        break;
    case 1:
        Wheels.steering_gain = STEERING_GAIN2;
        Wheels.throttle_gain = THROTTLE_GAIN2;
        if (!rxdata.get(B)) {
          if (!rxdata.get(R1) && !rxdata.get(L1) && !rxdata.getTrigger(R2) && !rxdata.getTrigger(L2)) { // use controller
            control(rxdata.getJoystick(LY), rxdata.getJoystick(RX),rxdata.getJoystick(LX)); // control
          } else if (rxdata.get(R1)) { // spin fast right (CW looking at ground)
            if (!tackle_sensor.tackled()) { 
              Wheels.Move(SPIN_HARD,-SPIN_HARD);
            } else Wheels.Stop();
          } else if (rxdata.get(L1)) {
            if (!tackle_sensor.tackled()) { 
              Wheels.Move(-SPIN_HARD,SPIN_HARD);
            } else Wheels.Stop();
          } else if (rxdata.getTrigger(R2)) {
            if (!tackle_sensor.tackled()) { 
              Wheels.TurnRight(rxdata.getJoystick(LY),rxdata.getTrigger(R2));
            } else Wheels.Stop();
          } else if (rxdata.getTrigger(L2)) {
            if (!tackle_sensor.tackled()) { 
              Wheels.TurnLeft(rxdata.getJoystick(LY),rxdata.getTrigger(L2));
            } else Wheels.Stop();
          } else Wheels.Stop();
        }
        else if (Wheels.moving) Wheels.Stop(); // brake 
        break;
    case 2:
        // Gain Calibration
        if (rxdata.ButtonPress(A)) Wheels.change_gains(1,-1); // change throttle down
        if (rxdata.ButtonPress(Y)) Wheels.change_gains(1,1); // change throttle up
        if (rxdata.ButtonPress(X)) Wheels.change_gains(2,-1); // change steering down
        if (rxdata.ButtonPress(B)) Wheels.change_gains(2,1); // change steering up
#ifdef DEBUG
        Serial.print("T_gain: "); Serial.print(Wheels.gains(1)); Serial.print(" \t");
        Serial.print("S_gain: "); Serial.println(Wheels.gains(2));
#endif
        break;
    case 3: // CALIBRATION MODE:
        control(rxdata.getJoystick(LY), rxdata.getJoystick(RX),rxdata.getJoystick(LX)); // control
        if (rxdata.getTrigger(R2)) { // calibrate the reverse offset if R2 is pressed
          Wheels.cdata.fwd_rev = CALIBRATE_REVERSE; 
          Wheels.cdata.left_right = CALIBRATE_MOTOR_REVERSE;
        } else { // calibrate the forward offset if R2 is not pressed
          Wheels.cdata.fwd_rev = CALIBRATE_FORWARD;
          Wheels.cdata.left_right = CALIBRATE_MOTOR_FORWARD;
        }
        /* Deal with the Buttons */
        if (rxdata.ButtonPress(A)) Wheels.Calibrate(  -1  ,  Wheels.cdata.fwd_rev);
        if (rxdata.ButtonPress(Y)) Wheels.Calibrate(   1  ,  Wheels.cdata.fwd_rev);
        if (rxdata.ButtonPress(X)) Wheels.Calibrate(  MOTOR_LEFT   ,  Wheels.cdata.left_right);
        if (rxdata.ButtonPress(B)) Wheels.Calibrate(  MOTOR_RIGHT  ,  Wheels.cdata.left_right);
        if (rxdata.ButtonPress(START)) Wheels.Calibrate(0, CALIBRATE_SAVE_OFFSETS);
#ifdef DEBUG
        mo = Wheels.offs();
        val = mo.forward(); 
        Serial.println();
        Serial.print("fwd: "); Serial.print(val); Serial.print(" \trev: ");
        val = mo.reverse();
        Serial.print(val); Serial.print(" \tdir: ");
        val = mo.direct();
        Serial.println(val);
#endif
        break;
    default: break;
  };
//}
#ifdef DEBUG_TIME
  time2 = micros();
  Serial.print("One loop = "); Serial.print(time2-time1); Serial.println(" microseconds");
#endif
}
/***************************************************
* control
***************************************************/
void control(signed char throttle, signed char steering, signed char side2side)
{
   if (!tackle_sensor.tackled()) {
     //if (!tackled) {
      Wheels.Controller(throttle,steering);
#ifdef DEBUG
      vals1 = Wheels.vals(1); vals2 = Wheels.vals(2);
      Serial.print("Left: "); Serial.print(vals1[0]); Serial.print(" \t");
      Serial.print("Right: "); Serial.println(vals2[0]);
#endif
    } else { // stop the motors if tackled
      Wheels.Stop();
#ifdef DEBUG
      Serial.println("TACKLED");
#endif
    }
}
