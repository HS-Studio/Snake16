#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <Wire.h>
#include <WiiChuck.h> // modified WiiChuck library #### https://github.com/HS-Studio/WiiChuck.git
#include <stdio.h>

enum Direction { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3, NONE = -1 };

class JoystickInput
{
    public:
    JoystickInput(TwoWire &wire = Wire1);

    void begin();
    void update();
    Direction getDirection();
    
    private:
    

    TwoWire &i2c;
    Accessory accessory;

    uint16_t joyXMin, joyXMax, joyXCenter;
    uint16_t joyYMin, joyYMax, joyYCenter;
    int16_t joy_x, joy_y;

    void calibrate();
    float customMap(long value, long in_min, long in_center, long in_max, long out_min, long out_max);
};

#endif