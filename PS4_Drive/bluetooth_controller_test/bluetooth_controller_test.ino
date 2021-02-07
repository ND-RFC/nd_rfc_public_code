#include <usbhub.h>
#include <PS4BT.h>
#include <spi4teensy3.h>
#include <SPI.h>

// if you redownload the USB_Host_Shield_2.0 library,
// update line 43 of UsbCore.h to say: typedef MAX3421e<P15, P9> MAX3421E;


USB Usb;
BTD Btd(&Usb);
PS4BT PS4(&Btd);


// Global variables:
int rumble = 0;

void setup() {
  // put your setup code here, to run once:
  // Check to Make Sure USB Initialized
  
  //pinMode(15, OUTPUT);
  //digitalWrite(15, LOW);
  if (Usb.Init() == -1)
  {
    Serial.print("OSC did not start\n");
    while (1); // Halt
  }
  Serial.print("PS4 Bluetooth Library Started\n");

  PS4.setLed(Green);

}

void loop() {
  // put your main code here, to run repeatedly:

  Usb.Task();
  // PS4 Controller Connected
  if (PS4.connected()) {
    updateController();
  } // End of PS4.connected
  else {
    Serial.print("PS4 Controller Not Connected\n");
  }
}

void updateController() {
  if (PS4.getButtonClick(UP)) {
    Serial.print(F("\r\nUp"));
    PS4.setLed(Red);
  } if (PS4.getButtonClick(RIGHT)) {
    Serial.print(F("\r\nRight"));
    PS4.setLed(Blue);
  } if (PS4.getButtonClick(DOWN)) {
    Serial.print(F("\r\nDown"));
    PS4.setLed(Yellow);
  } if (PS4.getButtonClick(LEFT)) {
    Serial.print(F("\r\nLeft"));
    PS4.setLed(Green);
  }
  if (PS4.getButtonClick(L1)) {
    if (rumble == 1) {
      PS4.setRumbleOff();
      rumble = 0;
    }
    else {
      PS4.setRumbleOn(RumbleLow);  
      rumble = 1;    
    }
  }
  if (PS4.getButtonClick(R1)) {
    if (rumble == 2) {
      PS4.setRumbleOff();
      rumble = 0;
    }
    else {
      PS4.setRumbleOn(RumbleHigh);
      rumble = 2;
    }
  }
}



