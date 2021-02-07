/* Motors.cpp
* ------------------------------------
* These are the functions for the motors class.
* ------------------------------------
* Author: Joe Rudy
* Date: 3/13/2013
*/
#include "Motors.h"
using namespace ND;

/* Title	:	Constructor
* Purpose	:	initializes all of the variables and sets the servos
*/
Motors::Motors(Servo *ser1, Servo * ser2, int inputTgain, int inputSgain) {Motors(ser1,ser2,inputTgain,inputSgain,0);}
Motors::Motors(Servo *ser1, Servo * ser2, int inputTgain, int inputSgain,int zerooff)
{
	s1 = ser1; s2 = ser2;
	unsigned char i;
	for (i = 0; i < 3; i++) {
		val1[i] = 0;
		val2[i] = 0;
	}
	// Initialize offset
	offset.read(ADDR_FORWARD_OFFSET, ADDR_REVERSE_OFFSET);
	cdata = calibration_data();
        left_zero_offset = 0;
        right_zero_offset = 0;
	steering_gain = inputSgain;
	throttle_gain = inputTgain;
	zero_offset = zerooff;
	Stop();
        moving = false;
}
/* Title	:	Move
* Purpose	:	sets all the servos (in this case 2) to the inputted value... keeps in mind the offset
*				in order for consistent movement.  The inputted speed should be a value between -928 and 928.
*				these 2 values are found from the minimum (544) and maximum (2400) pulse width difference divided by 2 and rounded
* Inputs	:	int spd
* Returns	:	none
*/
void Motors::Move(int spd) { Move(spd,spd);}
/* Title	:	Move
* Purpose	:	same as the other move but allows for 2 different motor speeds of the different motors
*/
void Motors::Move(int lmotor, int rmotor)
{
        // Apply offsets
        lmotor += left_zero_offset;
        rmotor += right_zero_offset;
	// make sure that the speeds are constrained within the max and min input
	lmotor = constrain(lmotor,-MAX_INPUT,MAX_INPUT);
	rmotor = constrain(rmotor,-MAX_INPUT,MAX_INPUT);

	save(lmotor,rmotor); // saves the motor values without the constraint of the offsets

        /*
	if (lmotor != 0 || rmotor != 0) offset.apply(lmotor,rmotor,MOTOR1); // apply offsets
	else { Stop(); return;} // stop the motors if they are both zero
        */

	// use the appropriate motor offset
	int newspd1 = map(lmotor,-MAX_INPUT,MAX_INPUT,MIN_OUTPUT,MAX_OUTPUT); // MIN_PULSE_WIDTH = 544, MAX_PULSE_WIDTH = 2400
	int newspd2 = map(rmotor,-MAX_INPUT,MAX_INPUT,MIN_OUTPUT,MAX_OUTPUT);
	if (rmotor != 0 || lmotor != 0) moving = true;
	else moving = false;
	if (newspd1 + zero_offset < MAX_OUTPUT && newspd1 + zero_offset >MIN_OUTPUT) newspd1+=zero_offset;
	if (newspd2 + zero_offset < MAX_OUTPUT && newspd2 + zero_offset >MIN_OUTPUT) newspd2+=zero_offset;
		// adding the offset undoes whatever
	s1->write(newspd1);
	s2->write(newspd2);
}
/* Title	:	Stop
*  Purpose	:	stops the 2 motors of the class
*  Inputs	:	none
*  Returns	:	none
*/
void Motors::Stop()
{
        Move(0,0);
	save(0,0);
	moving = false;
}
/* Title	:	Save
*  Purpose	:	shifts all the values down and saves the current values inputted
*  Inputs	:	int spd1, int spd2
*  Returns	:	none
*/
void Motors::save(int spd1, int spd2)
{
	val1[2] = val1[1]; val1[1] = val1[0]; val1[0] = spd1;
	val2[2] = val2[1]; val2[1] = val2[0]; val2[0] = spd2;
}
/* Title	:	TurnRight
*  Purpose	:	turns right when the left trigger is pressed, radius is determined by throttle
*/

