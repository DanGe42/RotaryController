#include "Arduino.h"
#include "Keyboard.h"
#include "KeyControl.h"

#define LEFT_KEY  KEY_LEFT_ARROW
#define RIGHT_KEY KEY_RIGHT_ARROW

void KeyControl::switchKey(int keyToRelease, int keyToPress) {
  if (!this->enabled) return;
  if (this->pressedKey == keyToPress) return;

  if (this->pressedKey == keyToRelease) {
    Keyboard.release(this->pressedKey);
  }
  this->pressedKey = keyToPress;
  Keyboard.press(this->pressedKey);
}

void KeyControl::left() {
  this->switchKey(RIGHT_KEY, LEFT_KEY);
}

void KeyControl::right() {
  this->switchKey(LEFT_KEY, RIGHT_KEY);
}

void KeyControl::release() {
  if (!this->enabled) return;
  if (!this->isPressed()) return;

  Keyboard.release(this->pressedKey);
  this->pressedKey = 0;
}

void KeyControl::disable() {
  if (!this->enabled) return;
  this->enabled = false;
  this->pressedKey = 0;
  Keyboard.releaseAll();
  Keyboard.end();
}

void KeyControl::enable() {
  this->enabled = true;
  Keyboard.begin();
}

boolean KeyControl::isEnabled() const {
  return this->enabled;
}

boolean KeyControl::isPressed() const {
  return this->pressedKey != 0;
}

void KeyControl::printCurrentState() const {
  Serial.print("enabled=");
  Serial.print(this->enabled ? "true" : "false");
  Serial.print("\tpressedKey=");
  if (this->pressedKey == LEFT_KEY) {
    Serial.println("LEFT");
  } else if (this->pressedKey == RIGHT_KEY) {
    Serial.println("RIGHT");
  } else {
    Serial.println(this->pressedKey);
  }
}
