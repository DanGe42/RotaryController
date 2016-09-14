#include "Keyboard.h"
#include "KeyControl.h"

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
  this->switchKey(KEY_RIGHT_ARROW, KEY_LEFT_ARROW);
}

void KeyControl::right() {
  this->switchKey(KEY_LEFT_ARROW, KEY_RIGHT_ARROW);
}

void KeyControl::release() {
  if (!this->enabled) return;
  if (this->pressedKey == 0) return;

  Keyboard.release(this->pressedKey);
  this->pressedKey = 0;
}

void KeyControl::disable() {
  if (!this->enabled) return;
  this->enabled = false;
  this->pressedKey = 0;
  Keyboard.releaseAll();
  Keyboard.end();

  Serial.println("Keyboard off");
}

void KeyControl::enable() {
  this->enabled = true;
  Keyboard.begin();

  Serial.println("Keyboard on");
}

boolean KeyControl::isEnabled() const {
  return this->enabled;
}

void KeyControl::printCurrentState() const {
  Serial.print("enabled=");
  Serial.print(this->enabled ? "true" : "false");
  Serial.print("\tpressedKey=");
  if (this->pressedKey == KEY_LEFT_ARROW) {
    Serial.println("LEFT");
  } else if (this->pressedKey == KEY_RIGHT_ARROW) {
    Serial.println("RIGHT");
  } else {
    Serial.println(this->pressedKey);
  }
}
