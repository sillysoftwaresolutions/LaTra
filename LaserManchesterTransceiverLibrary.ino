#include "LaserTransceiver.h"

LaserTransceiver transceiver(3, 200);

String msg = "hallo";

void setup() {
  Serial.begin(115200);

}

void loop() {
  //while(Serial.available() > 0)
  transceiver.transmit(msg, msg.length());
  while(!transceiver.getTransmitFinished()) {
    transceiver.tickTransmitter();
  }

}
