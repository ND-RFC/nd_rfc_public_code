#include "XDATA.h"

using namespace ND;

// Title	:	Constructor
// Purpose	:	this constructor is used when you are trying to receive data
// Input 	:	none
// Returns	:	none
XDATA::XDATA()
{
	uint8_t i;
	for (i = 0; i < (PAYLOAD_SIZE/8); i++) {
		payload[i] = 0;
		prev_payload[i] = 0;
	}
	ZERO();
}
// Title	:	ZERO
// Purpose	:	initializes all the payload to their 'zero' values ... which is not all zero
// Input	:	none
// Returns	:	none
void XDATA::ZERO()
{
	if (!BYTE0) { payload[LX/8] = JOYSTICK_ZERO; prev_payload[LX/8] = JOYSTICK_ZERO;}
	if (!BYTE1) { payload[LY/8] = JOYSTICK_ZERO; prev_payload[LY/8] = JOYSTICK_ZERO;}
	if (!BYTE2) { payload[RX/8] = JOYSTICK_ZERO; prev_payload[RX/8] = JOYSTICK_ZERO;}
	if (!BYTE3) { payload[RY/8] = JOYSTICK_ZERO; prev_payload[RY/8] = JOYSTICK_ZERO;}
	if (!BYTE4) { payload[L2/8] = 0; prev_payload[L2/8] = 0; }
	if (!BYTE5) { payload[R2/8] = 0; prev_payload[R2/8] = 0; }
	if (!BYTE6) { payload[FIRST_BUTTON_1/8]  = 0; prev_payload[FIRST_BUTTON_1/8]  = 0; }
	if (!BYTE7) { payload[FIRST_BUTTON_2/8] = 0; prev_payload[FIRST_BUTTON_2/8] = 0; }
	if (!BYTE8) { payload[FIRST_BUTTON_3/8] = 0; prev_payload[FIRST_BUTTON_3/8] = 0; }
}
// Title	:	set
// Purpose	:	sets the BUTTON bit number to the inputted value,val
// Input	:	BUTTON bitnum
//				bool val
// Returns	:	none
void XDATA::set(bool val,BUTTON bitnum) 
{ 
	uint8_t byteindex, bitindex;
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return; // check to make sure bitnum is valid
	byteindex = bitnum/8;
	bitindex = bitnum%8;
	payload[byteindex] = payload[byteindex] | (val << bitindex);
}
// Title	:	set
// Purpose	:	sets the value of a BUTTON2 or a button that requires more than 1 bit
// Input	:	BUTTON2 bitnum
//				uint8_t val
// Returns	:	none
void XDATA::set(uint8_t val, BUTTON2 bitnum)
{
	uint8_t byteindex, bitindex;
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return; // check to make sure bitnum is valid
	byteindex = bitnum/8;
	bitindex = bitnum%8;
	payload[byteindex] = payload[byteindex] | (val << bitindex);
}
// Title	:	set
// Purpose	:	sets the ANALOG bit number to the inputted value, val
// Input	:	ANALOG bitnum
//				uint8_t val
// Returns	:	none
void XDATA::set(uint8_t val, ANALOG bitnum)
{
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return; // check to make sure the bitnum is valid
	uint8_t byteindex = bitnum/8;
	payload[byteindex] = val;
}
// Title	:	ButtonPress
// Purpose	:	only returns true if the xbox controller button was pressed
// Inputs	:	BUTTON
// Returns	:	bool
bool XDATA::ButtonPress(BUTTON bitnum)
{
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return false;
	// Compare previous value to current value;
	if (get(bitnum,prev_payload) == 0 && get(bitnum) == 1) return true;
	else return false;
}
// Title	:	get
// Purpose	:	gets the value of the analog bitnum
// Inputs	:	ANALOG bitnum
// Returns	:	uint8_t
uint8_t XDATA::get(ANALOG bitnum) { return get(bitnum,payload);}
uint8_t XDATA::get(ANALOG bitnum, uint8_t * pay)
{
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return 0; // check to make sure the bitnum is valid
	uint8_t byteindex = bitnum/8;
	return pay[byteindex];
}
// Title	:	get
// Purpose	:	gets the value of the button
// Inputs	:	BUTTON bitnum
// Returns	:	bool
bool XDATA::get(BUTTON bitnum) { return get(bitnum,payload);}
bool XDATA::get(BUTTON bitnum, uint8_t *pay)
{
	uint8_t byteindex, bitindex;
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return 0; // check to make sure bitnum is valid
	byteindex = bitnum/8;
	bitindex = bitnum%8;
	if ((pay[byteindex] & (1 << bitindex)) > 0) { // shift the 1 into the proper location and & it with the payload byte
		return true;
	} else {
		return false;
	}
}
// Title	:	get
// Purpose	:	gets the value of the BUTTON2, or buttons that require more than 1 bit
// Inputs	:	BUTTON2 bitnum, uint8_t *pay
// Returns	:	uint8_t
uint8_t XDATA::get(BUTTON2 bitnum, uint8_t bitsize) { return get(bitnum, bitsize, payload);}
uint8_t XDATA::get(BUTTON2 bitnum, uint8_t bitsize, uint8_t *pay)
{
	uint8_t byteindex, bitindex, shift = 0, temp = 1, i = 0, j = 0;
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return 0;
	byteindex = bitnum/8; // for Mode = 8
	bitindex = bitnum%8; // for bitindex = 0
	if (bitsize > (8-bitindex+1)) return 0; // bitsize  = 2
	// adjust the shift for the appropriate bytesize
	// loop through to set the appropriate number of 1s based on the bitsize
	for (j = 0; j < bitsize; j++) { // j just loops through the size of the desired quantity
		temp = 1;
		for (i = 0; i < (bitindex+j); i++) { // i loops through til it reaches the bitindex location...always start at the first bit
			temp *=2;
		}
		shift+=temp;
	}
	// get the appropriate value in the payload 
	return ((pay[byteindex] & (shift << bitindex)) >> bitindex);
	
}
// Title	:	get
// Purpose	:	gets the value of the index in the payload ... used for 'for' loop printing
uint8_t XDATA::get(uint8_t index)
{
	if (8*index < PAYLOAD_SIZE && index >=0) {
		return payload[index];
	} else return 0;
}
// Title	:	debugprint
// Purpose	:	prints the payload for debugging
void XDATA::debugprint(HardwareSerial *p)
{
	int length = PAYLOAD_SIZE;
	p->print("("); p->print(length); p->print(") ");
	for (int i = 0; i< length/8;i++) {
		p->print(i); p->print(": ");
		p->print(payload[i]);
		p->print("\t ");
		}
		p->println();
}