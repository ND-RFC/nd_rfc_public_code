#ifndef ND_XBOX_H
#define ND_XBOX_H

#include "XBOXUSB.h"
#include "XDATA.h"
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

namespace ND 
{
	class XBOX : public XDATA
	{	
	public:
		XBOX(XBOXUSB *xbusb, USB *Usb);
		// get the XBOXUSB class used by this class
		// initialization
		void init(HardwareSerial *p);
		XBOXUSB Xbox();
		// TRANSMITTING FUNCTIONS
		void Update(void); // updates the payload from the inputs of the xbox
		void Update(BUTTON bitnum); // updates the payload for the button inputted
		void Update(BUTTON2 bitnum);
		void Update(ANALOG bitnum); // updates payload for the analog inputted
		// XBox connections and LEDs
		bool isConnected() { return xbox->Xbox360Connected;}
		// Specific bit manipulations ... not really buttons, but still sent data
		void SendOnOff(bool onoff); // changes the STATUS bit to 1 (ON) or 0 (OFF) ... set this to ON when you want the receiver to interpret
		void SetMode(uint8_t m);
		uint8_t GetMode();
		// XBOX Light Controls
		LED led(); // this determines the mode
		void Rotate() { xbox->setLedMode(ROTATING); delay(100);}
		void Alternate() { xbox->setLedMode(ALTERNATING); delay(100);}
		void BlinkFast() { xbox->setLedMode(FASTBLINK); delay(100);}
		void BlinkSlow() { xbox->setLedMode(SLOWBLINK); delay(100);}
		void LedOn(LED l) { xbox->setLedOff(); xbox->setLedOn(l);}
	protected:
		USB *usb;
		XBOXUSB *xbox;
	private:
		bool status; // tells if the controller is ready to transmit or not
		uint8_t mode; // determines the controller mode
		
	};
};
#endif