#ifndef ND_XBEE_H
#define ND_XBEE_H

#include "XBee.h"
#include "NDMessages.h"

#define DEFAULT_CHANNEL "0x0C"
#define DEFAULT_PAN "1111"
#define DEFAULT_DH "0000"
#define DEFAULT_DL "0002"
#define DEFAULT_MY "0001"
#define DEFAULT_BAUD "6" // this corresponds to 57600

#ifndef PAYLOAD_SIZE
#define PAYLOAD_SIZE 72
#endif

#define TX_STATUS_ERROR 	0	// error in receiving packet
#define TX_STATUS_SUCCESS	1	// successfully received the sent packet
#define TX_STATUS_TIMEOUT	2	// no timely response
#define TX_STATUS_RX		3	// actually it's an RX packet
#define TX_STATUS_OTHER		4	// returned another message other than a status response

namespace ND
{	
	class ND_XBEE {
		public:
			ND_XBEE();
			/*void init(HardwareSerial *p);
			void init(HardwareSerial *p, char * my, char *dl, char *baud);
			void init(HardwareSerial *p, char * my, char * dl, char * baud, char * dh, char * pan, char * channel);
			*/
			XBee xbee;
			// TRANSMISSION
			void Send(Tx16Request tx);
			void Send(uint8_t *payload, unsigned int target, int length);
			void Send(uint8_t *msg, unsigned int target);
			void Send(uint8_t msg, unsigned int target);
			uint8_t WaitForResponse(int time);
			// RECEIVING
			bool Read(void);
			bool Read(uint8_t time);
			uint8_t *getData(void);
			uint8_t getMessage(void); 
			// Addresing
			uint16_t Address16() {return address;}
			uint8_t Length() { return datalength;}
		protected:
			uint16_t address;
			uint8_t datalength;
			void getPacketInfo();
		private:
			TxStatusResponse txStatus;
			Rx16Response rx16;
		
	};
}

	
#endif