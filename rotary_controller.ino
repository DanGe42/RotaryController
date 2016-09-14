/**
   rotary_controller.ino
   @author Daniel Ge

   A sketch to turn an Arduino Micro and a rotary encoder into a dumb gamepad in
   which turning the encoder in one direction sends one arrow key, and turning
   it the other direction sends the opposite arrow key.

   Credits:
   - Most of the interrupt handling code forthe rotary encoder's is derived from
     http://playground.arduino.cc/Main/RotaryEncoders, namely rafbuff's example.
   - The debouncing circuit I built is derived from a circuit drawing that I
     also found on http://playground.arduino.cc/Main/RotaryEncoders, namely
     _bse_'s example.
*/

#define VERBOSE

#include "Keyboard.h"
#include "KeyControl.h"

// usually the rotary encoders three pins have the ground pin in the middle
#define encoderPinA 2   // right
#define encoderPinB 3   // left

volatile unsigned int encoderPos = 0;  // a counter for the dial
unsigned int lastReportedPos = 1;      // change management

// interrupt service routine vars
boolean A_set = false;
boolean B_set = false;

// Control whether we read keyboard input. Helps us avoid runaway keyboard input
#define keyboardSwitchPin 5
KeyControl keyControl;

void debugInterrupt(int digitalPin) {
  Serial.print("Digital pin ");
  Serial.print(digitalPin);
  Serial.print(" -> Interrupt ");
  Serial.println(digitalPinToInterrupt(digitalPin));
}

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), doEncoderB, CHANGE);

  pinMode(keyboardSwitchPin, INPUT_PULLUP);

  Serial.begin(9600);  // output
  while (!Serial);
  debugInterrupt(encoderPinA);
  debugInterrupt(encoderPinB);
  Serial.println("start");
}

// main loop, work is done by interrupt service routines, this one only prints
// stuff
void loop() {
  if (lastReportedPos != encoderPos) {
    Serial.print("Index:");
    Serial.println(encoderPos, DEC);
    lastReportedPos = encoderPos;
  }

  switchKeyboard(digitalRead(keyboardSwitchPin));
}

// If the switch is open, turn off keyboard
// If the switch is closed, turn on keyboard
void switchKeyboard(int keyboardPinState) {
  if (keyControl.isEnabled()) {
    if (keyboardPinState == HIGH) {
      keyControl.enable();
    }
  } else {
    if (keyboardPinState == LOW) {
      keyControl.disable();
    }
  }
}

// Interrupt on A changing state
void doEncoderA() {
  // In the example code, both interrupt handlers had a
  // `if (rotating) { delay(1); }` line to help with debouncing. This is
  // potentially buggy though: notice that the documentation for
  // attachInterrupt[0] states that `delay()` won't work within the handler.
  //
  // In reality, only one conditional to do software debouncing is really
  // necessary. The rest can be improved via a separate debouncing circuit.
  //
  // [0] https://www.arduino.cc/en/Reference/AttachInterrupt

  // Test transition, did things really change?
  if (digitalRead(encoderPinA) != A_set) { // debounce once more
    A_set = !A_set;

    // adjust counter + if A leads B
    if (A_set && !B_set) {
      encoderPos += 1;
    }
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB() {
  if (digitalRead(encoderPinB) != B_set) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if (B_set && !A_set) {
      encoderPos -= 1;
    }
  }
}

