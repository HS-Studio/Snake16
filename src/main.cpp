#include <Arduino.h>
#include <LittleFS.h>
#include "snake.h"
#include "LGFX_SPI_ST7789.h" // deine Display-Konfiguration hier

#include <Wire.h>
#include <WiiChuck.h>

LGFX tft;
SnakeGame* game;
JoystickInput input(Wire1); 

void setup() {
  Serial.begin(115200);
  delay(2000);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  input.begin();

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }

  game = new SnakeGame(&tft, 12, 12);
}

void loop() {
  game->changeDirection(input.getDirection());
  game->update();
}
