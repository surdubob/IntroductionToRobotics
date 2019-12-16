#include <LiquidCrystal.h>
#include <LedControl.h>
#include <EEPROM.h>

// lcd variables
const int RS = 7;
const int EN = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(RS, EN, d4, d5, d6, d7);

const byte diamondChar[8] = {
  B00000,
  B00000,
  B01110,
  B11111,
  B01110,
  B00100,
  B00000,
};

const byte left[8] = {
  B00011,
  B00111,
  B01110,
  B11100,
  B01110,
  B00111,
  B00011,
};

const byte right[8] = {
  B11000,
  B11100,
  B01110,
  B00111,
  B01110,
  B11100,
  B11000,
};

const byte startMat[8][8] = {
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};
/*{
  {0, 1, 1, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 1, 1, 0, 1},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 1, 1, 0, 0},
  {0, 0, 0, 0, 1, 1, 0, 0},
};*/


#define LCD_WIDTH 16


// matrix variables
const byte dinPin = 12;
const byte clkPin = 11;
const byte loadPin = 10;
const byte brightness = 1;

#define MATRIX_SIZE 8

LedControl lc = LedControl(dinPin, clkPin, loadPin, 1);

// joystick variables
const int jsX = A0, jsY = A1, jsSW = 8;
int xValue = 0, yValue = 0;
int minThreshold = 400, maxThreshold = 600;
bool joyMovedX = false, joyMovedY = false, swState = false, lastSwState = false;


byte mat[21][21];

byte width = 7, height = 7;

byte playerX = 0, playerY = 1;
bool showPlayer = false;
long lastPlayerBlink = 0, lastPlayerMoveX = 0, lastPlayerMoveY = 0;
byte playerBlinkDelay = 150;

bool cameraLockedX = false, cameraLockedY = false;
byte cameraX = 0, cameraY = 0;
const byte longMoveDelay = 200;
long lastStartAnimation = 0;

byte collectedDiamonds = 0;
byte startingLevelValue = 1;
byte level = 1;
int score = 0;
int hiscore = 0;

#define NUMBER_OF_DIAMONDS 3

struct Diamond {
  byte x, y;
  bool taken;
  public:
  Diamond() {
    taken = false;
  }
  
  Diamond(int ecs, int uai): x(ecs), y(uai) {
    taken = false;
  }
};
byte diamondNumber = 0;
Diamond diamonds[3];
long lastDiamondBlink = 0;
int diamondBlinkDelay = 200;
bool showDiamonds = false;

long infoMenuTimer = 0, timeDif = 0;

enum MenuStates {
  START_GAME, HIGH_SCORE, SETTINGS, INFO, PLAYING, ADJUSTING, LEVEL_COMPLETED
} menuState;

void setup() {
  pinMode(jsSW, INPUT_PULLUP);

  Serial.begin(9600);

  lcd.begin(2, 16);
  lcd.createChar(0, diamondChar);
  lcd.createChar(1, left);
  lcd.createChar(2, right);

  centerTextOnLcd(F("Welcome to the"), 0);
  centerTextOnLcd(F("aMAZEing Game "), 1);
  delay(2000);
  centerTextOnLcd(F("Made by Surdu "), 0);
  centerTextOnLcd(F("Bob Alexandru "), 1);  
  delay(2500);
  lcd.clear();

  lc.shutdown(0, false); //turn off power saving , enables display
  lc.setIntensity(0, brightness);
  lc.clearDisplay(0);

  randomSeed(analogRead(A5));

  menuState = START_GAME;

  EEPROM.get(0, hiscore);
  
  if(hiscore == 255) {
    hiscore = 0;
  }
}