void Motors::TurnRight(signed char throttle, unsigned char trigger)
{
	int lmotor, rmotor, sign = 1; // lmotor is w1 greater
	if (abs(throttle) < DEAD_ZONE) throttle = 0;
	if (throttle > 0) throttle -= DEAD_ZONE; else if (throttle < 0) throttle+= DEAD_ZONE;
	int n = trigger/8; // n is multplied by 10
	if (throttle > 0) sign = 1;
	else if (throttle < 0) sign = -1;
	lmotor = sign*50+throttle;
	if (n != 0) {
		rmotor = (lmotor*(n-1))/(10*n);
		Move(lmotor,rmotor);
	} else if (n == 0) {
		Move(lmotor,0);
	}
}

void Motors::TurnLeft(signed char throttle, unsigned char trigger)
{
	int lmotor, rmotor, sign=1; // lmotor is w1 greater
	if (abs(throttle) < DEAD_ZONE) throttle = 0;
	if (throttle > 0) throttle -= DEAD_ZONE; else if (throttle < 0) throttle+= DEAD_ZONE;
	// between [-102 102]
	int n = trigger/8; // was 5
	if (throttle > 0) sign = 1;
	else if (throttle < 0) sign = -1;
	rmotor = sign*50+throttle;
	if (n != 0) {
		lmotor = (rmotor*(n-1))/(10*n);
		Move(lmotor,rmotor);
	} else if (n == 0) {
		Move(0,rmotor);
	}
}
/* Title	:	Controller
*  Purpose	:	this is the controller based on the inputted throttle value and steering value 
*				the throttle & steering value is between (0 to 255) so adjust accordingly
* Inputs	:	signed char throttle, signed char steering, unsigned char [0 255]
* Returns	:	none
*/
void Motors::Controller(signed char throttle, signed char steering)
{
    if(throttle == 0) throttle--;
    
	// Normalize so the function works with PS4 controllers. Shift by 128
	throttle = throttle - 128;
	steering = steering - 128;
	
	long max_motor = 0, lmotor = 0, rmotor = 0;
	// only add the side onto the left motor if side2side is > 0, otherwise subtract from right motor because negative value
	long rside = 0, lside = 0;
    //Check dead zone calculation first
	if (abs(throttle) < DEAD_ZONE) throttle = 0;
	if (abs(steering) < DEAD_ZONE) steering = 0;
    // adjust the values so that they fall between [-127 127];
	if (throttle == -128) throttle++; if (steering == -128) steering++;
	// further adjust the values so that they fall between [-127+DEAD_ZONE 127-DEAD_ZONE] ... gains should be adjusted appropriately
	if (throttle > 0) throttle -= DEAD_ZONE; else if (throttle < 0) throttle+= DEAD_ZONE;
	if (steering > 0) steering -= DEAD_ZONE; else if (steering < 0) steering += DEAD_ZONE;
	// Calculate Motor Values... divide by 100 to even out the factor in the gain
	lmotor = ((long)throttle_gain*(long)throttle + (long)steering_gain*(long)steering)/100;
	rmotor = ((long)throttle_gain*(long)throttle - (long)steering_gain*(long)steering)/100;
	
	// normalize -- find the maximum value first
	if ( abs(lmotor) > abs(rmotor)) max_motor = abs(lmotor);
	else max_motor = abs(rmotor);
	
	// Scale it
	if (max_motor > MAX_INPUT) {
		lmotor = (lmotor*MAX_INPUT)/max_motor;
		rmotor = (rmotor*MAX_INPUT)/max_motor;
	} // else no scaling
    Move(lmotor,rmotor);
}
/* Title	:	Controller_v2
*  Purpose	:	this is the controller based on the inputted throttle value and steering value 
*				the throttle & steering value is between (-128 to 127) so adjust accordingly
*				version 2 controller adds a quadratic and cubic throttling compared to the linear in version 1
* Inputs	:	signed char throttle, signed char steering, unsigned char [0 255]
* Returns	:	none
*/
long Motors::Controller_v2(signed char throttle, signed char steering)
{
	long max_motor = 0, lmotor = 0, rmotor = 0;
	long throttle_value = 0;
	long t = throttle;
	int sign = 1;
	// only add the side onto the left motor if side2side is > 0, otherwise subtract from right motor because negative value
    //Check dead zone calculation first
	if (abs(t) < DEAD_ZONE) t = 0;
	if (abs(steering) < DEAD_ZONE) steering = 0;
    // adjust the values so that they fall between [-127 127];
	if (t < -127) t = -127; if (steering < -127) steering = -127;
	if (t >  127) t = 127;  if (steering >  127)  steering = 127;
	// find the sign of throttle and make throttle positive
	if (t < 0) { t = -t; sign = -1;}
	// THROTTLE VALUE
	if (t != 0) {
		if (t <= QUADRATIC_MAX_INPUT) // quadratic zone
		{
			t -= DEAD_ZONE; // throttle falls between [0 102]
			throttle_value = (T_KT*t)/100;
			//throttle_value = (A1*(t*t) + B1*(t) + C1)/1000;
		} else {
			t -= QUADRATIC_MAX_INPUT;
			throttle_value = (T_C1*(t*t)/100 + T_C2*(t))/100 + Z_SHIFT/100;
		}
	} else throttle_value = 0;
	
	
	// STEERING adjustment
	if (steering > 0) steering -= DEAD_ZONE; else if (steering < 0) steering += DEAD_ZONE;
	// Calculate Motor Values... divide by 100 to even out the factor in the gain
	lmotor = (long)sign*throttle_value + ((long)steering_gain*(long)steering)/100;
	rmotor = (long)sign*throttle_value - ((long)steering_gain*(long)steering)/100;
	
	// normalize -- find the maximum value first
	if ( abs(lmotor) > abs(rmotor)) max_motor = abs(lmotor);
	else max_motor = abs(rmotor);
	
	// Scale it
	if (max_motor > MAX_INPUT) {
		lmotor = (lmotor*MAX_INPUT)/max_motor;
		rmotor = (rmotor*MAX_INPUT)/max_motor;
	} // else no scaling
    Move(lmotor,rmotor);
	return throttle_value;
}
/*	Title	:	change_gains
*	Purpose	:	allows you to calibrate gains on the fly
*
*
*/
void Motors::change_gains(int whichone, int delta)
{ 
	if (whichone == 1) throttle_gain+=delta;
	else if (whichone == 2) steering_gain+=delta;
	if (throttle_gain < 0) throttle_gain = 0;
	if (throttle_gain > 250) throttle_gain = 250;
	if (steering_gain < 0) steering_gain = 0;
	if (steering_gain > 250) steering_gain = 250;
}

/* Title	:	Calibrate
*  Purpose	:	this function is used to calibrate the offsets zero offsets for the left and right
*                       motors.
*
*  Inputs	:	int Motor number, int amount of change
*  Returns	:	none
*/
void Motors::Calibrate(int Motor, int change) {
// leftMotor = 0
// rightMotor = 1
    if (Motor == 0) {
        left_zero_offset += change;
    }
    else if (Motor == 1) {
        right_zero_offset += change;
    }
}

void Motors::Reset_Calibration() {
    left_zero_offset = 0;
    right_zero_offset = 0;
}

int Motors::get_zero_offset(int Motor) {
// leftMotor = 0
// rightMotor = 1
    if (Motor == 0) {
        return left_zero_offset;
    }
    else if (Motor == 1) {
        return right_zero_offset;
    }
    else {
        return 0;
    }
}
