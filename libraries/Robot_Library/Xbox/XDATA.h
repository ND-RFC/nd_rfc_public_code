#ifndef ND_XDATA_H
#define ND_XDATA_H

#include "xsetup.h"
#include <Arduino.h>

/*  SETUP 0
*	payload[0] = LX; (0-7)
*	payload[1] = LY; (8-15)
*	payload[2] = RX; (16-23)
*	payload[3] = RY; (24-31)
*	payload[4] = L2; (32-39)
*	payload[5] = R2; (40-47)
*	payload[6] = A | B | X | Y | UP | DOWN | LEFT | RIGHT
* 				 0   1   2   3   4     5       6      7   (bit indices)
*   payload[7] = L1 | R1 | L3 | R3 | START | BACK | XBOX | MODE
* 				 0    1     2    3     4       5     6      7 (bit indices)
*------------------------------------------------------------------
*/
#define JOYSTICK_ZERO 128
#define FIRST_BUTTON_1 A
#define FIRST_BUTTON_2 L1
#define FIRST_BUTTON_3 MODE

namespace ND 
{
	class XDATA
	{	
	public:
		XDATA();
		// ZERO the payload
		virtual void ZERO(void); // this should 'zero' all the values in the payload
		
		// debug printing
		void debugprint(HardwareSerial *p);
		// Get functions
		virtual bool ButtonPress(BUTTON bitnum);
		uint8_t * get(void) { return payload;} // gets the payload
		// Get button functions
		uint8_t get(ANALOG bitnum);
		bool get(BUTTON bitnum);
		
		uint8_t get(BUTTON2 bitnum, uint8_t bitsize);
		uint8_t get(uint8_t index);
	protected:
		uint8_t payload[PAYLOAD_SIZE];
		uint8_t prev_payload[PAYLOAD_SIZE];
		
		// used to get any member of a random payload in any class ... used in receiving
		uint8_t get(ANALOG bitnum, uint8_t *pay);
		bool get(BUTTON bitnum, uint8_t *pay);
		uint8_t get(BUTTON2 bitnum, uint8_t bytesize, uint8_t *pay);
		// Set functions
		void set(uint8_t val,ANALOG a);
		void set(bool val, BUTTON b);
		void set(uint8_t val, BUTTON2 bitnum);
		
	};
};
#endif