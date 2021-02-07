#include "ND_XBEE.h"

using namespace ND;


ND_XBEE::ND_XBEE()
{
	datalength = 0;
	address = 0x0000;
	xbee = XBee();
	rx16 = Rx16Response();
}
// Title	:	init
// Purpose	:	initializes the xbee .... use this in the 'setup' function of main code
//				for some reason the xbee will reset and go back to the start of the setup code
// Inputs	:	HardwareSerial *p
// Returns	:	none
/*void ND_XBEE::init(HardwareSerial *p) { init(p,DEFAULT_MY,DEFAULT_DL,DEFAULT_BAUD, DEFAULT_DH,DEFAULT_PAN,DEFAULT_CHANNEL);}
void ND_XBEE::init(HardwareSerial *p, char * my, char * dl, char *baud) { init(p,my,dl,baud, DEFAULT_DH,DEFAULT_PAN,DEFAULT_CHANNEL);}
void ND_XBEE::init(HardwareSerial *p, char * my, char * dl, char * baud, char * dh, char * pan, char * channel)
{
	p->print("+++"); delay(4000); // go into edit mode
	p->print("ATRE"); p->print("\r");// reset to factory settings 
	p->print("ATAP2"); p->print("\r");// set to API mode - 2
	p->print("ATCE1"); p->print("\r");// make a coordinator
	p->print("ATBD"); p->print(baud); p->print("\r"); // set up the baud rate -- 5 = 57600;
	p->print("ATMY");p->print(my); p->print("\r");// sets my addr
	p->print("ATDL"); p->print(dl); p->print("\r");// sets the low addr
	p->print("ATDH"); p->print(dh); p->print("\r");// sets the high addr
	p->print("ATCH"); p->print(channel); p->print("\r");// sets the channel
	p->print("ATID"); p->print(pan); p->print(",");// PAN ID
	p->print("ATWR"); p->print("\r"); 
	p->print("ATWR"); p->print("\r"); 
	p->print("ATFR"); p->print("\r");
	delay(2000);
}*/
// Title	:	Send
// Purpose	:	updates all the packets and then sends them all
// Inputs	:	uint8_t payload
//				unsigned short payload_size
// Returns	:	void
void ND_XBEE::Send(Tx16Request tx)
{
	xbee.send(tx);//xbee.send does not like tx[i] being sent so just send a substitute packet
			// it probably does not like sending data on the heap ??
}
// Title	:	Send
// Purpose	:	you can use this send instead if you wish to send a message
// Inputs	:	uint8_t *payload
//				unsigned int target .... destination address
//				int length .... length of payload
void ND_XBEE::Send(uint8_t *payload, unsigned int target, int length)
{
	Tx16Request tx = Tx16Request(target,payload,length);
	xbee.send(tx);
}
// used to send a single byte message
void ND_XBEE::Send(uint8_t *msg, unsigned int target){ Send(msg,target,8);}
void ND_XBEE::Send(uint8_t msg, unsigned int target) { uint8_t *msgptr = &msg; Send(msgptr,target);}
// Title	:	Read
// Purpose	:	reads a packet and returns true if it found anything
// Inputs	:	void
// Returns	:	bool
bool ND_XBEE::Read()
{
	xbee.readPacket();
	if (xbee.getResponse().isAvailable()) {
		getPacketInfo();
		return true;
	} else {
		address = 0;
		datalength = 0;
		return false;
	}
}
// Title	:	Read
// Purpose	:	waits a inputted time to receive a packet... use for status response
bool ND_XBEE::Read(uint8_t time)
{
	if (xbee.readPacket(time)) {
		if (xbee.getResponse().isAvailable()) {
			getPacketInfo();
			return true;
		} else {
			address = 0; datalength = 0;
			return false;
		}
	} else if (xbee.getResponse().isError()) {
		// error code
		address = 0; datalength = 0;
		return false;
	} else {
		address = 0; datalength = 0;
		return false;
	}
}
// Title	:	getData
// Purpose	: 	get the data of the incoming packet or tx_response_status
// Inputs	:	uint8_t *
// Returns	:	
uint8_t * ND_XBEE::getData()
{
	if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
	{
		return rx16.getData();
	} else return NULL;
}
uint8_t ND_XBEE::getMessage()
{ 
	if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
	{
		return rx16.getData(0);
	} else return 0;
}
// Title	:	WaitForResponse
// Purpose	:	waits at most the inputted time [in milliseconds] for a status response and then
//				returns a message ... either TX_STATUS_ERROR, TX_STATUS_SUCCESS, TX_STATUS_TIMEOUT, TX_STATUS_OTHER
// Inputs	:	int time [in milliseconds]
// Returns	:	int
uint8_t ND_XBEE::WaitForResponse(int time)
{
	if (Read(time)) {
		if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) // received a status response
		{
			xbee.getResponse().getZBTxStatusResponse(txStatus);
			if (txStatus.getStatus() == SUCCESS) return TX_STATUS_SUCCESS;
			else return TX_STATUS_ERROR;
		} else if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
			return TX_STATUS_RX;
		} else { // another type of packet
			return TX_STATUS_OTHER;
		}
	} else return xbee.getResponse().getErrorCode();
}
// Title	:	getPacketInfo
// Purpose	:	gets the packet information like the address and datalength
// Inputs	:	none
// Returns	:	none
void ND_XBEE::getPacketInfo()
{
	if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
	{
		xbee.getResponse().getRx16Response(rx16);
		datalength = rx16.getDataLength()/8;
		address = rx16.getRemoteAddress16();
	}
	if (xbee.getResponse().getApiId() == RX_64_RESPONSE) {
	
	}
}