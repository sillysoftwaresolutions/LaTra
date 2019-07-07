#include "Arduino.h"
#include "LaserTransceiver.h"

LaserTransceiver::LaserTransceiver(uint8_t lPin, uint16_t baseL, uint8_t ldPin) {
  laserPin = lPin;
  ldrPin = ldPin;
  baseLength = baseL;
  halfClock = baseLength / 2;
  pinMode(laserPin, OUTPUT);
}

void LaserTransceiver::transmit(String buf, uint8_t len) {
  buf.toCharArray(msg, sizeof(msg));
  msgLength = len; 
  transmitFinished = 0;
  
}

bool LaserTransceiver::getTransmitFinished() {
  return transmitFinished;
}

bool LaserTransceiver::getReceiveByteAvailable() {
  return receiveByteAvailable;
}

void LaserTransceiver::tickTransmitter() {
  unsigned long now = millis();

  if(now - halfStart >= halfClock) {
    halfClock = baseLength / 2;
    halfStart = now;
    transmitByte = msg[msgIndex];
    //Serial.println(bitRead(transmitByte, 7 - byteIndex));
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

void LaserTransceiver::tickReceiver() {
  bool thisBit = this->bitDecode();
  
  // wenn keine Flanke => nicht relevant
  // wird als erstes geprüft, damit nicht unnötig die dTime berechnet werden muss
  if(thisBit != oldBit)return; 
  oldBit = thisBit;  // jetzt brauche ich oldBit eig nicht mehr

  unsigned long now = millis();
  unsigned long dTime = now - lastEdge;
  
  // wenn die Zeit seit der letzten Flanke größer als 2 bitTime ist, dann beginnt ein neuer Frame
  // -> die bitTime ist ja eig noch nicht definiert ?!
  // -> technisch würde 1,25 * bitTime reichen. Ich glaube so ist es durch die Multiplikation mit 2
  //    (die ja bei jeder Flanke gemacht werden muss um zu bestimmen, ob es noch eine bitTime ist) insgesamt schneller
  // -> ob es eine positive oder negative Flanke ist, ist ja eig nicht relevant
  if(dTime > bitTime * 2){
    // => neuer Frame
    lastEdge = now;  // für die Clocksync
    receiveByteIndex = 255;  // da ich in dem Byte noch platz hab verwende ich es, um zu speichern, dass ich grad
                             // die clockSync mache 
  }else if(receiveByteIndex = 255){
    // => zweite Flanke der clkSync 
    // die "Übertragene Zeitspanne" ist die neue bitTime
    bitTime = dTime;
    receiveByteIndex = 0;  // alles wird resettet gesetzt
    receiveByte = 0;
  }else if(dTime > bitTime * 0.75){  // ich weiß nicht, ob die Multiplikation mit 0,75 für die Rechenleistung so gut ist
    // => Übertragenes bit, da alle anderen Fälle sind bereits "abgefangen" worden sind
    receiveByteIndex++;
    // da wusste ich nicht, wie man dann das <receiveByteIndex>te bit des recieveBytes beschreiben kann. Der Wert, der da rein
    // geschrieben werden soll ist auf jeden Fall thisBit. Ich glaube sowas in der Art:
    receiveByte |= thisBit << receiveByteIndex;
  }  else {
    return;  // => halbe bitTime => wird Ignoriert (durch das return wird diese Flanke nicht als lastRelevantEdge gespeichert)
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
