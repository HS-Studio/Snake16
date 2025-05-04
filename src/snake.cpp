#include "snake.h"
#include <FS.h>
#include <LittleFS.h>
#include <Arduino.h>

SnakeGame::SnakeGame(LGFX* display, int gridWidth, int gridHeight)
  : tft(display), gridWidth(gridWidth), gridHeight(gridHeight),
    currentDir(RIGHT), shouldGrow(false), lastUpdateTime(0)
{
  loadLevel("/bg.csv");
  initGraphics();
  resetGame();
}

void SnakeGame::update()
{
  unsigned long now = millis();
  if (now - lastUpdateTime < moveInterval) return;
  lastUpdateTime = now;

  Segment head = snake.front();
  switch (currentDir) {
    case UP: head.y--; break;
    case DOWN: head.y++; break;
    case LEFT: head.x--; break;
    case RIGHT: head.x++; break;
  }
  head.dir = currentDir;

  // Wände begrenzen
  if (head.x < 0) head.x = gridWidth - 1;
  if (head.x >= gridWidth) head.x = 0;
  if (head.y < 0) head.y = gridHeight - 1;
  if (head.y >= gridHeight) head.y = 0;

  // Kollision mit sich selbst
  if (isOccupied(head.x, head.y)) {
    resetGame();
    return;
  }

  snake.push_front(head);
  if (checkFruitCollision()) 
  {
    shouldGrow = true;
    spawnFruit();
  }

  if (!shouldGrow) snake.pop_back();
  else shouldGrow = false;

  render();
}

void SnakeGame::resetGame()
{
  snake.clear();
  snake.push_back({5, 5, RIGHT});
  snake.push_back({5, 5, RIGHT});
  snake.push_back({5, 5, RIGHT});
  currentDir = RIGHT;
  shouldGrow = false;
  spawnFruit();
}

void SnakeGame::drawTileToSprite(LGFX_Sprite* targetSprite, uint16_t* tile[], int index, int x, int y) {
  if (index < 0 || index >= TILE_COUNT || !tile[index]) return;
  targetSprite->pushImage(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tile[index], TFT_TRANSPARENT);
}

void SnakeGame::drawBackground() {
  for (int y = 0; y < levelData.size(); y++) {
    for (int x = 0; x < levelData[y].size(); x++) {
      int tileIndex = levelData[y][x];
      drawTileToSprite(backgroundSprite, gameTiles, tileIndex, x, y);
    }
  }
}

void SnakeGame::render()
{
  drawBackground();
  snakeSprite->fillSprite(TFT_TRANSPARENT);

  for (int i = 0; i < snake.size(); i++) {
    int tile = getTileForSegment(i);
    drawTileToSprite(snakeSprite, snakeTiles, tile, snake[i].x, snake[i].y);
  }
  
  drawTileToSprite(snakeSprite, gameTiles, random(1), fruitX, fruitY);

  snakeSprite->pushSprite(0, 0, TFT_TRANSPARENT);
  backgroundSprite->pushSprite(TILE_SIZE, TILE_SIZE, TFT_TRANSPARENT);
  //snakeSprite->pushSprite(TILE_SIZE, TILE_SIZE);
}


bool SnakeGame::checkFruitCollision()
{
  return snake.front().x == fruitX && snake.front().y == fruitY;
}

bool SnakeGame::isOccupied(int x, int y)
{
  for (auto& s : snake)
    if (s.x == x && s.y == y) return true;
  return false;
}

void SnakeGame::spawnFruit() {
  do {
    fruitX = random(0, gridWidth);
    fruitY = random(0, gridHeight);
  } while (isOccupied(fruitX, fruitY));
}

int SnakeGame::getTileForSegment(int i)
{
  if (i == 0)
    return snake[i].dir; // Kopf

  if (i == snake.size() - 1)
    return 12 + snake[i - 1].dir; // Schwanz

  Direction fromDir = snake[i].dir;       // Richtung dieses Segments
  Direction toDir = snake[i - 1].dir;     // Richtung des nächsten Segments (zum Kopf hin)

  // Kurven
  if ((fromDir == UP && toDir == RIGHT) || (fromDir == LEFT && toDir == DOWN)) return 4;
  if ((fromDir == UP && toDir == LEFT)  || (fromDir == RIGHT && toDir == DOWN)) return 5;
  if ((fromDir == DOWN && toDir == RIGHT) || (fromDir == LEFT && toDir == UP)) return 6;
  if ((fromDir == DOWN && toDir == LEFT)  || (fromDir == RIGHT && toDir == UP)) return 7;

  // Gerade
  if ((fromDir == UP && toDir == DOWN) || (fromDir == DOWN && toDir == UP) || (fromDir == UP && toDir == UP) || (fromDir == DOWN && toDir == DOWN)) return 9;
  if ((fromDir == LEFT && toDir == RIGHT) || (fromDir == RIGHT && toDir == LEFT) || (fromDir == LEFT && toDir == LEFT) || (fromDir == RIGHT && toDir == RIGHT)) return 8;

  // Fallback
  return 8;
}

