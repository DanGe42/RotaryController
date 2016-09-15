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

#include "KeyControl.h"
// Uncomment this header to enable DEBUG_MODE. Then, open the serial monitor.
// #include "debug.h"

// Usually the rotary encoders three pins have the ground pin in the middle.
// Orientation of left and right is correct if you are looking at the three-pin
// side with the knob on top.
#define encoderPinA 2   // left
#define encoderPinB 3   // right

// Enables/disables keyboard input. Helps prevent runaway keyboard input.
#define keyboardSwitchPin 5

// Interrupt service routine vars. Used for debouncing.
volatile boolean A_set = false;
volatile boolean B_set = false;

// Keeps track of the last time the rotary encoder interrupted us
volatile unsigned long lastRotaryTurnTime = 0;
// How long we should wait after the last rotary turn before releasing the key
// 60 fps => 1000 ms / 60 frames => 16.667 ms / frame
#define KEYBOARD_RELEASE_DELAY_MS 16

KeyControl keyControl;

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), doEncoderB, CHANGE);

  pinMode(keyboardSwitchPin, INPUT_PULLUP);
  lastRotaryTurnTime = millis();

#ifdef DEBUG_MODE
  Serial.begin(9600);  // output
  while (!Serial);

  debugInterrupt(encoderPinA);
  debugInterrupt(encoderPinB);
  printTime(lastRotaryTurnTime);

  Serial.println("start");
#endif
}

// Actual work is done by interrupts further down. The main loop checks the
// keyboard toggle switch and releases keypresses after a timeout.
void loop() {
#ifdef DEBUG_MODE
  printPosition();
#endif

  switchKeyboard(digitalRead(keyboardSwitchPin));
  maybeExpireKeyPress();
}

// The switch line is pulled up (INPUT_PULLUP).
// If the switch is open (voltage pulled HIGH), then turn off keyboard.
// If the switch is closed (voltage pulled LOW), then turn on keyboard.
void switchKeyboard(int keyboardPinState) {
  if (keyControl.isEnabled()) {
    if (keyboardPinState == HIGH) {
      keyControl.disable();
      digitalWrite(LED_BUILTIN, LOW);
    }
  } else {
    if (keyboardPinState == LOW) {
      keyControl.enable();
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

// Release keypresses after some amount of time passes after the last keypress.
// Does not release keypresses if the rotary encoder continues turning.
//
// Interrupts are disabled in this function because this is a critical block to
// be run in the main loop. If interrupts interrupted this code, the value of
// `lastRotaryTurnTime` would be undefined, and the behavior of the `if` block
// would become nondeterministic.
void maybeExpireKeyPress() {
  noInterrupts();

  unsigned long currentTime = millis();
  // Times are unsigned. Compare first before subtracting.
  if (currentTime > lastRotaryTurnTime &&
      currentTime - lastRotaryTurnTime >= KEYBOARD_RELEASE_DELAY_MS) {
    keyControl.release();
  }

  interrupts();
}

// ## Interrupts ##

// Interrupt on A changing state. Move left.
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
#ifdef DEBUG_MODE
      incrementPosition();
#endif

      keyControl.right();
      lastRotaryTurnTime = millis();
    }
  }
}

// Interrupt on B changing state, same as A above. Move right.
void doEncoderB() {
  if (digitalRead(encoderPinB) != B_set) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if (B_set && !A_set) {
#ifdef DEBUG_MODE
      decrementPosition();
#endif

      keyControl.left();
      lastRotaryTurnTime = millis();
    }
  }
}
