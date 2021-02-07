#ifndef ND_RXDATA_H
#define ND_RXDATA_H
#include "XDATA.h"

#define MAX_TIMEOUT 25

namespace ND
{
	class RXDATA : public XDATA
	{
		public:
			RXDATA();
			// setting the initial payload
			void set(uint8_t * newpayload);
			// Button Presses and returns
			virtual bool ButtonPress(BUTTON bitnum);
			virtual bool ButtonRelease(BUTTON bitnum);
			uint8_t getTrigger(ANALOG bitnum);
			signed char getJoystick(ANALOG bitnum);
			signed char AnalogChange(ANALOG bitnum);
			// determines if there was a packet received on this count through
			bool NoPacket;
			bool TimeOut(); // increases timeout and returns true if timeout > MAX_TIMEOUT
		protected:
		private:
			uint8_t timeout;
			uint8_t prev_payload[PAYLOAD_SIZE];
	};
};


#endif