void loop() {
  readInputs();
  
  switch (menuState) {
    case START_GAME:
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      centerTextOnLcd(F("Start game"), 0);
      lcd.setCursor(14, 0);
      lcd.write(byte(2));
      centerTextOnLcd(F("JS button=start"), 1);
      for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
          mat[i][j] = startMat[i][j];
        }
      }
      showPlayer = showDiamonds = false;
      processMenuEvents();
      break;

    case HIGH_SCORE:
      
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      centerTextOnLcd(String(F("HiScore: ")) + String(hiscore), 0);
      lcd.setCursor(14, 0);
      lcd.write(byte(2));
      //centerTextOnLcd(String(F("Your HiScore:")) + String(hiscore), 1);
      centerTextOnLcd("Press JS = clear", 1);
      processMenuEvents();
      break;

    case SETTINGS:
      
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      centerTextOnLcd(F("Settings"), 0);
      lcd.setCursor(14, 0);
      lcd.write(byte(2));
      centerTextOnLcd(F("JS button=enter"), 1);
      processMenuEvents();
      break;

    case ADJUSTING:
      centerTextOnLcd(F("Starting level"), 0);
      lcd.setCursor(0, 1);
      lcd.print(F("JS -Y"));
      lcd.write(byte(1));
      lcd.print((startingLevelValue < 10 ? String("  ") : String(" ")) + String(startingLevelValue) + String(" "));
      lcd.write(byte(2));
      lcd.print(F("JS +Y"));
      processMenuEvents();
      break;

    case PLAYING:
      if(playerX == height - 1 && playerY == width - 2) {
        menuState = LEVEL_COMPLETED;
      }
      
      lcd.setCursor(0, 0);
      lcd.print(F("Level: "));
      lcd.print(level);
      lcd.print("  ");
      lcd.print(collectedDiamonds);
      lcd.write(byte(0));
      lcd.setCursor(0, 1);
      lcd.print(F("Score: "));
      lcd.print(score);

      processGameEvents();
      blinkPlayer();
      blinkDiamonds();
      break;

    case LEVEL_COMPLETED:
      lcd.setCursor(0, 0);
      lcd.print(F("Level completed"));
      lcd.setCursor(0, 1);
      lcd.print(F("Press JS to exit"));
      processMenuEvents();
      break;

    case INFO:
      lcd.setCursor(0, 0);
      timeDif = millis() - infoMenuTimer;
      if(timeDif < 2000) {
        centerTextOnLcd(F(" Creator: Surdu "), 0);
        centerTextOnLcd(F(" Bob Alexandru  "), 1);
      } else if(timeDif >= 2000 && timeDif < 4000) {
        centerTextOnLcd(F("github: surdubob"), 0);
        centerTextOnLcd(F("@UnibucRobotics "), 1);
      } else if(timeDif >= 4000 && timeDif < 6000) {
        centerTextOnLcd(F("   Game name:   "), 0);
        centerTextOnLcd(F(" aMAZEing game  "), 1);
      } else {
        infoMenuTimer = millis();
      }
      processMenuEvents();
      break;
  }

  renderMatrix();
}


void readInputs() {
  xValue = analogRead(jsY);
  yValue = analogRead(jsX);
  swState = !digitalRead(jsSW);
}

void changeMenuState(bool fw) {
  if (fw) {
    if (menuState == 3) {
      menuState = 0;
    } else {
      menuState = menuState + 1;
    }
  } else {
    if (menuState == 0) {
      menuState = 3;
    } else {
      menuState = menuState - 1;
    }
  }
  lcd.clear();
}

void processMenuEvents() {
  if (xValue < minThreshold && joyMovedX == false) {
    switch(menuState) {
      case ADJUSTING:
        if(startingLevelValue < 20)
        {
          startingLevelValue++;
          lcd.clear();
        }
        break;
    }
    joyMovedX = true;
  }

  if (xValue > maxThreshold && joyMovedX == false) {
    switch(menuState) {
      case ADJUSTING:
        if(startingLevelValue > 1)
        {
          startingLevelValue--;
          lcd.clear();
        }
        break;
    }
    joyMovedX = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMovedX = false;
  }
  
  if (yValue < minThreshold && joyMovedY == false) {
    switch (menuState)
    {
      case START_GAME:
      case HIGH_SCORE:
      case SETTINGS:
      case INFO:
        changeMenuState(false);
        break;
    }
    joyMovedY = true;
  }

  if (yValue > maxThreshold && joyMovedY == false) {
    switch (menuState)
    {
      case START_GAME:
      case HIGH_SCORE:
      case SETTINGS:
      case INFO:
        changeMenuState(true);
        break;
    }
    joyMovedY = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMovedY = false;
  }

  swState = digitalRead(jsSW);
  if (swState !=  lastSwState) {
    if (swState == LOW) {
      switch (menuState) {
        case HIGH_SCORE:
          hiscore = 0;
          EEPROM.put(0, 0);
          lcd.clear();
          break;
        case SETTINGS:
          menuState = ADJUSTING;
          startingLevelValue = level;
          lcd.clear();
          break;
          
        case ADJUSTING:
          menuState = SETTINGS;
          level = startingLevelValue;
          lcd.clear();
          break;
          
        case START_GAME:
          menuState = PLAYING;
          lcd.clear();
          width = height = 7 + 2 * level;
          if(width > 20){
            width = 21;
          }
          if(height > 20) {
            height = 21;
          }
          newMaze();
          break;
          
        case LEVEL_COMPLETED:
          lcd.clear();
          menuState = START_GAME;
          level++;
          score += 10;
          playerX = 0;
          playerY = 1;
          cameraX = cameraY = 0;
          collectedDiamonds = 0;
          if(hiscore < score) {
            hiscore = score;
          }
          int eepromHiscore = 0;
          EEPROM.get(0, eepromHiscore);
          if(eepromHiscore == 255 || eepromHiscore < hiscore) {
            EEPROM.put(0, hiscore);
          }
          lc.clearDisplay(0);
          break;
      }
    }
  }
  lastSwState = swState;
}

