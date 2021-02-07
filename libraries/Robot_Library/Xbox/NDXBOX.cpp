#include "NDXBOX.h"
using namespace ND;
// Title	:	Constructor
// Purpose	:	this constructor is used when you are trying to transmit data
// Input 	:	none
// Returns	:	none
XBOX::XBOX(XBOXUSB *xbusb, USB *Usb)
{
	xbox = xbusb;
	usb = Usb;
	status = false;
	ZERO();
}

// Title	:	xbox
// Purpose	:	returns the XBOXUSB class used by this class
// Input	:	none
// Returns	:	XBOXUSB
XBOXUSB XBOX::Xbox()
{
	return (*xbox);
}

// Title	:	init
// Purpose	:	use this code in the setup function of the arduino, "setup" function
// Input	:	none
// Returns	:	false
void XBOX::init(HardwareSerial *p)
{
	if (usb == NULL || xbox == NULL) return; // return if these are equal to NULL (either not set or receiving function
	if (usb->Init() == -1) {
		p->println(F("\r\nOSC did not start"));
		while(1); //halt
	}
	xbox->setLedBlink(ALL);
	p->println(F("XBOX Library initialized"));
}
// Title	:	Update
// Purpose	:	updates all the values of the buttons and analogs
// Input	:	none
// Returns	:	none
void XBOX::Update(void)
{
	uint8_t i;
	usb->Task();
	// Update the Buttons
	for (i = 0; i < (PAYLOAD_SIZE/8); i++) {
		prev_payload[i] = payload[i];
	}
	if (!BYTE6 || !BYTE7) {
		if (xbox->buttonChanged) {
			if (!BYTE6) {
				payload[FIRST_BUTTON_1/8] = 0;
				Update(A); Update(B);Update(X); Update(Y);
				Update(UP); 
				Update(RIGHT); 
				Update(DOWN); 
				Update(LEFT);
			}
			if (!BYTE7) {
				payload[FIRST_BUTTON_2/8] = 0;
				Update(L1); Update(R1);Update(L3); Update(R3);
				Update(START); Update(BACK); Update(XBX); 
			}
		}
		Update(STATUS);
	}
	if (!BYTE8) {
		payload[FIRST_BUTTON_3/8] = 0;
		Update(MODE);
	}
	// Update Analogs
	if (!BYTE0) Update(LX); 
	if (!BYTE1) Update(LY);
	if (!BYTE2) Update(RX); 
	if (!BYTE3) Update(RY);
	if (!BYTE4) Update(L2); 
	if (!BYTE5) Update(R2);
}

// Title	:	Update
// Purpose	:	updates the values of the payload but does individual buttons
// Input 	:	BUTTON b
// Returns	:	none
void XBOX::Update(BUTTON b) 
{
	switch (b) 
	{
		case A:		set(xbox->getButton(B_A),A); break;
		case B:		set(xbox->getButton(B_B),B); break;
		case X:		set(xbox->getButton(B_X),X); break;
		case Y:		set(xbox->getButton(B_Y),Y); break;
		case UP:	set(xbox->getButton(B_UP),UP); break;
		case DOWN:	set(xbox->getButton(B_DOWN),DOWN); break;
		case LEFT:	set(xbox->getButton(B_LEFT),LEFT); break;
		case RIGHT:	set(xbox->getButton(B_RIGHT),RIGHT); break;
		case L1:	set(xbox->getButton(B_L1),L1); break;
		case R1:	set(xbox->getButton(B_R1),R1); break;
		case L3:	set(xbox->getButton(B_L3),L3); break;
		case R3:	set(xbox->getButton(B_R3),R3); break;
		case START:	set(xbox->getButton(B_START),START); break;
		case BACK: 	set(xbox->getButton(B_BACK),BACK); break;
		case XBX: 	set(xbox->getButton(B_XBOX),XBX); break;
		case STATUS:set(status, STATUS); break;
	};
}
// Title	:	Update
// Purpose	:	updates the values of the payload for BUTTON2 enumeration
// input	:	BUTTON2 bitnum
// Returns	:	none
void XBOX::Update(BUTTON2 bitnum)
{
	switch (bitnum)
	{
		case MODE:		set(mode,MODE); break;
	}

}
// Title	:	Update
// Purpose	:	updates the values of the payload but does individual buttons
// Input 	:	ANALOG a
// Returns	:	none
void XBOX::Update(ANALOG a) 
{
	signed short ss_low = -32768, ss_high = 32767;
	unsigned char  uc_low = 0, uc_high = 255;
	switch (a) {
		case LY: set(lowByte(map(xbox->getAnalogHat(LeftHatY),ss_low,ss_high,uc_low,uc_high)),LY); break;
		case LX: set(lowByte(map(xbox->getAnalogHat(LeftHatX),ss_low,ss_high,uc_low,uc_high)),LX); break;
		case RX: set(lowByte(map(xbox->getAnalogHat(RightHatX),ss_low,ss_high,uc_low,uc_high)),RX); break;
		case RY: set(lowByte(map(xbox->getAnalogHat(RightHatY),ss_low,ss_high,uc_low,uc_high)),RY); break;
		case L2: set(xbox->getButton(B_L2),L2); break;
		case R2: set(xbox->getButton(B_R2),R2); break;
	};
}
// Title	:	SendOnOff
// Purpose	:	updates the SendOnOff button
// Inputs	:	bool onoff
// Returns	:	none
void XBOX::SendOnOff(bool onoff)
{
	status  = onoff;
}
// Title	:	Mode
// Purpose	:	sets the mode of the controller
// Inputs	:	bool
// Returns	:	none
void XBOX::SetMode(uint8_t m)
{
	mode = m;
	if (mode > MAX_MODE) mode = 0;
}
uint8_t XBOX::GetMode()
{
	return mode;
}
// Title	:	led
// Purpose	:	gives the LED to be lit up when everything is properly functioning
// Inputs	:	none
// Returns  :	LED
LED XBOX::led()
{
	if (mode == 0) {
		return LED1;
	} else if (mode == 1) {
		return LED2;
	} else if (mode == 2) {
		return LED3;
	} else if (mode == 3) {
		return LED4;
	}
}