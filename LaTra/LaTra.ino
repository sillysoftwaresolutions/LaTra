//Testing program for the library

#include "LaserTransceiver.h"

LaserTransceiver transceiver(3, 40, A1);

String msg;

void setup() {
  Serial.begin(115200);

}

void loop() {
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
      if(msg == "getAnalogVal") {
        Serial.print("analog val is: ");
        Serial.println(transceiver.getAnalogVal());
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
  
  
  if(transceiver.getReceiveByteAvailable()) {
    Serial.print("got Char: ");
    Serial.println(char(transceiver.getReceiveByte()));
  }
  

}