void processGameEvents() {

  if (xValue < minThreshold && joyMovedX == false) {
    eventJSX(false);
    lastPlayerMoveX = millis();

    joyMovedX = true;
  }

  if (millis() - lastPlayerMoveX >= longMoveDelay && xValue < minThreshold)
  {
    eventJSX(false);
    lastPlayerMoveX = millis();
  }


  if (xValue > maxThreshold && joyMovedX == false) {
    eventJSX(true);
    lastPlayerMoveX = millis();

    joyMovedX = true;
  }

  if (millis() - lastPlayerMoveX >= longMoveDelay && xValue > maxThreshold)
  {
    eventJSX(true);
    lastPlayerMoveX = millis();
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMovedX = false;
  }


  if (yValue < minThreshold && joyMovedY == false) {
    eventJSY(false);
    lastPlayerMoveY = millis();
    joyMovedY = true;
  }

  if (millis() - lastPlayerMoveY > longMoveDelay && yValue < minThreshold) {
    eventJSY(false);
    lastPlayerMoveY = millis();
  }

  if (yValue > maxThreshold && joyMovedY == false) {
    eventJSY(true);
    lastPlayerMoveY = millis();
    joyMovedY = true;
  }

  if (millis() - lastPlayerMoveY > longMoveDelay && yValue > maxThreshold) {
    eventJSY(true);
    lastPlayerMoveY = millis();
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMovedY = false;
  }

  if (swState !=  lastSwState) {
    if (swState == LOW) {
      eventJSSW();
    }
  }
  lastSwState = swState;
}

int isDiamond(int px, int py) {
  for(int i = 0; i < diamondNumber; i++) {
    if(diamonds[i].x == px && diamonds[i].y == py) {
      if(!diamonds[i].taken) {
        return i;
      }
    }
  }
  return -1;
}

