/* Drive_Center_KURT
* This is downloaded to the robot.  Make sure the switch is set to USB.
*-----------------------------------------------------
* This is the main receiving code for a lineman.  What you may need to change is the CONTROLLER_ADDRESS found in the "Addressing and Other Constants"
* There are 3 functions: setup(), loop(), and control()
*-----------------------------------------------------
* Author : Joe Rudy
* Date  :  3/15/2013
*
* Modified:  2/25/2015 - Kat Landers
* Center Specific Additions: 4/6/2015 - Eddie Hunckler

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
#define CONTROLLER_ADDRESS 0x9012
#define THROTTLE_GAIN 140
#define STEERING_GAIN 50 
#define THROTTLE_GAIN2 125
#define STEERING_GAIN2 45
#define ZERO_OFFSET 0
#define SPIN_HARD 135 // this value is to   determine the spin in place for spinning with RB and LB ... 0-255
#define BOUNCEDELAY 20
/***************************************************
 * Pin Numbers
 ***************************************************/
#define TACKLE_SENSOR_PIN 8 // first interrupt ... this is digital pin 2
#define LEFT_MOTOR_PIN 10 // interior wire
#define RIGHT_MOTOR_PIN 11 // interior wire

#define LIMITPINDOWN 6
#define LIMITPINUP 7
#define CLAW_SERVOPIN 5
#define SLIDER_SERVOPIN 4
#define ALIGNMENT_SERVOPIN 3

/***************************************************
* Include Files
***************************************************/
#include <ND/Xbox/RXDATA.h>
#include <ND/Actuation/Motors.h>
#include <ND/Xbee/ND_XBEE.h>
#include <ND/Sensors/Sensor.h>
#include <Servo.h>

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
// Center Specific
Servo claw_servo; Servo slider_servo; Servo alignment_servo;
int clawpos=90, sliderpos=90, alignmentpos=100;
int alignmentup = 0;
boolean exitloop = false;
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
uint8_t CLAW_OPEN;
uint8_t CLAW_CLOSED;
uint8_t UP_ALIGNMENT;
uint8_t DOWN_ALIGNMENT;
uint8_t SLIDER_MIN;
uint8_t SLIDER_MAX;


/***************************************************
* setup
***************************************************/
void setup() {
  // Define Center Specific Variables
  if (CONTROLLER_ADDRESS==0x9012){ // 5 Hours
    CLAW_OPEN  =70;
    CLAW_CLOSED = 120;
    UP_ALIGNMENT = 20;
    DOWN_ALIGNMENT = 120;
    SLIDER_MIN = 60; //Extended
    SLIDER_MAX = 100; //Contracted
  }else if (CONTROLLER_ADDRESS==0x9011){ // KURT
    CLAW_OPEN = 120;
    CLAW_CLOSED = 80;
    UP_ALIGNMENT = 40;
    DOWN_ALIGNMENT = 120;
    SLIDER_MIN = 40; //Extended
    SLIDER_MAX = 85; //Contracted
  }
  
  //Center Specific
  pinMode(LIMITPINDOWN, INPUT_PULLUP);
  pinMode(LIMITPINUP, INPUT_PULLUP);
  claw_servo.attach(CLAW_SERVOPIN); slider_servo.attach(SLIDER_SERVOPIN); alignment_servo.attach(ALIGNMENT_SERVOPIN);
  claw_servo.write(clawpos);
  slider_servo.write(sliderpos);
  alignment_servo.write(alignmentpos);
  alignmentup = UP_ALIGNMENT;
  //Finds upper calibration point for alignment mechanism
//  alignmentup = alignmentpos;
//  while(digitalRead(LIMITPIN)){
//    alignmentup--;
//    alignment_servo.write(alignmentup);
//    delay(15);
//  }
//  alignmentpos = alignmentup;
  
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
  Serial.println("START...");
  delay(3000); // delay 2 seconds
}
int count;
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
//      Serial.print("Succes");
      rxdata.debugprint(&Serial);
#endif
    }
#ifdef DEBUG
//Serial.print(ndbee.Address16());
//Serial.print(" ADDR\t");
//Serial.println("gotsomething");
#endif
  } else {
      rxdata.NoPacket = true;
      if (rxdata.TimeOut()) rxdata.ZERO(); // after so many cycles of no packets cut the power...
  }  
  
  
