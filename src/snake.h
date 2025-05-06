#ifndef SNAKE_H
#define SNAKE_H

#pragma once

#include <deque>
#include "LGFX_SPI_ST7789.h"
#include "input.h"
#include <Wire.h>
#include <vector>

//enum Direction { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3, NONE = -1 };

#define TILE_SIZE 16
#define TILE_COUNT 16
#define TILES_PER_ROW 4

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 280

struct Segment 
{
  int x, y;
  Direction dir;
};

class SnakeGame
{
  public:
    SnakeGame(LGFX* display, int gridWidth, int gridHeight);

    void update();
    void render();
    void renderDebug();
    void changeDirection(Direction newDir);
    void grow();

    void spawnFruit();
    bool checkFruitCollision();
    bool isOccupied(int x, int y);

  private:
    LGFX* tft;
    //LGFX_Sprite* sprite;
    LGFX_Sprite* snakeSprite;
    LGFX_Sprite* backgroundSprite;
    LGFX_Sprite* scoreSprite;
    uint16_t* snakeTiles[TILE_COUNT];   // snake_tiles.raw
    uint16_t* gameTiles[TILE_COUNT];    // game_tiles.raw

    std::deque<Segment> snake;
    std::deque<Segment> digestingSegments;
    Direction currentDir;
    
    bool shouldGrow;
    //int tileSize;
    int gridWidth, gridHeight;
    int fruitX, fruitY;

    unsigned long lastUpdateTime;
    unsigned long moveInterval = 300;
    int gridX, gridY;
    int score = 0;
    int highscore = 0;
    std::vector<std::vector<uint8_t>> levelData;
    void drawTileToSprite(LGFX_Sprite* targetSprite, uint16_t* tile[], int tileIndex, int x, int y);
    void drawBackground();
    void drawScore();
    int getTileForSegment(int index);
    void resetGame();
    void loadLevel(const char* path);
    void loadTileSet(const char* path, uint16_t* tileArray[TILE_COUNT]);
    void initGraphics();
    void exit();
};

#endif
