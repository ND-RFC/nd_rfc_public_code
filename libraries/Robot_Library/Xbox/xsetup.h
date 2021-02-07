#ifndef XSETUP
#define XSETUP 0
#define MAX_MODE 3
/*  SETUP 0
*	payload[0] = LX; (0-7)
*	payload[1] = LY; (8-15)
*	payload[2] = RX; (16-23)
*	payload[3] = RY; (24-31)
*	payload[4] = L2; (32-39)
*	payload[5] = R2; (40-47)
*	payload[6] = A | B | X | Y | UP | DOWN | LEFT | RIGHT
* 				 0   1   2   3   4     5       6      7
*   payload[7] = L1 | R1 | L3 | R3 | START | BACK | XBOX | STATUS
* 				 0    1     2    3     4       5     6      7
*	payload[8] = MODE(0&1)|  - |  - |   -   |   -  |  -   |    -|
* 				 0    1     2    3     4       5     6      7
*------------------------------------------------------------------
* MODE must be set to 1 in order for the receiver to interpret the data
*/
	/* These values decide if you are going to send these bytes. 
	* These values must be the same for the sender and receiver or else the data will not be read correctly 
	* 0 - data is being sent
	* 1 - data is not being sent
	*/

#if XSETUP == 0
	#define BYTE0 0 // byte 0
	#define BYTE1 0 // byte 1
	#define BYTE2 0 // byte 2
	#define BYTE3 0 // byte 3
	#define BYTE4 0 // byte 4
	#define BYTE5 0 // byte 5
	#define BYTE6 0 // byte 6
	#define BYTE7 0 // byte 7
	#define BYTE8 0 // byte 8
	// Calculate the bit position adjustment for each byte just in case there
	// is any data not being sent ... if all data is sent these values will all be 0
	#define UNSENT1 BYTE0*8
	#define UNSENT2 UNSENT1+(BYTE1*8)
	#define UNSENT3 UNSENT2+(BYTE2*8)
	#define UNSENT4 UNSENT3+(BYTE3*8)
	#define UNSENT5 UNSENT4+(BYTE4*8)
	#define UNSENT6 UNSENT5+(BYTE5*8)
	#define UNSENT7 UNSENT6+(BYTE6*8)
	#define UNSENT8 UNSENT7+(BYTE7*8)
	#if (BYTE7 == 1)
		#define UNSENT9 UNSENT8+(BYTE8*8)+8
	#else
		#define UNSENT9 UNSENT8+(BYTE8*8)
	#endif
	// undefine the payload size if pre-defined ... this is because the xbee will define a default payload
	// but this is really where you want PAYLOAD_SIZE defined
	#ifdef PAYLOAD_SIZE
	#undef PAYLOAD_SIZE
	#endif
	// PAYLOAD_SIZE is determined from what is turned on and off
	#define PAYLOAD_SIZE 72-(UNSENT9)
#endif

namespace ND 
{
	enum BUTTON {
		A 		= 48-(UNSENT7),
		B 		= 49-(UNSENT7),
		X 		= 50-(UNSENT7),
		Y 		= 51-(UNSENT7),
		UP 		= 52-(UNSENT7),
		DOWN 	= 53-(UNSENT7),
		LEFT 	= 54-(UNSENT7),
		RIGHT 	= 55-(UNSENT7),
		L1 		= 56-(UNSENT8),
		R1 		= 57-(UNSENT8),
		L3 		= 58-(UNSENT8),
		R3 		= 59-(UNSENT8),
		START	= 60-(UNSENT8),
		BACK	= 61-(UNSENT8),
		XBX 	= 62-(UNSENT8),
		STATUS	= 63-(UNSENT8),
	};
	
	enum BUTTON2 {
		MODE	= 64-(UNSENT9),
	};

	enum ANALOG {
		LX		= 0-(UNSENT1)-(BYTE0*1), // these must be set for different values if there are multiple bytes unsent
		LY		= 8-(UNSENT2)-(BYTE1*10),
		RX		= 16-(UNSENT3)-(BYTE2*18),
		RY 		= 24-(UNSENT4)-(BYTE3*27),
		L2		= 32-(UNSENT5)-(BYTE4*36),
		R2		= 40-(UNSENT6)-(BYTE5*45),
	};
};
#endif