void SnakeGame::changeDirection(Direction newDir)
{
  if (newDir == NONE) 
  {
    return;
  }
  else if ((newDir + 2) % 4 != currentDir) 
  {
    currentDir = newDir;
  }
}

void SnakeGame::grow() {
  shouldGrow = true;
}

void SnakeGame::loadLevel(const char* path)
{
  File file = LittleFS.open(path, "r");
  if (!file)
  {
    Serial.println("Level-Datei nicht gefunden!");
    return;
  }

  levelData.clear();

  while (file.available())
  {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    std::vector<uint8_t> row;
    int from = 0;

    while (from < line.length())
    {
      int to = line.indexOf(',', from);
      if (to == -1) to = line.length();
      row.push_back(line.substring(from, to).toInt());
      from = to + 1;
    }

    levelData.push_back(row);
  }

  file.close();
}

void SnakeGame::initGraphics()
{
  backgroundSprite = (LGFX_Sprite *)malloc(sizeof(LGFX_Sprite));
  new (backgroundSprite) LGFX_Sprite(tft);
  //snakeSprite->setPsram(true);
  backgroundSprite->setColorDepth(16);
  backgroundSprite->createSprite(gridWidth * TILE_SIZE, gridHeight * TILE_SIZE);
  backgroundSprite->fillSprite(TFT_BLACK);
  backgroundSprite->setRotation(tft->getRotation());

  snakeSprite = (LGFX_Sprite *)malloc(sizeof(LGFX_Sprite));
  new (snakeSprite) LGFX_Sprite(backgroundSprite);
  //snakeSprite->setPsram(true);
  snakeSprite->setColorDepth(16);
  snakeSprite->createSprite(gridWidth * TILE_SIZE, gridHeight * TILE_SIZE);
  snakeSprite->fillSprite(TFT_TRANSPARENT);
  snakeSprite->setRotation(tft->getRotation());

  loadTileSet("/snake_tiles.raw", snakeTiles);
  loadTileSet("/game_tiles.raw", gameTiles);
}

void SnakeGame::loadTileSet(const char* path, uint16_t* tileArray[TILE_COUNT])
{
  File file = LittleFS.open(path, "r");
  if (!file || file.size() == 0) {
    Serial.printf("Fehler beim Laden von %s\n", path);
    return;
  }

  int sheetWidth = TILE_SIZE * TILES_PER_ROW;

  for (int i = 0; i < TILE_COUNT; i++) {
    tileArray[i] = (uint16_t*)ps_malloc(TILE_SIZE * TILE_SIZE * sizeof(uint16_t));
    if (!tileArray[i]) {
      Serial.printf("Fehler beim Allocieren von tileArray[%d]\n", i);
      continue;
    }

    int tx = (i % TILES_PER_ROW) * TILE_SIZE;
    int ty = (i / TILES_PER_ROW) * TILE_SIZE;
    int offset = (ty * sheetWidth + tx) * 2;

    for (int row = 0; row < TILE_SIZE; row++) {
      file.seek(offset + row * sheetWidth * 2);
      file.read((uint8_t*)&tileArray[i][row * TILE_SIZE], TILE_SIZE * 2);
    }
  }

  file.close();
}

void SnakeGame::exit()
{
  snakeSprite->deleteSprite();
  snakeSprite->~LGFX_Sprite();
  free(snakeSprite);
  
  backgroundSprite->deleteSprite();
  backgroundSprite->~LGFX_Sprite();
  free(backgroundSprite);

  for (int i = 0; i < TILE_COUNT; i++) {
    if (snakeTiles[i]) {
      free(snakeTiles[i]);
      snakeTiles[i] = nullptr;
    }
    if (gameTiles[i]) {
      free(gameTiles[i]);
      gameTiles[i] = nullptr;
    }
  }
}