void eventJSX(bool plus) {
  if (plus) {
    int diamondIndex = isDiamond(playerX + 1, playerY);
    if(diamondIndex != -1) {
      diamonds[diamondIndex].taken = true;
      score += 5;
      collectedDiamonds++;
      mat[diamonds[diamondIndex].x][diamonds[diamondIndex].y] = 0;
    }
    if (!cameraLockedX) {
      if (playerX + 1 < height && mat[playerX + 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX++;
        if (playerX == 3) {
          cameraLockedX = 1;
        }
      }
    } else {
      if (mat[playerX + 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX++;
        if (cameraX == height - MATRIX_SIZE) {
          cameraLockedX = 0;
        } else {
          cameraX++;
        }
      }
    }
  } else {
    int diamondIndex = isDiamond(playerX - 1, playerY);
    if(diamondIndex != -1) {
      diamonds[diamondIndex].taken = true;
      score += 5;
      collectedDiamonds++;
      mat[diamonds[diamondIndex].x][diamonds[diamondIndex].y] = 0;
    }
    if (!cameraLockedX) {
      if (playerX > 0 && mat[playerX - 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX--;
        if (playerX == height - MATRIX_SIZE + 3) {
          cameraLockedX = 1;
        }
      }
    } else {
      if (mat[playerX - 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX--;
        if (cameraX == 0) {
          cameraLockedX = 0;
        } else {
          cameraX--;
        }
      }
    }
  }
}

void eventJSY(bool plus) {
  if (plus) {
    int diamondIndex = isDiamond(playerX, playerY + 1);
    if(diamondIndex != -1) {
      diamonds[diamondIndex].taken = true;
      score += 5;
      collectedDiamonds++;
      mat[diamonds[diamondIndex].x][diamonds[diamondIndex].y] = 0;
    }
    if (!cameraLockedY) {
      if (playerY + 1 < width && mat[playerX][playerY + 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY++;
        if (playerY == 3) {
          cameraLockedY = 1;
        }
      }
    } else {
      if (mat[playerX][playerY + 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY++;
        if (cameraY == width - MATRIX_SIZE) {
          cameraLockedY = 0;
        } else {
          cameraY++;
        }
      }
    }
  } else {
    int diamondIndex = isDiamond(playerX, playerY - 1);
    if(diamondIndex != -1) {
      diamonds[diamondIndex].taken = true;
      score += 5;
      collectedDiamonds++;
      mat[diamonds[diamondIndex].x][diamonds[diamondIndex].y] = 0;
    }
    if (!cameraLockedY) {
      if (playerY > 0 && mat[playerX][playerY - 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY--;
        if (playerY == width - MATRIX_SIZE + 3) {
          cameraLockedY = 1;
        }
      }
    } else {
      if (mat[playerX][playerY - 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY--;
        if (cameraY == 0) {
          cameraLockedY = 0;
        } else {
          cameraY--;
        }
      }
    }
  }
}

void eventJSSW() {
  
}

void blinkPlayer() {
  if (millis() - lastPlayerBlink >= playerBlinkDelay) {
    showPlayer = !showPlayer;
    lastPlayerBlink = millis();
  }
}

void blinkDiamonds() {
  if (millis() - lastDiamondBlink >= diamondBlinkDelay) {
    showDiamonds = !showDiamonds;
    lastDiamondBlink = millis();
  }
}

void renderMatrix() {
  if(menuState == PLAYING) {
    mat[playerX][playerY] = showPlayer;
  
    for(int i = 0; i < NUMBER_OF_DIAMONDS; i++) {
      if(!diamonds[i].taken) {
        mat[diamonds[i].x][diamonds[i].y] = showDiamonds;
      }
    }
  }
  for (int i = cameraX ; i < cameraX + MATRIX_SIZE; i++) {
    for (int j = cameraY; j < cameraY + MATRIX_SIZE; j++) {
      lc.setLed(0, i - cameraX, j - cameraY, mat[i][j]);
    }
  }
}

/*  Utility function for centering text on the lcd display. */
void centerTextOnLcd(String text, short line) {
  short l = text.length();
  short spaces = (LCD_WIDTH - l) / 2;
  lcd.setCursor(spaces, line);
  lcd.print(text);
}

void spawnDiamonds() {
  for(diamondNumber = 0; diamondNumber < NUMBER_OF_DIAMONDS; diamondNumber++) {
    int dx = (random() % (height - 1)) + 1;
    int dy = (random() % (width - 1)) + 1;
    while(mat[dx][dy] == 1) {
      dx = (random() % (height - 1)) + 1;
      dy = (random() % (width - 1)) + 1;
    }
    diamonds[diamondNumber] = Diamond(dx, dy);
  }
}

/* Generate maze and copy it to the led matrix */
void newMaze() {
  char *maze;

  /* Allocate the maze array. */
  maze = (char*)malloc(width * height * sizeof(char));
  if (maze == NULL) {
    Serial.println("No memory available");
  }

  /* Generate the maze. */
  GenerateMaze(maze);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      mat[i][j] = maze[width * i + j];
    }
  }

  free(maze);

  spawnDiamonds();
}

/*  Carve the maze starting at x, y. */
void CarveMaze(char *maze, int width, int height, int x, int y) {

  int x1, y1;
  int x2, y2;
  int dx, dy;
  int dir, count;

  dir = random() % 4;
  count = 0;
  while (count < 4) {
    dx = 0; dy = 0;
    switch (dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
    }
    x1 = x + dx;
    y1 = y + dy;
    x2 = x1 + dx;
    y2 = y1 + dy;
    if (x2 > 0 && x2 < width && y2 > 0 && y2 < height
        && maze[y1 * width + x1] == 1 && maze[y2 * width + x2] == 1) {
      maze[y1 * width + x1] = 0;
      maze[y2 * width + x2] = 0;
      x = x2; y = y2;
      dir = random() % 4;
      count = 0;
    } else {
      dir = (dir + 1) % 4;
      count += 1;
    }
  }

}

/* Generate maze in matrix maze with size width, height. */
void GenerateMaze(char *maze) {
  int x, y;

  /* Initialize the maze. */
  for (x = 0; x < width * height; x++) {
    maze[x] = 1;
  }
  maze[1 * width + 1] = 0;

  /* Carve the maze. */
  for (y = 1; y < height; y += 2) {
    for (x = 1; x < width; x += 2) {
      CarveMaze(maze, width, height, x, y);
    }
  }

  /* Set up the entry and exit. */
  maze[0 * width + 1] = 0;
  maze[(height - 1) * width + (width - 2)] = 0;
}
