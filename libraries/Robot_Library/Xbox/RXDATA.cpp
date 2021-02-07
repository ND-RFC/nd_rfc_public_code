#include "RXDATA.h"

using namespace ND;

RXDATA::RXDATA() 
{
	timeout = true;
	NoPacket = true;
	ZERO();
}
// Title	:	set
// Purpose	:	sets the whole payload equal to the new payload
void RXDATA::set(uint8_t * newpayload)
{
	uint8_t i;
	for (i = 0; i < (PAYLOAD_SIZE/8); i++) {
		prev_payload[i] = payload[i];
		payload[i] = newpayload[i];
	}
	NoPacket = false;
	timeout = 0;
}

// Title	:	ButtonPress
// Purpose	:	only returns true if the xbox controller button was pressed
// Inputs	:	BUTTON
// Returns	:	bool
bool RXDATA::ButtonPress(BUTTON bitnum)
{
	if (NoPacket) return false;
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return false;
	// Compare previous value to current value;
	if (get(bitnum,prev_payload) == 0 && get(bitnum) == 1) return true;
	else return false;
}
bool RXDATA::ButtonRelease(BUTTON bitnum)
{
	if (NoPacket) return false;
	if (bitnum < 0 || bitnum >= PAYLOAD_SIZE) return false;
	// Compare previous value to current value;
	if (get(bitnum,prev_payload) == 1 && get(bitnum) == 0) return true;
	else return false;
}

uint8_t RXDATA::getTrigger(ANALOG bitnum)
{
	return get(bitnum);
}

signed char RXDATA::getJoystick(ANALOG bitnum)
{
	return (get(bitnum)-128);
}
signed char RXDATA::AnalogChange(ANALOG bitnum)
{
	signed char cur_val = get(bitnum)-128;
	signed char prev_val = get(bitnum,prev_payload)-128;
	return (cur_val-prev_val);
}
// Title	:	TimeOut
// Purpose	:	returns true if timeout is > MAX_TIMEOUT... this is used for
//				the receiving code to see if there has not been a packet received for awhile
//				We would want to cut off motors if this is the case.  Also, Note that
//				MAX_TIMEOUT may need to be increased because it is based on the difference
//				in time elapsed between RX and TX
// Inputs	:	none
// Returns	:	bool
bool RXDATA::TimeOut()
{
	if (timeout > MAX_TIMEOUT) return true;
	else { timeout++; return false; }
}