/************************************************************
* PART 3. Select Your Mode
************************************************************/
  //switch (rxdata.get(MODE,2))
  switch(0)
  {
    case 0:  //main drive mode
      //Serial.println("case 0");
      // Claw Servo Control (A opens and closes sequentially)
      if(rxdata.ButtonPress(Y)){
        Serial.println("Y press");
        if(clawpos == CLAW_OPEN){
          clawpos = CLAW_CLOSED;
        } else {
          clawpos = CLAW_OPEN;
        }
        claw_servo.write(clawpos);
        delay(15);
      }
     //Control Slider Servo 
      if(rxdata.ButtonPress(X)){
        Serial.println("X press");
        if(sliderpos == SLIDER_MIN){
          while(sliderpos!=SLIDER_MAX){
            sliderpos++;
            slider_servo.write(sliderpos);
            delay(15);
          }
          
        } else {
          while(sliderpos!=SLIDER_MIN){
            sliderpos--;
            slider_servo.write(sliderpos);
            delay(15);
          }
//          delay(100);
//          //Pull up the alignment piece
//          if(alignmentpos != alignmentup) {
//            while(!exitloop){
//              alignmentpos--;
//              alignment_servo.write(alignmentpos);
//              Serial.println(alignmentpos);
//              delay(15);
//            
//              if(!digitalRead(LIMITPINUP)){ //HIGH is up. LOW is down.
//                Serial.println("Up Limit Switch Pressed");
//                exitloop = true;
//              }
//              if(alignmentpos==UP_ALIGNMENT){//if position equals reached
//                exitloop = true;
//              }
//            }
//            exitloop = false;
//            alignmentup = alignmentpos;
//        }
        }
      }
      
    //Control Alignment Servo
    //NEW Control with 2 limit switched and 2 pins (6 & 7)
    if(rxdata.ButtonPress(A)){
        Serial.println("A press");
        if(alignmentpos != alignmentup) {
            while(!exitloop){
              alignmentpos--;
              alignment_servo.write(alignmentpos);
              Serial.println(alignmentpos);
              delay(15);
            
              if(!digitalRead(LIMITPINUP)){ //HIGH is up. LOW is down.
                Serial.println("Up Limit Switch Pressed");
                exitloop = true;
              }
              if(alignmentpos<=UP_ALIGNMENT){//if position equals reached
                exitloop = true;
              }
            }
            exitloop = false;
            alignmentup = alignmentpos;
        } else {
          while(!exitloop){
            alignmentpos++;
            alignment_servo.write(alignmentpos);
            // Check to see we're moving the right direction
            Serial.println(alignmentpos);
            //
            delay(15);
            //Check Limit Switch
            if(!digitalRead(LIMITPINDOWN)){ //HIGH is up. LOW is down.
              Serial.println("Down Limit Switch Pressed");
              exitloop = true;
            }
            if(alignmentpos>=DOWN_ALIGNMENT){//if position equals reached
              exitloop = true;
            }
          }
          exitloop = false;
        }
        
      }
    
    
    
    //New Control with 2 limit switches Top and Bottom on same pin
//    Serial.println(alignmentpos);
//    if(rxdata.ButtonPress(A)){
//        Serial.println("A press");
//        if(alignmentpos != alignmentup) {//After this sequence the arrow will be in the up alignment position...which is the limit switch.
//            //Get off the bottom limit switch
//            for(int i=0; i<=20;i++){
//              alignmentpos--;
//              alignment_servo.write(alignmentpos);
//            }
//            //Go up 
//            while((alignmentpos != alignmentup)||(!digitalRead(LIMITPIN))){
//              alignmentpos--;
//              alignment_servo.write(alignmentpos);
//              delay(15);
//            }
//            //Depending on which stop was met first set the new up position
//            if(!digitalRead(LIMITPIN)){
//              while(digitalRead(LIMITPIN)){
//                alignmentpos--;
//                alignment_servo.write(alignmentpos);
//                delay(15);
//                alignmentup = alignmentpos;
//              }
//            } else {
//              alignmentup = alignmentpos;
//            }
//            
//        } else {
//          //Get off the top limit switch
//          for(int i=0; i<=20; i++){
//              alignmentpos++;
//              alignment_servo.write(alignmentpos);
//            }
//          while(!exitloop){
//            alignmentpos++;
//            alignment_servo.write(alignmentpos);
//            // Check to see we're moving the right direction
//            Serial.println(alignmentpos);
//            //
//            delay(15);
//            //Check Limit Switch
//            if(!digitalRead(LIMITPIN)){ //HIGH is up. LOW is down.
//              Serial.println("Limit Switch Pressed");
//              exitloop = true;
//            }
//            if(alignmentpos==DOWN_ALIGNMENT){//if position equals reached
//              exitloop = true;
//            }
//          }
//          exitloop = false;
//        }
//        
//      }
    
    //Old Control wityh one limit switch on Bottom
//    if(rxdata.ButtonPress(A)){
//        Serial.println("A press");
//        if(alignmentpos != UP_ALIGNMENT) {
//            alignmentpos = UP_ALIGNMENT;
//            alignment_servo.write(alignmentpos);
//            delay(15);
//        } else {
//          while(!exitloop){
//            alignmentpos++;
//            alignment_servo.write(alignmentpos);
//            // Check to see we're moving the right direction
//            Serial.println(alignmentpos);
//            //
//            delay(15);
//            //Check Limit Switch
//            if(!digitalRead(LIMITPIN)){ //HIGH is up. LOW is down.
//              Serial.println("Limit Switch Pressed");
//              exitloop = true;
//            }
//            if(alignmentpos==DOWN_ALIGNMENT){//if position equals reached
//              exitloop = true;
//            }
//          }
//          exitloop = false;
//        }
//        
//      }

    
        //Serial.println("Case 0");
        Wheels.steering_gain = STEERING_GAIN;
        Wheels.throttle_gain = THROTTLE_GAIN;
     if (rxdata.get(R1)) { // spin fast right (CW looking at ground)
       if (!tackle_sensor.tackled()) { 
         //Serial.println("Spin Hard Right");
         Wheels.Move(SPIN_HARD,-SPIN_HARD);
       } else{ 
         Wheels.Stop();
       }
     } 
     else if (rxdata.get(L1)) {
         if (!tackle_sensor.tackled()) { 
         Wheels.Move(-SPIN_HARD,SPIN_HARD);
         //Serial.println("Spin Hard Left");
         } else{
         Wheels.Stop();
         }
     }
    else if (rxdata.getTrigger(R2)) {
      if (!tackle_sensor.tackled()) { 
        Wheels.TurnRight(rxdata.getJoystick(LY),rxdata.getTrigger(R2));
        //Serial.println("Turn Right");
      } 
      else{
        Wheels.Stop();
      }
    } 
    else if (rxdata.getTrigger(L2)) {
      if (!tackle_sensor.tackled()) { 
        Wheels.TurnLeft(rxdata.getJoystick(LY),rxdata.getTrigger(L2));
        //Serial.println("Turn Left");
      } 
      else{
        Wheels.Stop();
      }
    } 
    else{ // if nothing pressed use controller
      control(rxdata.getJoystick(LY), rxdata.getJoystick(RX),rxdata.getJoystick(RX));  
      //Serial.println("Nothing Pressed.");
  }
        break;
        
        case 1:
        Serial.println("Case 1");
        Wheels.steering_gain = STEERING_GAIN2;
        Wheels.throttle_gain = THROTTLE_GAIN2;
     if (rxdata.get(R1)) { // spin fast right (CW looking at ground)
       if (!tackle_sensor.tackled()) { 
         Serial.println("Spin Hard Right");
         Wheels.Move(SPIN_HARD,-SPIN_HARD);
       } else{ 
         Wheels.Stop();
       }
     } else if (rxdata.get(L1)) {
         if (!tackle_sensor.tackled()) { 
         Wheels.Move(-SPIN_HARD,SPIN_HARD);
         Serial.println("Spin Hard Left");
         } else{
         Wheels.Stop();
         }
     }
    else if (rxdata.getTrigger(R2)) {
      if (!tackle_sensor.tackled()) { 
        Wheels.TurnRight(rxdata.getJoystick(LY),rxdata.getTrigger(R2));
      } 
      else{
        Wheels.Stop();
      }
    } 
    else if (rxdata.getTrigger(L2)) {
      if (!tackle_sensor.tackled()) { 
        Wheels.TurnLeft(rxdata.getJoystick(LY),rxdata.getTrigger(L2));
      } 
      else{
        Wheels.Stop();
      }
    } 
    else{ // if nothing pressed use controller
      control(rxdata.getJoystick(LY), rxdata.getJoystick(RX),rxdata.getJoystick(RX));  
  }
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
        
        case 3:// CALIBRATION MODE:
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
      //Serial.print("Left: "); Serial.print(vals1[0]); Serial.print(" \t");
      //Serial.print("Right: "); Serial.println(vals2[0]);
#endif
    } else { // stop the motors if tackled
      Wheels.Stop();
#ifdef DEBUG
      Serial.println("TACKLED");
#endif
    }
}
