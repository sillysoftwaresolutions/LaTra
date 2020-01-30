#ifndef LaserTransceiver_h
#define LaserTransceiver_h

#include "Arduino.h"

class LaserTransceiver {
  public:
    LaserTransceiver(byte lPin, uint16_t baseLength, uint8_t ldPin);
    void transmit(char * message);
    bool getTransmitFinished();
    void tickTransmitter();
    void tickReceiver();
    void setReceiveThreshold(uint16_t val);
    byte getReceiveByte();
    bool getReceiveByteAvailable();
    uint16_t getAnalogVal();
    uint16_t getAnalogValLaserHIGH();

    //Settings:
    void setLaserInverted(bool val);
    void setLdrInverted(bool val);
    
  private:
    void writeLaser(bool state);
    bool bitDecode();
    bool laserInverted;
    bool ldrInverted;
    uint8_t laserPin;
    uint8_t ldrPin;
    
    unsigned int halfClock;
    bool secondHalf = 0;
    char *msg;
    byte msgIndex = 0;
    byte msgLength;
    unsigned long startTime;
    char transmitByte;
    byte byteIndex = 0;
    bool pause = 0;

    bool transmitFinished = 0;
    bool bitLengthTransmitted = 0;
    unsigned long halfStart = 0;
    uint16_t baseLength;

    int analogThreshold = 650;
    unsigned long lastEdge = 0;

    bool oldBit = 0;
    byte receiveByte = 0;
    byte receiveByteIndex = 0;
    byte receiveByteReturn = 0;
    bool receiveByteAvailable = 0;
    unsigned int bitTime = 0;

};

#endif
