#include "Arduino.h"
#include "LaserTransceiver.h"

LaserTransceiver::LaserTransceiver(uint8_t lPin, uint16_t baseL, uint8_t ldPin) {
  laserPin = lPin;
  ldrPin = ldPin;
  baseLength = baseL;
  halfClock = 2 * baseLength;
  bitTime = baseLength;
  pinMode(laserPin, OUTPUT);
  ldrInverted = false;
  laserInverted = false;
}

void LaserTransceiver::transmit(char * message) {
  //Serial.println(message);
  msg = message;
  msgLength = strlen(msg); 
  transmitFinished = 0;
  halfClock = 2 * baseLength; // vor dem Senden wird die doppelte baseLength gewartet
  halfStart = micros();
  bitLengthTransmitted = 0;  // gibt an, ob mit dem preamble die baseLength/bitLength gesendet wurde
  msgIndex = 0;
  
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

void LaserTransceiver::setReceiveThreshold(uint16_t val) {
  analogThreshold = val;
}

uint16_t LaserTransceiver::getAnalogValLaserHIGH() {
  writeLaser(HIGH);
  delay(1);
  uint16_t ret = analogRead(ldrPin);
  delay(1);
  writeLaser(LOW);
  return ret;
}

uint16_t LaserTransceiver::getAnalogVal() {
  return analogRead(ldrPin);
}

void LaserTransceiver::tickTransmitter() {
  unsigned long now = micros();
  
  if(now - halfStart >= halfClock) {
    halfClock = baseLength / 2;
    halfStart = now;

    //preamble
    if(msgIndex == 0) {
      if(!bitLengthTransmitted) {  // => Signal für eine baseLength HIGH
        halfClock = baseLength;
        writeLaser(HIGH);
        bitLengthTransmitted = 1;
        return;
      } else {
        halfClock = baseLength / 2;
        writeLaser(LOW);
        msgIndex=1;// => Start Manchester Coding in half baseLength
        return;  
      }
      
    }

    //send data with Manchester Code
    // Ein Bit wird in der Zeit einer baseLength gesendet.
    // eine positive Flanke (erst negativ; dann positiv) repräsentiert eine 1
    // eine negative Flanke ...                                             0
    // Ein Bit besteht dabei aus 2 Teilen (mit bool secondHalf wird angegeben, welche gerade gesendet wird)

    transmitByte = msg[msgIndex - 1];
    if(!secondHalf) { 
      writeLaser(!bitRead(transmitByte, byteIndex)); 
      secondHalf = 1;
    } else {
      writeLaser(bitRead(transmitByte, byteIndex)); 
      secondHalf = 0;

      // Counter erhöhen: 
      byteIndex++;
      if(byteIndex >= 8) {
        byteIndex = 0;
        msgIndex++;
        writeLaser(LOW);
        if(msgIndex > msgLength) {
          transmitFinished = 1;
          msgIndex = 0;
          bitLengthTransmitted = 0;
          halfClock = 2 * baseLength;
          writeLaser(LOW);
        }
      }
      
    } 

  } 
}

void LaserTransceiver::writeLaser(bool state){
  digitalWrite(laserPin, !(state==laserInverted)); 
}

void LaserTransceiver::tickReceiver() {
  bool thisBit = this->bitDecode();

  if(thisBit == oldBit) return; // keine Flanke => return
  oldBit = thisBit;  

  unsigned long now = micros();
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
    return !ldrInverted;
  } else {
    return ldrInverted;
  }
}

void LaserTransceiver::setLaserInverted(bool val){
  laserInverted = val;
  writeLaser(LOW);
}
void LaserTransceiver::setLdrInverted(bool val){
  ldrInverted = val;
}
