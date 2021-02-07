/* Motor_Data.h
*  --------------------------------------
*  This header file contains the data structures used in Motors.h and Mecanum.h
*  These structures are 'motor_offsets' and 'motor_gains'.  This file
*  also contains the data that may be specific to your robot so you will want to switch 
*  all the default offsets and the
* ----------------------------------------
*	What happens is that the class reads the offsets from memory (EEPROM) and then
*	it will save those offsets as the real offsets
*/
#include <ND/Memory/EEPROM.h>
#include <inttypes.h>
#include <Arduino.h>

// These determine which motor ... left or right that the offset is applied to
// you want these to be the faster of the 2 motors so that the motor speed will be reduced by the offset
// These define the bits in the 'dir' variable... keep in mind there are 8 bits so you can have 4 motor pairs
// 1 = left, 0 = right
#define OFFSET_FWD1_BIT 0 // Forward of motor 1 pair (driving wheels & mecanum wheels)
#define OFFSET_REV1_BIT 1 // Reverse of motor 1 pair (driving wheels & mecanum wheels)
#define OFFSET_FWD2_BIT 2 // Forward of motor 2 pair (mecanum wheels)
#define OFFSET_REV2_BIT 3 // Reverse of motor 2 pair (mecanum wheels)
#define OFFSET_FWD3_BIT 4 // Forward	"	"	3	"
#define OFFSET_REV3_BIT 5 // Reverse	"	"	3	"
#define OFFSET_FWD4_BIT 6 // offset for the throwing wheels
#define OFFSET_REV4_BIT 7
#define FORWARD			2 // you can use these in a formula to get the offset bit (2*motornum - FORWARD) = bitval
#define REVERSE			1 // same as FORWARD except (2*motornum - REVERSE) = bitval)

#define MOTOR_LEFT 		1 // if the bit is 1 then apply offset to the left motor
#define MOTOR_RIGHT		0 // if the bit is 0 then apply the offset to the right motor

#define MOTOR1			1 // used in the apply function so only one term is passed through
#define MOTOR2			2 // used for mecanum wheels
#define MOTOR3			3 // used for a third motor if you want
#define MOTOR4			4 // used for throwing wheels... do not use this offset in this class

#define MAX_OFFSET 50
typedef uint8_t off_type;
namespace ND
{
// use this structure for all your offset needs
	class motor_offsets
	{
		private: 	
			off_type fwd; // forward offset
			off_type rev; // reverse offset
			off_type saved_fwd; // this is the fwd that can be found in the EEPROM
			off_type saved_rev; // this is the rev that can be found in the EEPROM
			// The following are static because they are shared between all the instances ... 
			// which means you can only have 4 motor pairs on one board with offsets (that's enough I think,
			// but you can add more variables if you need more offsets)
			static off_type dir; // these determine the motor that the offset is applied to (left or right)
			static off_type saved_dir; // this is the direction saved in the EEPROM
					
			// METHODS
			bool getMotor(uint8_t offsetbit);
			void setMotor(uint8_t offsetbit, bool value);
		public: 
			motor_offsets();
			/////////////////////////////////////////////////////////////
			// Properties
			/////////////////////////////////////////////////////////////
			// Property : forward
			// handles the forward offset
			// make sure that fwd is always positive or zero
			void forward(int val);
			off_type forward();
			// Property : reverse
			// handles the reverse offset
			// make sure that rev is always positive or zero
			void reverse(int val);
			off_type reverse();
			// Property : direct (read-only)
			// which motor is the offset motor
			off_type direct() { return dir;}
			//////////////////////////////////////////////////////////////
			// Methods
			//////////////////////////////////////////////////////////////
			void zero(); // zeros the 2 offsets
			//void Switch(uint8_t motor_number, uint8_t direction);
			void setMotor(uint8_t motor_number, uint8_t direction, bool leftorright);
			void read(int fwd_addr, int rev_addr); // initializes the variables by reading from the EEPROM
			void save(int fwd_addr, int rev_addr); // saves the variables to the EEPROM if they have changed
			void apply(int &lmotor, int &rmotor, uint8_t motor_number); // applies the offsets to the motor speed
	};
};