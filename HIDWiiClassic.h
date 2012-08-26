
#ifndef HIDJoystick_h
#define HIDJoystick_h

//================================================================================
//================================================================================
// Joystick
// Implemented in HID.cpp

class Joystick_
{
public:
   Joystick_();
   void begin(void);
   void end(void);
   void move(uint8_t throttle, uint8_t brake, uint8_t x, uint8_t y, uint8_t buttons);
};
extern Joystick_ Joystick;

#endif /* if defined(HIDJoystick_h) */
