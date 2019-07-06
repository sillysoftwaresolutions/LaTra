#ifndef LaserTransceiver_h
#define LaserTransceiver_h

#include "Arduino.h"

class LaserTransceiver {
  public:
    LaserTransceiver(byte pin, uint16_t baseLength);
    void transmit(String buf, uint8_t msgLength);
    bool getTransmitFinished();
    void tickTransmitter();

  private:
    int laserPin;

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

};

#endif
