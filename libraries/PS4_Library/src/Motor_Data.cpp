/*	Motor_Data.cpp
* ----------------------------------
* this contains the functions for the motor_offsets class ... keep in mind that
* these offsets get saved in permanent memory (EEPROM) so that they can be used after reset.
* (see EEPROM for addressing)
* ----------------------------------------------
* Author: Joseph Rudy
* Date: 3/14/2013
*
*/
#include "Motor_Data.h"
using namespace ND;
off_type motor_offsets::dir = 0;
off_type motor_offsets::saved_dir = 0;
// Constructor : sets 2 offsets equal to 0
motor_offsets::motor_offsets() 
{ 
	zero();
	saved_fwd = 0;
	saved_rev = 0;
}
// Properties
// Property : forward
// handles the forward offset
// make sure that fwd is always positive or zero
void motor_offsets::forward(int val)
{
	if (fwd <= -val) fwd = 0;
	else if (fwd > (MAX_OFFSET-val)) fwd = MAX_OFFSET;
	else fwd += val;
}
off_type motor_offsets::forward() {return fwd;}
// Property : reverse
// handles the reverse offset
void motor_offsets::reverse(int val)
{
	if (rev <= -val) rev = 0;
	else if (rev > (MAX_OFFSET-val)) rev = MAX_OFFSET;
	else rev += val;
}
off_type motor_offsets::reverse() { return rev;}
/////////////////////////////////////////////////////////////////
// Methods
/////////////////////////////////////////////////////////////////
/* 	Method 	: getMotor
*	Purpose	:	gets which motor that the offset is applied to based on the bit number
*   			these bit numbers are all found in the header file as #define's (eg. OFFSET_FWD1_BIT)
*	Inputs	:	uint8_t
* 	Returns	:	bool (1 if left, 0 if right)
*/
bool motor_offsets::getMotor(uint8_t offsetbit)
{
	if (offsetbit < 0 || offsetbit > 7) return false; // check to make sure it is appropriate bit value
	// bitshift a 1 into the proper bit position (based on the inputted offsetbit) 
	// and then & it with the byte and check to see if it is greater than 0;
	if ((dir & (1 << offsetbit)) > 0) return true;
	else return false;
}
/* 	Method	:	setMotor
*	Purpose	:	sets the motor which the offset is applied to
*				based on the inputted value (left = 1, right = 0)
*	Inputs	: 	uint8_t and bool
*	returns	:	none
*/
void motor_offsets::setMotor(uint8_t offsetbit, bool value)
{
	unsigned char i,j;
	unsigned char clear_bits = 0, pow;
	if (offsetbit < 0 || offsetbit > 7) return; // check to make sure it is appropriate bit value
	// clear the bit that is the offsetbit in dir .. this setups the clearbits variable
	for (i = 0; i < 8; i++) {
		if (offsetbit != i) {
			pow = 1;
			for (j = 0; j < i; j++)  pow *= 2; // basically a power function
			clear_bits += pow;
		}
	}
	dir = (dir & clear_bits) | (value << offsetbit);
}
// motor_number = 0-4, Direction = FORWARD or REVERSE, leftorright = MOTOR_LEFT or MOTOR_RIGHT, 
void motor_offsets::setMotor(uint8_t motor_number, uint8_t direction, bool leftorright)
{
	bool offmotor;
	uint8_t offsetbit = 2*motor_number - direction;
	setMotor(offsetbit,leftorright);
}
// Method : zero
// zeros both of the offsets
void motor_offsets::zero() 
{ 
	rev = 0; fwd = 0;
}
// Method	:	Switch
// Purpose	:	used to switch the offset motor to left if it is right or to right if it is left
// Inputs	:	uint8_t
// Returns	:	none
/*void motor_offsets::Switch(uint8_t motor_number, uint8_t direction)
{
	bool offmotor;
	uint8_t offsetbit = 2*motor_number - direction;
	offmotor = getMotor(offsetbit); // get the offset motor [ either LEFT or RIGHT]
	if (offmotor == MOTOR_LEFT) { // if left switch to right
		setMotor(offsetbit, MOTOR_RIGHT);
	} else if (offmotor == MOTOR_RIGHT) { // if right switch to left
		setMotor(offsetbit, MOTOR_LEFT);
	}
	
}*/

void motor_offsets::apply(int &lmotor, int &rmotor, uint8_t motor_number)
{
    return;
}

// Method : read
// reads the values from the EEPROM and puts them into the saved values
// and then they become the actual ones

void motor_offsets::read(int fwd_addr, int rev_addr) 
{
    return;
}
    
/*
	// read in the appropriate offsets
	if (fwd_addr >= ADDR_FORWARD_OFFSET && fwd_addr <= ADDR_REVERSE_OFFSET3) saved_fwd = EEPROM.read(fwd_addr);
    if (rev_addr >= ADDR_FORWARD_OFFSET && rev_addr <= ADDR_REVERSE_OFFSET3) saved_rev = strafe > 0; 	strafe -= DEAD_ZONE;
    else if (strafe < 0) strafe += DEAD_ZONE;
	if (rotate > 0)		rotate -= DEAD_ZONE;	else if (rotate < 0)	rotate += DEAD_ZONE;
	
  // Get the motor_values with gains on each
 motorvals[0] = (T_GAIN*throttle + S_GAIN*strafe + R_GAIN*rotate)/100; //front left motor
 motorvals[1] = (T_GAIN*throttle - S_GAIN*strafe - R_GAIN*rotate)/100; //front right motor
 motorvals[2] = (T_GAIN*throttle - S_GAIN*strafe + R_GAIN*rotate)/100; //back left motor
 motorvals[3] = (T_GAIN*throttle + S_GAIN*strafe - R_GAIN*rotate)/100; //back right motor
  
  // Find the maximum motor value to scale to this motor
  for(int i=0; i<4; i++){
    if(abs(motorvals[i])>maxval){
         maxval = motorvals[i];
    }
  }
  
  if(maxval > 255){ //positive scaling
    ratio = 255/maxval;
  } else if(maxval < -255){ //negative scaling
    ratio = -255/maxval;
  } else{ // no scaling
	ratio = 1; 
  }
  for (int i = 0; i < 4; i++) motorvals[i] *=ratio;
  
  Move(motorvals[0],motorvals[1],motorvals[2],motorvals[3]);
}
 */
