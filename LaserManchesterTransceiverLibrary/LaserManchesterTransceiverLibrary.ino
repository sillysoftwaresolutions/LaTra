//Testing program for the library

#include "LaserTransceiver.h"

LaserTransceiver transceiver(3, 20, A0);

String msg;

void setup() {
  Serial.begin(115200);

}

void loop() {
  bool toTransmit = 0;
  
  while(Serial.available() > 0) {
    msg += Serial.readString();
    toTransmit = 1;
    Serial.println(msg);
  }
  
  if(toTransmit) {
    toTransmit = 0;
    transceiver.transmit(msg, msg.length());
    while(!transceiver.getTransmitFinished()) {
      transceiver.tickTransmitter();
    }
    Serial.println("out");
  }

}
