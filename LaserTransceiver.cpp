#include "Arduino.h"
#include "LaserTransceiver.h"

LaserTransceiver::LaserTransceiver(uint8_t pin, uint16_t baseL) {
  laserPin = pin;
  baseLength = baseL;
  halfClock = baseLength / 2;
  pinMode(laserPin, OUTPUT);
}

void LaserTransceiver::transmit(String buf, uint8_t len) {
  //int i = 0;
  //while((msg[i] = buf[i]) != '\0') ++i; 
  //*msg = *buf;
  //char msg[len + 1];
  buf.toCharArray(msg, sizeof(msg));
  msgLength = len; 
  transmitFinished = 0;

  
}

bool LaserTransceiver::getTransmitFinished() {
  return transmitFinished;
}

void LaserTransceiver::tickTransmitter() {
  unsigned long now = millis();

  if(now - halfStart >= halfClock) {
    halfClock = baseLength / 2;
    halfStart = now;
    transmitByte = msg[msgIndex];
    //Serial.println(bitRead(transmitByte, 7 - byteIndex));
    if(!secondHalf) {
      digitalWrite(laserPin, !bitRead(transmitByte, 7 - byteIndex)); 
      secondHalf = 1;
    } else {
      digitalWrite(laserPin, bitRead(transmitByte, 7 - byteIndex)); 
      secondHalf = 0;
      byteIndex++;
      
      if(byteIndex >= 8) {
        Serial.println(msg);
        byteIndex = 0;
        msgIndex++;
        halfClock = 2 * baseLength;
        digitalWrite(laserPin, LOW);
        if(msgIndex >= msgLength) {
          transmitFinished = 1;
          msgIndex = 0;
          digitalWrite(laserPin, LOW);
        }
      }
      
    } 

  }  
}
