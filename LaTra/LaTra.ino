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
    Serial.print(msg);
    Serial.println(msg.length());
  }
  
  if(toTransmit) {
    toTransmit = 0;
    if(msg.charAt(0) == '!') {
      msg.remove(0, 1);
      Serial.println(msg);
      unsigned int threshold = msg.toInt();
      msg = "";
    } else {
      char test[msg.length()];
      msg.toCharArray(test, msg.length());
      transceiver.transmit(test);
      
      while(!transceiver.getTransmitFinished()) {
        transceiver.tickTransmitter();
      }
      Serial.println("sent");
      msg = "";
    }
  }
  
  /*
  if(transceiver.getReceiveByteAvailable()) {
    Serial.println(transceiver.getReceiveByte());
  }
  */

}
