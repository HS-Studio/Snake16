#include "input.h"

JoystickInput::JoystickInput(TwoWire &wire)
  : i2c(wire), accessory(wire),
    joyXMin(0), joyXMax(0), joyXCenter(0),
    joyYMin(0), joyYMax(0), joyYCenter(0),
    joy_x(0), joy_y(0)
{
  Accessory accessory(i2c);
}

void JoystickInput::begin()
{
  Serial.println("[INFO] Joystick Kalibrieren...");
  Serial.println("[INFO] Den Joystick loslassen bevor das Programm gestartet wird.");
  Serial.println("[INFO] Nach dem Start den Joystick in alle Richtungen bewegen");
  Serial.println();

  i2c.begin();
  delay(500);
  accessory.begin();

  if (i2c.requestFrom(0x52, 1))
  {
    Serial.println("[INFO] WiiChuck gefunden!");
    if (accessory.type == WIICLASSIC)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyXLeft();
      joy_y = accessory.getJoyYLeft();

      joyXMin = accessory.getJoyXLeft() - 10;
      joyXMax = accessory.getJoyXLeft() + 10;
      joyXCenter = accessory.getJoyXLeft();

      joyYMin = accessory.getJoyYLeft() - 10;
      joyYMax = accessory.getJoyYLeft() + 10;
      joyYCenter = accessory.getJoyYLeft();
    }
    else if (accessory.type == NUNCHUCK)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyX();
      joy_y = accessory.getJoyY();

      joyXMin = accessory.getJoyX() - 10;
      joyXMax = accessory.getJoyX() + 10;
      joyXCenter = accessory.getJoyX();

      joyYMin = accessory.getJoyY() - 10;
      joyYMax = accessory.getJoyY() + 10;
      joyYCenter = accessory.getJoyY();
    }
  }
}

void JoystickInput::update()
{
  if (Wire1.requestFrom(0x52, 1))
  {
    if (accessory.type == WIICLASSIC)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyXLeft();
      joy_y = accessory.getJoyYLeft();
    }
    else if (accessory.type == NUNCHUCK)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyX();
      joy_y = accessory.getJoyY();
    }
  }
  else
  {
    Serial.println("[INFO] WiiChuck nicht gefunden!"); // Joystick nicht gefunden
    Wire1.end(); // Beende den I2C-Bus
  }
  calibrate();
}

void JoystickInput::calibrate()
{
  // Joystick calibration    // Joystick Kalibrierung
  if (joy_x < joyXMin) joyXMin = joy_x;
  if (joy_x > joyXMax) joyXMax = joy_x;
  if (joy_y < joyYMin) joyYMin = joy_y;
  if (joy_y > joyYMax) joyYMax = joy_y;

  joy_x = customMap(joy_x, joyXMin, joyXCenter, joyXMax, -127, 127);
  joy_y = customMap(joy_y, joyYMin, joyYCenter, joyYMax, -127, 127);
}

Direction JoystickInput::getDirection()
{
  // Joystick movement    // Joystick Bewegung
  // 0: up | 1: right | 2: down | 3: left
  update();
  if (joy_x < -75) return LEFT;  
  else if (joy_x > 75) return RIGHT;
  else if (joy_y < -75) return DOWN;
  else if (joy_y > 75) return UP; 

  return NONE; // no movement
}

float JoystickInput::customMap(long value, long in_min, long in_center, long in_max, long out_min, long out_max)
{
  if (value < in_center)
  {
    return (float)(value - in_center) / (in_center - in_min) * (out_min) * -1; // Skalierung für den Bereich links vom Mittelpunkt
  }
  else
  {
    return (float)(value - in_center) / (in_max - in_center) * (out_max); // Skalierung für den Bereich rechts vom Mittelpunkt
  }
}