/*	Calibration.h
*	-------------------------------------
*	Used for constants in calibration of a motor
*	purpose of calibration: the motors will not spin at the same speeds so it
*	is imperative that you bump one down in order to go straight
* 	-------------------------------------
*	Author	:	Joe Rudy
*	Date	:	3/22/2013
*/
#ifndef CALIBRATION_H
#define CALIBRATION_H

// These values will get passed into the what statement of a Calibrate function to determine what to calibrate
#define CALIBRATE_FORWARD 		0 // used in 'Calibrate' for the what variable to calibrate the forward offset of the motor
#define CALIBRATE_REVERSE 		1 // used in 'Calibrate' for the what variable to calibrate the reverse offset of the motor
#define CALIBRATE_SWITCH  		2 // used in 'Calibrate' for the what variable to switch the motor that the offset is applied to (left or right)
#define CALIBRATE_MOTOR_FORWARD 3 // used in 'Calibrate' for what variable to set left or right motor as offset motor going forward
#define CALIBRATE_MOTOR_REVERSE	4 // used in 'Calibrate' for what variable to set left or right motor as offset motor going in reverse
#define CALIBRATE_SAVE_OFFSETS	5 // used in 'Calibrate' to save the offsets
#define CALIBRATE_FORWARD2		6 // used in 'Calibrate' for what variable to calibrate the forward offset of the 2nd motor
#define CALIBRATE_REVERSE2		7 // used in 'Calibrate' for what variable to calibrate the reverse offset of the 2nd motor
#define CALIBRATE_FORWARD3		8 // used in 'Calibrate' for what variable to calibrate the forward offset of the 3rd motor
#define CALIBRATE_REVERSE3		9 // used in 'Calibrate' for what variable to calibrate the reverse offset of the 3rd motor
#define CALIBRATE_ZERO			10 // used in 'Calibrate’ to zero the offsets
#define CALIBRATE_RESET			11 // used in 'Calibrate’ to reset the offsets to the saved values
// QB Calibration
#define CALIBRATE_READ			12 // used in 'Calibrate' to read from the eeprom
#define CALIBRATE_SAVE			13 // used in 'Calibrate' to save the passing distances
#define CALIBRATE_PASS_DOWN		14 // used in 'Calibrate' to calibrate the passing distances ...decrease the inputted one by 1
#define CALIBRATE_PASS_UP		15 // used in 'Calibrate' to calibrate the passing distances ...increase the inputted one by 1
namespace ND
{
	// use this in your main code to condense code
	// this structure will be used to pass 
	typedef struct calibrate_data
	{
		unsigned char fwd_rev;
		unsigned char left_right;
		calibrate_data() 
		{ 
			fwd_rev = CALIBRATE_FORWARD;
			left_right = CALIBRATE_MOTOR_FORWARD;
		}
	} calibration_data;
};
#endif