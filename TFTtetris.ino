//ILI9341_Tetris
//ver1.0 2023.9

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include <Fonts/FreeSans18pt7b.h>
//#include <Fonts/FreeSans12pt7b.h>
#include <SPI.h>

//SPI0
#define TFT_CS 17    // CS
#define TFT_RST 22   // Reset
#define TFT_DC 28    // D/C
#define TFT_MOSI 19  // SDI(MOSI)
#define TFT_SCK 18   // SCK

#define TOUCH_CS 20
#define TFT_OUT 16

XPT2046_Touchscreen ts(TOUCH_CS);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);

static const int FIELD_WIDTH = 12;
static const int FIELD_HEIGHT = 24;
static const int MINO_WIDTH = 4;
static const int MINO_HEIGHT = 4;
const int blockSize = 10;
int minoType, minoAngle, minoX, minoY;
int interval, score;
uint32_t t;
bool gameover = false;

enum {
  MINO_TYPE_I,
  MINO_TYPE_O,
  MINO_TYPE_S,
  MINO_TYPE_Z,
  MINO_TYPE_J,
  MINO_TYPE_L,
  MINO_TYPE_T,
  MINO_TYPE_MAX
};

enum {
  MINO_ANGLE_0,
  MINO_ANGLE_90,
  MINO_ANGLE_180,
  MINO_ANGLE_270,
  MINO_ANGLE_MAX
};

int field[FIELD_HEIGHT][FIELD_WIDTH];
int displayBuffer[FIELD_HEIGHT][FIELD_WIDTH];

