/**
   A simple class to manage combined state transitions and keyboard action.
*/

#ifndef __KEYCONTROL_H__
#define __KEYCONTROL_H__

class KeyControl {
    boolean enabled;
    int pressedKey;

  public:
    KeyControl() : enabled(false), pressedKey(0) {}
    void left();
    void right();
    void release();
    void enable();
    void disable();
    boolean isEnabled() const;
    void printCurrentState() const;

  private:
    void switchKey(int currentKey, int nextKey);
    boolean isPressed() const;
};

#endif
