//Testing program for the library

#include "LaserTransceiver.h"

// Laser Pin; base_time; ldr_pin
LaserTransceiver transceiver(3, 1000, A0);
//LaserTransceiver transceiver(PC13, 1000, PA1);  // for stm32

String msg;

void setup() {
  Serial.begin(115200);
  transceiver.setLaserInverted(true);
}

void loop() {
  loop_transmitter();
  loop_receiver();
}

void loop_receiver(){
  if(transceiver.getReceiveByteAvailable()) {
    Serial.print("got Char: ");
    Serial.println(char(transceiver.getReceiveByte()));
  }
}

void loop_transmitter(){
  bool toTransmit = 0;
  transceiver.tickReceiver();
  
  while(Serial.available() > 0) {
    msg += Serial.readString();
    toTransmit = 1;
  }
  
  if(toTransmit) {
    toTransmit = 0;
    if(msg.charAt(0) == '!') {
      msg.remove(0, 1);
      if(msg == "getAnalogVal" || msg == "getAnalogVal\n") {
        Serial.print("analog val is: ");
        Serial.println(transceiver.getAnalogVal());
        msg = "";
      } else if(msg == "getAnalogValLaserHIGH") {
        Serial.print("analog val is: ");
        Serial.println(transceiver.getAnalogValLaserHIGH());
        msg = "";
      } else {
        unsigned int threshold = msg.toInt();
        Serial.print("changed threshold: ");
        Serial.println(threshold);
        msg = "";
      }
    } else {
      char test[msg.length() + 1];
      msg.toCharArray(test, msg.length() + 1);
      transceiver.transmit(test);
      
      while(!transceiver.getTransmitFinished()) {
        transceiver.tickTransmitter();
      }
      Serial.println("sent");
      msg = "";
    }
  }
}