int minoShapes[MINO_TYPE_MAX][MINO_ANGLE_MAX][MINO_WIDTH][MINO_HEIGHT] = {
  // MINO_TYPE_I
  {
    // MINO_ANGLE_0
    {
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 0, 0 },
    // MINO_ANGLE_90
    {
      0, 0, 0, 0,
      0, 0, 0, 0,
      1, 1, 1, 1,
      0, 0, 0, 0 },
    // MINO_ANGLE_180
    {
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0 },
    // MINO_ANGLE_270
    {
      0, 0, 0, 0,
      1, 1, 1, 1,
      0, 0, 0, 0,
      0, 0, 0, 0 } },

  // MINO_TYPE_O
  {
    // MINO_ANGLE_0
    {
      0, 0, 0, 0,
      0, 2, 2, 0,
      0, 2, 2, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_90
    {
      0, 0, 0, 0,
      0, 2, 2, 0,
      0, 2, 2, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_180
    {
      0, 0, 0, 0,
      0, 2, 2, 0,
      0, 2, 2, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_270
    {
      0, 0, 0, 0,
      0, 2, 2, 0,
      0, 2, 2, 0,
      0, 0, 0, 0 } },

  // MINO_TYPE_S
  {
    // MINO_ANGLE_0
    {
      0, 0, 0, 0,
      0, 3, 3, 0,
      3, 3, 0, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_90
    {
      0, 3, 0, 0,
      0, 3, 3, 0,
      0, 0, 3, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_180
    {
      0, 0, 0, 0,
      0, 3, 3, 0,
      3, 3, 0, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_270
    {
      0, 0, 0, 0,
      0, 3, 0, 0,
      0, 3, 3, 0,
      0, 0, 3, 0 } },

  // MINO_TYPE_Z
  {
    // MINO_ANGLE_0
    {
      0, 0, 0, 0,
      4, 4, 0, 0,
      0, 4, 4, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_90
    {
      0, 0, 0, 0,
      0, 0, 4, 0,
      0, 4, 4, 0,
      0, 4, 0, 0 },
    // MINO_ANGLE_180
    {
      0, 0, 0, 0,
      0, 4, 4, 0,
      0, 0, 4, 4,
      0, 0, 0, 0 },
    // MINO_ANGLE_270
    {
      0, 0, 4, 0,
      0, 4, 4, 0,
      0, 4, 0, 0,
      0, 0, 0, 0 } },

  // MINO_TYPE_J
  {
    // MINO_ANGLE_0
    {
      0, 0, 5, 0,
      0, 0, 5, 0,
      0, 5, 5, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_90
    {
      0, 0, 0, 0,
      5, 5, 5, 0,
      0, 0, 5, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_180
    {
      0, 0, 0, 0,
      0, 5, 5, 0,
      0, 5, 0, 0,
      0, 5, 0, 0 },
    // MINO_ANGLE_270
    {
      0, 0, 0, 0,
      0, 5, 0, 0,
      0, 5, 5, 5,
      0, 0, 0, 0 } },

  // MINO_TYPE_L
  {
    // MINO_ANGLE_0
    {
      0, 6, 0, 0,
      0, 6, 0, 0,
      0, 6, 6, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_90
    {
      0, 0, 0, 0,
      0, 0, 6, 0,
      6, 6, 6, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_180
    {
      0, 0, 0, 0,
      0, 6, 6, 0,
      0, 0, 6, 0,
      0, 0, 6, 0 },
    // MINO_ANGLE_270
    {
      0, 0, 0, 0,
      0, 6, 6, 6,
      0, 6, 0, 0,
      0, 0, 0, 0 } },

  // MINO_TYPE_T
  {
    // MINO_ANGLE_0
    {
      0, 0, 0, 0,
      7, 7, 7, 0,
      0, 7, 0, 0,
      0, 0, 0, 0 },
    // MINO_ANGLE_90
    {
      0, 0, 0, 0,
      0, 7, 0, 0,
      0, 7, 7, 0,
      0, 7, 0, 0 },
    // MINO_ANGLE_180
    {
      0, 0, 0, 0,
      0, 0, 7, 0,
      0, 7, 7, 7,
      0, 0, 0, 0 },
    // MINO_ANGLE_270
    {
      0, 0, 7, 0,
      0, 7, 7, 0,
      0, 0, 7, 0,
      0, 0, 0, 0 } }
};

void display(int mode);
bool isHit(int argMinoX, int argMinoY, int argMinoType, int argMinoAngle);
void resetMino();
void moveMino();
void MinoUpdate();
void init();

void setup() {
  SPI.setTX(TFT_MOSI);
  SPI.setSCK(TFT_SCK);
  tft.begin();
  tft.setTextSize(3);
  ts.begin();
  ts.setRotation(3);
  init();
}

void loop() {
  while (1) {
    t = micros();
    resetMino();
    while (gameover == false) {
      moveMino();
      interval = 750000 - 20000 * score;
      if ((micros() - t) > interval) {
        MinoUpdate();
        display(1);
      }
    }

    //in case of gameover
    delay(50);
    tft.fillScreen(ILI9341_BLACK);
    while (ts.touched() != true) {
      tft.setRotation(4);
      tft.setCursor(45, 50);
      tft.setTextColor(ILI9341_RED);
      tft.printf("GAME OVER\n");
      tft.setCursor(45, 80);
      tft.setTextColor(ILI9341_WHITE);
      tft.print("score: ");
      tft.print(String(score));
      delay(200);
    }
    init();
  }
}

void init() {
  score = 0;
  gameover = false;
  minoType = minoAngle = minoX = minoY = 0;
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(3);
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      field[i][j] = displayBuffer[i][j] = 0;
    }
  }
  //create wall
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    field[i][0] = 8;
    field[i][FIELD_WIDTH - 1] = 8;
  }
  //create bottom
  for (int i = 0; i < FIELD_WIDTH; ++i) {
    field[FIELD_HEIGHT - 1][i] = 8;
  }
  display(0);
  delay(800);
}
void display(int mode) {
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      displayBuffer[i][j] = field[i][j];
    }
  }
  if (mode == 1) {
    for (int i = 0; i < MINO_HEIGHT; ++i) {
      for (int j = 0; j < MINO_WIDTH; ++j) {
        displayBuffer[minoY + i][minoX + j] += minoShapes[minoType][minoAngle][i][j];
      }
    }
  }
  for (int i = 2; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      switch (displayBuffer[i][j]) {
        case 0:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_BLACK);
          break;
        case 1:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_CYAN);
          break;
        case 2:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_YELLOW);
          break;
        case 3:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_GREEN);
          break;
        case 4:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_RED);
          break;
        case 5:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_BLUE);
          break;
        case 6:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_ORANGE);
          break;
        case 7:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_PURPLE);
          break;
        case 8:
          tft.fillRect((300 + (blockSize + 1) * (-i)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_WHITE);
          break;
        default:
          //tft.fillRect((300 + (blockSize + 1) * (-i - 2)), (60 + (blockSize + 1) * j), blockSize, blockSize, ILI9341_WHITE);
          break;
      }
    }
  }
  tft.drawLine(20, 0, 20, 240, ILI9341_WHITE);
  tft.drawLine(20, 120, 0, 120, ILI9341_WHITE);
  delay(50);
}

