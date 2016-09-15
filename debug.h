#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_MODE

extern volatile unsigned int encoderPos;
extern unsigned int lastReportedPos;

void debugInterrupt(int digitalPin);
void printTime(unsigned long ms);

void printPosition();
void decrementPosition();
void incrementPosition();

#endif
