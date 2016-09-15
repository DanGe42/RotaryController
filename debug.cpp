#include "Arduino.h"
#include "debug.h"

volatile unsigned int encoderPos = 0;  // a counter for the dial
unsigned int lastReportedPos = 1;      // change management

void debugInterrupt(int digitalPin) {
  Serial.print("Digital pin ");
  Serial.print(digitalPin);
  Serial.print(" -> Interrupt ");
  Serial.println(digitalPinToInterrupt(digitalPin));
}

void printTime(unsigned long ms) {
  Serial.print("time = ");
  Serial.println(ms);
}

void printPosition() {
  if (lastReportedPos != encoderPos) {
    Serial.print("Index: ");
    Serial.println(encoderPos, DEC);
    lastReportedPos = encoderPos;
  }
}

void decrementPosition() {
  encoderPos--;
}

void incrementPosition() {
  encoderPos++;
}