void moveMino() {
  TS_Point tPoint;
  int state = 0;
  bool bTouch = ts.touched();
  if (bTouch == true) {
    tPoint = ts.getPoint();
    delay(40);
    if (tPoint.x < 1000) {
      state = 1;  //down
    } else if (tPoint.x > 3200) {
      if (tPoint.y > 2200) {
        state = 2;  //left
      } else {
        state = 3;  //right
      }
    } else {
      state = 4;  //rotate
    }
  }
  switch (state) {
    case 1:  //down
      if (!isHit(minoX, minoY + 1, minoType, minoAngle)) {
        ++minoY;
      }
      break;
    case 2:  //left
      if (!isHit(minoX - 1, minoY, minoType, minoAngle)) {
        --minoX;
      }
      break;
    case 3:  //right
      if (!isHit(minoX + 1, minoY, minoType, minoAngle)) {
        ++minoX;
      }
      break;
    case 4:  //rotate
      if (!isHit(minoX, minoY, minoType, (minoAngle + 1) % MINO_ANGLE_MAX)) {
        minoAngle = (minoAngle + 1) % MINO_ANGLE_MAX;
      }
      break;
    default:
      break;
  }
  display(1);
}

bool isHit(int argMinoX, int argMinoY, int argMinoType, int argMinoAngle) {
  for (int i = 0; i < MINO_HEIGHT; ++i) {
    for (int j = 0; j < MINO_WIDTH; ++j) {
      if ((minoShapes[argMinoType][argMinoAngle][i][j] > 0) && (field[argMinoY + i][argMinoX + j] > 0)) {
        return true;
      }
    }
  }
  return false;
}

void resetMino() {
  minoX = 5;
  minoY = 0;
  minoType = micros() % MINO_TYPE_MAX;
  minoAngle = micros() % MINO_ANGLE_MAX;
}

void MinoUpdate() {
  bool isLineFilled;
  t = micros();
  if (isHit(minoX, minoY + 1, minoType, minoAngle)) {
    for (int i = 0; i < MINO_HEIGHT; ++i) {
      for (int j = 0; j < MINO_WIDTH; ++j) {
        field[minoY + i][minoX + j] += minoShapes[minoType][minoAngle][i][j];
      }
    }
    for (int line = 2; line < FIELD_HEIGHT - 1; ++line) {
      isLineFilled = true;
      for (int j = 1; j < FIELD_WIDTH - 1; ++j) {
        if (field[line][j] == 0) {
          isLineFilled = false;
        }
      }
      //erase lined mino
      if (isLineFilled == true) {
        for (int k = 1; k < FIELD_WIDTH - 1; ++k) {
          field[line][k] = 0;
        }
        display(0);
        delay(50);
        //slide mino
        for (int p = line; p > 0; p--) {
          for (int q = 1; q < FIELD_WIDTH - 1; ++q) {
            field[p][q] = field[p - 1][q];
          }
        }
        display(0);
        delay(200);
        score++;
      }
    }
    gameover = false;
    for (int k = 1; k < FIELD_WIDTH - 1; ++k) {
      if (field[2][k] != 0) {
        gameover = true;
      }
    }
    resetMino();
  } else {
    ++minoY;
  }
  display(1);
}
