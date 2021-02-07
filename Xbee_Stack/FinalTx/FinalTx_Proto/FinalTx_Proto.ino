/* Final Tx
* This is downloaded to the controller.  Make sure the switch is set to USB.
*--------------------------------------------------------------------
* NOTE:
* ROBOT_ADDRESS is the only thing that needs changed from controller to controller
* This must be changed and it should be equal to the address of the xbee on your robot
* For example, if ROBOT_ADDRESS = 0x0008, then the xbee on the robot must have a MY address of 0x0008
*--------------------------------------------------------------------
* Transmitting function for the controller
* Can be used for all robots except the QB which needs extra functionality from sensors
* Based on the Xbee-Arduino example files, and USB Host Shield Example for Xbox 360 controller
* 
Joe Rudy
24 Jan 2013

Should allow signals from an Xbox controller to be used to control
the motor speed using Xbees for wireless communication in API mode
*/

/******************************
* Addressing and Other Constants
******************************/
#define BAUD_RATE 38400
#define ROBOT_ADDRESS 0x1001  // Robot address in hex
/******************************
* Include Files
******************************/
#include <ND/Xbox/XBOXUSB.h>
#include <ND/Xbox/NDXBOX.h>
#include <ND/Xbee/ND_XBEE.h>
/***************************************************
* Function Prototypes and Namespaces
***************************************************/
using namespace ND;
/******************************
* Global Variables
******************************/
USB Usb;
XBOXUSB Xbox(&Usb);
// Setup the ND xbox class
XBOX ndbox(&Xbox,&Usb);
// Setup the ND xbee class;
ND_XBEE ndbee;
uint8_t statvar = 0;
uint8_t error_count = 0; // counts the number of errored messages
/***************************************************
* Debugging Variables
***************************************************/
//#define DEBUG_TIME
#ifdef DEBUG_TIME
  long time1, time2;
#endif

#define DEBUG
#ifdef DEBUG
  uint8_t *fake;
#endif
/***************************************************
* setup
***************************************************/
void setup() {
  Serial.begin(BAUD_RATE);
  // Initialize classes
  ndbox.init(&Serial); ndbee = ND_XBEE();
  ndbee.xbee.setSerial(Serial);
  // wait for the xbox to connect
  while (!ndbox.isConnected()) {ndbox.Update();}// you must poll the xbox until it syncs up}
  ndbox.Rotate();
  // Wait until you get a response
  do {
    ndbee.Send(ndbox.get(),ROBOT_ADDRESS, PAYLOAD_SIZE);
    delay(50);
  } while ( ndbee.WaitForResponse(5000) != TX_STATUS_SUCCESS);
  ndbox.LedOn(LED1);
  ndbox.SendOnOff(1);
  delay(3000);
}

/***************************************************
* loop
***************************************************/
void loop() {
#ifdef DEBUG_TIME
  time1 = micros();
#endif
  // change the mode if XBOX button pressed
  if (ndbox.ButtonPress(XBX)){
      ndbox.SetMode(ndbox.GetMode()+1);
  }
  ndbox.Update(); // update the data
  // get the payload and send it to the robot address :)
  ndbee.Send(ndbox.get(), ROBOT_ADDRESS, PAYLOAD_SIZE);
   Serial.println();
  // Wait for a Response for 1 second
  statvar = ndbee.WaitForResponse(1000);
  //Interpret the status response 
  switch (statvar) 
  {
///////////////////////// CASE TX_STATUS_SUCCESS ///////////////////////
      case TX_STATUS_SUCCESS:
          ndbox.LedOn(ndbox.led());  
          error_count=0; 
#ifdef DEBUG
          Serial.print("Success"); 
#endif
          break; 
///////////////////////// CASE TX_STATUS_TIMEOUT ///////////////////////
      case TX_STATUS_TIMEOUT: 
          ndbox.LedOn(ALL); 
          ndbox.Alternate(); 
#ifdef DEBUG
          Serial.print("timeout"); 
#endif
          break;
///////////////////////// CASE TX_STATUS_ERROR ///////////////////////
      case TX_STATUS_ERROR:   
        if (error_count >= 5) {
            ndbox.LedOn(ALL); ndbox.BlinkFast(); 
        } else error_count++;
#ifdef DEBUG
     Serial.print("error");
#endif
      break;
///////////////////////// CASE TX_STATUS ///////////////////////
      case TX_STATUS_RX:     
#ifdef DEBUG
        Serial.print("RX");
#endif 
        break;
      case TX_STATUS_OTHER:  Serial.print("Other"); break;
      default: break;
   };
   delay(25);
#ifdef DEBUG
  ndbox.debugprint(&Serial);
#endif
#ifdef DEBUG_TIME
  time2 = micros();
  Serial.print("One loop = "); Serial.print(time2-time1); Serial.println(" microseconds");
#endif
    
}
  
  
  
  
  
  
