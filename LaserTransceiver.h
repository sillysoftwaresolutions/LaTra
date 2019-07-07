#ifndef LaserTransceiver_h
#define LaserTransceiver_h

#include "Arduino.h"

class LaserTransceiver {
  public:
    LaserTransceiver(byte lPin, uint16_t baseLength, uint8_t ldPin);
    void transmit(String buf, uint8_t msgLength);
    bool getTransmitFinished();
    void tickTransmitter();
    void tickReceiver();
    void setReceiveThreshold(uint16_t val);
    bool getReceiveByteAvailable();
    String getReceive();

  private:
    bool bitDecode();
  
    uint8_t laserPin;
    uint8_t ldrPin;
    
    unsigned int halfClock;
    bool secondHalf = 0;
    char msg[16];
    byte msgIndex = 0;
    byte msgLength;
    unsigned long startTime;
    char transmitByte;
    byte byteIndex = 0;
    bool pause = 0;

    bool transmitFinished = 0;
    unsigned long halfStart = 0;
    uint16_t baseLength;

    int analogThreshold = 0;
    unsigned long lastEdge = 0;

    bool oldBit = 0;
    byte receiveByte = 0;
    byte receiveByteIndex = 0;
    bool receiveByteAvailable = 0;
    unsigned int bitTime;

};

#endif
