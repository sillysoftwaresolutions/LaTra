#include "Arduino.h"
#include "LaserTransceiver.h"

LaserTransceiver::LaserTransceiver(uint8_t lPin, uint16_t baseL, uint8_t ldPin) {
  laserPin = lPin;
  ldrPin = ldPin;
  baseLength = baseL;
  halfClock = 2 * baseLength;
  bitTime = baseLength;
  pinMode(laserPin, OUTPUT);
}

void LaserTransceiver::transmit(char * message) {
  //Serial.println(message);
  msg = message;
  msgLength = strlen(msg); 
  transmitFinished = 0;
  halfClock = 2 * baseLength;
  halfStart = millis();
  bitLengthTransmitted = 0;
  
}

bool LaserTransceiver::getTransmitFinished() {
  return transmitFinished;
}

bool LaserTransceiver::getReceiveByteAvailable() {
  return receiveByteAvailable;
}

byte LaserTransceiver::getReceiveByte() {
  receiveByteAvailable = 0;
  return receiveByteReturn;
}

void LaserTransceiver::setReceiveThreshold(unsigned int val) {
  analogThreshold = val;
}

uint16_t LaserTransceiver::getAnalogVal() {
  return analogRead(ldrPin);
}

void LaserTransceiver::tickTransmitter() {
  unsigned long now = millis();
  
  if(now - halfStart >= halfClock) {
    halfClock = baseLength / 2;
    halfStart = now;

    //preamble
    if(msgIndex == 0) {
      if(!bitLengthTransmitted) {
        halfClock = baseLength;
        digitalWrite(laserPin, HIGH);
        bitLengthTransmitted = 1;
        return;
      } else {
        halfClock = baseLength / 2;
        digitalWrite(laserPin, LOW);
        msgIndex++;
        return;  
      }
      
    }

    //send data with Manchester Code
    transmitByte = msg[msgIndex - 1];
    if(!secondHalf) {
      digitalWrite(laserPin, !bitRead(transmitByte, byteIndex)); 
      secondHalf = 1;
    } else {
      digitalWrite(laserPin, bitRead(transmitByte, byteIndex)); 
      secondHalf = 0;
      byteIndex++;
      
      if(byteIndex >= 8) {
        byteIndex = 0;
        msgIndex++;
        digitalWrite(laserPin, LOW);
        if(msgIndex > msgLength) {
          transmitFinished = 1;
          msgIndex = 0;
          bitLengthTransmitted = 0;
          halfClock = 2 * baseLength;
          digitalWrite(laserPin, LOW);
        }
      }
      
    } 

  } 
}

void LaserTransceiver::tickReceiver() {
  bool thisBit = this->bitDecode();

  if(thisBit == oldBit) return; 
  oldBit = thisBit;  

  unsigned long now = millis();
  unsigned long dTime = now - lastEdge;

  if(dTime > bitTime * 2){
    lastEdge = now; 
    receiveByteIndex = 255;  
    
  }else if(receiveByteIndex == 255){
    bitTime = dTime;
    receiveByteIndex = 0;  
    receiveByte = 0;
    
  }else if(dTime > bitTime * 0.75){  
    receiveByte |= thisBit << receiveByteIndex;
    receiveByteIndex++;
    if(receiveByteIndex > 7) {
      receiveByteIndex = 0;
      receiveByteAvailable = 1;
      receiveByteReturn = receiveByte;
      receiveByte = 0;
    }
  }  else {
    return;
  }
  
  lastEdge = now;
   
}

bool LaserTransceiver::bitDecode() {
  int analogVal = analogRead(ldrPin);
  if(analogVal >= analogThreshold) {
    return 1;
  } else {
    return 0;
  }
}
