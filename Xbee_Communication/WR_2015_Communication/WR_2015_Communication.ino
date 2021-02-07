// Note: YOU CANNOT SERIAL PRINT IF YOU ARE SENDING DATA PACKETS

/////////////////////////////////////////////////////////////////
//           MUST CHANGE THIS DEFINITION FOR EACH WR           //
//         CHOOSE A WR_IDENITFIER DEPENDING ON WR COLOR        //
#define WR_IDENTIFIER 1 // uncomment this line for PINK WR   //
//#define WR_IDENTIFIER 3 // uncomment this line for YELLOW WR //
//#define WR_IDENTIFIER 4 // uncomment this line for GREEN WR  //
//#define WR_IDENTIFIER 5 // uncomment this line for BLUE WR   //
/////////////////////////////////////////////////////////////////

#include <ND/Xbee/ND_XBEE.h>
#include <ND/Xbox/RXDATA.h>

using namespace ND;
ND_XBEE ndbee = ND_XBEE();

#define BAUD_RATE 38400
#define QB_ROBOT_ADDRESS 0x1001
#define OFFSET 10 //ultrasound offset (in)

long time1, time;
long ss=13397;
char count;
boolean xbeeflag, ultraflag, bounceflag;
int D;
int Da, Db, Dc;
uint8_t message[3] = {0,0,0};


void setup(){
  xbeeflag=false;
  ultraflag=false;
  bounceflag=false;
  count=0;
  Serial.begin(BAUD_RATE);
  ndbee.xbee.setSerial(Serial);
  attachInterrupt(0,xbee_isr,RISING);
  attachInterrupt(1,ultra_isr,FALLING);
}

void xbee_isr(){
  time1=micros();
  xbeeflag=true;
  bounceflag=true;
}

void ultra_isr(){
  if (bounceflag){
    time=micros()-time1;
    ultraflag=true;
    bounceflag=false;
  }
}

void send_packet(int D){
  message[0] = WR_IDENTIFIER;
  message[1] = (D >> 8) & 0xFF;
  message[2] = (D & 0xFF);
  ndbee.Send(message,QB_ROBOT_ADDRESS,PAYLOAD_SIZE);
}

void loop(){
  if (xbeeflag) xbeeflag=false;
  
  if (ultraflag){
    ultraflag=false;
    if (time > 1000 && time < 50000){
      D=time*ss/1000000 - OFFSET;
      ++count;
    }
  if (count == 1) Da = D;
  else if (count == 2) Db = D;
  else if (count == 3) {
    Dc = D;
    D = min(Da,min(Db,Dc));
    send_packet(D);
    count = 0;
    }    
 }
}
      

