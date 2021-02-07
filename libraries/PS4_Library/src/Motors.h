/* Motors.h
* ------------------------------------
* This class handles two motors in a drive train that are controlled by servo motors.
* It inherits the abstract class 'Actuator' found in Actuator.h  ... which means it will
* define these methods: Move, Stop, Controller, Calibrate.
* -------------------------------------
* Gains: there are 2 gains used in the controller.  These are the throttle gain and steering
* gain and as you can imagine, it is really just a proportional gain that basically weights the
* throttle (forward and backward movement) with the steering (side-to-side movement).
* Therefore a low steering gain will mean that you cannot make sharp turns... whereas a high steering gain
* sends you spinning in circles :)
* --------------------------------------
* Moving the motors: In order to use the Move() function or understand the Controller function, the minimum value for
* inputting into the move function is -928 [full reverse], 0 [stop], 928 [full forward] ... these values are calculated
* from the difference between the minimum and maximum pulse width found in the Servo class
* --------------------------------------
* Offsets are applied due to the fact that there may be a difference in motor speeds. All this does is subtract off the offset
* value from the appropriately selected motor (left or right) depending on the value of MOTOR_OFFSET
* --------------------------------------
* Author: Joe Rudy
* Date: 3/12/2013
*/
#ifndef MOTORS_H
#define MOTORS_H
#include <Arduino.h>

#include "EEPROM.h"
#include "Actuator.h"
#include "Motor_Data.h" // used to calibrate the motors, contains the actual offsets and how they are managed
#include "Calibration.h" // used to calibrate the motors
#include "Servo.h"

#define ZERO_MICROSECONDS 1472 // use this when you write in microseconds
#define ZERO_ANGLE 90 // use this zero when using the 'write' function for servos
// Maximum input into the motors
#define MAX_INPUT 255
#define MAX_OUTPUT 180 // max servo value
#define MIN_OUTPUT 0 // min servo value

// Gains & Motor Controller values
// change these values to adjust throttle and steering gain ... Note: I would not touch the throttle gain
/* Adjusting Gains ....
* The gains were determined by trying to map a range from the DEAD_ZONE = 0 [output] to FULL_THROTTLE = 948 [output]...
* see documentation for this
*/
#ifndef DEAD_ZONE
#define DEAD_ZONE 25 // determines the dead zone values for throttle and steering
#endif
// These are good values for initial gains:
//#define DEFAULT_STEERING_GAIN 40
//#define DEFAULT_THROTTLE_GAIN 250 
// Quadratic and Cubic Zones
#define QUADRATIC_MAX_INPUT 110
#define T_C1 45L
#define T_C2 136L
#define T_KT 136L
#define Z_SHIFT 102L


namespace ND
{	
	class Motors: public Actuator
	{
		public:
			Motors(Servo *ser1, Servo *ser2, int inputTgain, int inputSgain);
			Motors(Servo *ser1, Servo *ser2, int inputTgain, int inputSgain,int zeroposition);
			virtual void Move(int spd); // moves all motors to the input spd
			virtual void Stop(); // stops both motors
			virtual void Controller(signed char throttle, signed char steering);
			long Controller_v2(signed char throttle, signed char steering);
			void TurnRight(signed char throttle, unsigned char trigger);
			void TurnLeft(signed char throttle, unsigned char trigger);
			virtual void Calibrate(int val, int what);
                        void Reset_Calibration();
			motor_offsets offs() { return offset;} // used for debugging purposes... take out when you are actually doing code
			calibration_data cdata;
			void Move(int spd1, int spd2); // moves the 2 different motors to the 2 inputted speeds
			bool moving; // tells if the motors are moving or stopped
			void change_gains(int whichone, int delta);
			int gains(int index) { if (index == 1) return throttle_gain; if (index == 2) return steering_gain;}
			/* Used for debugging */
			int throttle_gain, steering_gain;
			int * vals(int i) { if (i==1) return val1; else return val2;} // used for debugging purposes
                        int get_zero_offset(int Motor); // returns left_zero_offset or right_zero_offset
		protected:
			// use val1 for left motor and val2 for right motor
			int val1[3]; // val1[0] = current value,  val1[1] = previous value, val1[2] = 2 time steps ago value
			// these values are quantified between the -MAX_INPUT (-928) and MAX_INPUT (928)
			int val2[3];
			Servo *s1, *s2;  // s1 = left motor, s2 = right motor
			motor_offsets offset; // motor offsets for the appropriate motor
		private:
			void save(int spd1, int spd2); // saves the values that were currently inputted
			int zero_offset; // zero value for the motors
                        int left_zero_offset; // zero value for left motor
                        int right_zero_offset; // zero value for right motor

	};
};
#endif
