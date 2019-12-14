#include <LiquidCrystal.h>
#include <LedControl.h>

// lcd variables
const int RS = 7;
const int EN = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(RS, EN, d4, d5, d6, d7);

// matrix variables
const int dinPin = 12;
const int clkPin = 11;
const int loadPin = 10;
const int brightness = 0;
const int numberOfDrivers = 1;

#define MATRIX_SIZE 8

LedControl lc = LedControl(dinPin, clkPin, loadPin, numberOfDrivers);

// joystick variables
const int jsX = A0, jsY = A1, jsSW = 8;
int xValue = 0, yValue = 0;
int minThreshold = 400, maxThreshold = 600;
bool joyMovedX = false, joyMovedY = false, swState = false, lastSwState = false;


byte mat[30][30];

int width = 9, height = 9;

int playerX = 0, playerY = 1;
bool showPlayer = true;
long lastPlayerBlink = 0, lastPlayerMoveX = 0, lastPlayerMoveY = 0;
int playerBlinkDelay = 150;

bool cameraLockedX = false, cameraLockedY = false;
int cameraX = 0, cameraY = 0;
const int longMoveDelay = 200;

void setup() {
  pinMode(jsSW, INPUT_PULLUP);

  lcd.begin(2, 16);
  lcd.print("Hello world");

  lc.shutdown(0, false); //turn off power saving , enables display
  lc.setIntensity(0, brightness);
  lc.clearDisplay(0);

  Serial.begin(9600);

  randomSeed(analogRead(A5));

   newMaze();
}

void loop() {
  readInputs();
  processEvents();

  blinkPlayer();

  renderMatrix();
}


void readInputs() {
  xValue = analogRead(jsY);
  yValue = analogRead(jsX);
  swState = !digitalRead(jsSW);
}

void processEvents() {

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

void eventJSX(bool plus) {
  if (plus) {
    if (!cameraLockedX) {
      if (playerX + 1 < height && mat[playerX + 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX++;
        if(playerX == 3) {
          cameraLockedX = 1;
        }
      }
    } else {
      if(mat[playerX + 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX++;
        if(cameraX == height - MATRIX_SIZE) {
          cameraLockedX = 0;
        } else {
          cameraX++;  
        }
      }
    }
  } else {
    if (!cameraLockedX) {
      if (playerX > 0 && mat[playerX - 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX--;
        if(playerX == height - MATRIX_SIZE + 3) {
          cameraLockedX = 1;
        }
      }
    } else {
      if (mat[playerX - 1][playerY] != 1) {
        mat[playerX][playerY] = 0;
        playerX--;
        if(cameraX == 0) {
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
    if (!cameraLockedY) {
      if (playerY + 1 < width && mat[playerX][playerY + 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY++;
        if(playerY == 3) {
          cameraLockedY = 1;
        }
      }
    } else {
      if(mat[playerX][playerY + 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY++;
        if(cameraY == width - MATRIX_SIZE) {
          cameraLockedY = 0;
        } else {
          cameraY++;  
        }
      }
    }
  } else {
    if (!cameraLockedY) {
      if (playerY > 0 && mat[playerX][playerY - 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY--;
        if(playerY == width - MATRIX_SIZE + 3) {
          cameraLockedY = 1;
        }
      }
    } else {
      if (mat[playerX][playerY - 1] != 1) {
        mat[playerX][playerY] = 0;
        playerY--;
        if(cameraY == 0) {
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

void renderMatrix() {
  mat[playerX][playerY] = showPlayer;

  for (int i = cameraX ; i < cameraX + MATRIX_SIZE; i++) {
    for (int j = cameraY; j < cameraY + MATRIX_SIZE; j++) {
      lc.setLed(0, i - cameraX, j - cameraY, mat[i][j]);
    }
  }
}

void printMatrix() {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      Serial.print(mat[i][j]);
    }
    Serial.println();
  }
  Serial.println();
  Serial.println();
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

void newMaze() {
  char *maze;

  /* Allocate the maze array. */
  maze = (char*)malloc(width * height * sizeof(char));
  if (maze == NULL) {
    Serial.println("No memory available");
  }

  /* Generate and display the maze. */
  GenerateMaze(maze);
  //ShowMaze(maze, width, height);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      mat[i][j] = maze[width * i + j];
    }
  }

  free(maze);
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
