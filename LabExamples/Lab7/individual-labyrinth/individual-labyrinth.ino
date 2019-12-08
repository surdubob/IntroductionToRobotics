#include <LedControl.h>

const int brightness = 1;
const int dinPin = 12;
const int clkPin = 11;
const int loadPin = 10;
const int numberOfDrivers = 1;

LedControl lc = LedControl(dinPin, clkPin, loadPin, numberOfDrivers);

const int jsX = A0;
const int jsY = A1;

int xValue = 0, yValue = 0;
int minThreshold = 400, maxThreshold = 600;
bool joyMovedX = false, joyMovedY = false;

int xP = 1, yP = 0;
int playerBlinkDelay = 100;
long lastPlayerBlink = 0;
bool showPlayer = 1;

bool matrix[8][8] = {
  {1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0},
  {1, 1, 1, 0, 1, 1, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};


void setup() {
  lc.shutdown(0, false); //turn off power saving , enables display
  lc.setIntensity(0, brightness);
  lc.clearDisplay(0);

  Serial.begin(9600);

  movePlayer(0, 0);
}

void loop() {

  if(millis() - lastPlayerBlink >= playerBlinkDelay) {
    showPlayer = !showPlayer;
    lastPlayerBlink = millis();
  }

  if(showPlayer) {
    matrix[xP][yP] = 1;
  } else {
    matrix[xP][yP] = 0;
  }
  joystickEventCheck();
  updateMatrix();
}

void joystickEventCheck() {
  xValue = 1023 - analogRead(jsX);
  yValue = analogRead(jsY);
  
  if (xValue < minThreshold && joyMovedX == false) {
    movePlayer(-1, 0);
    joyMovedX = true;
  }

  if (xValue > maxThreshold && joyMovedX == false) {
    movePlayer(1, 0);
    joyMovedX = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMovedX = false;
  }

  if (yValue < minThreshold && joyMovedY == false) {
    movePlayer(0, -1);
    joyMovedY = true;
  }

  if (yValue > maxThreshold && joyMovedY == false) {
    movePlayer(0, 1);
    joyMovedY = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMovedY = false;
  }

}

void movePlayer(int amountX, int amountY) {
  matrix[xP][yP] = 0;
  if(matrix[xP + amountX][yP] == 0) {
    xP += amountX;  
  }
  if(matrix[xP][yP + amountY] == 0) {
    yP += amountY;  
  }
  
  if(xP >= 8) {
    xP = 7;
  }
  if(xP < 0) {
    xP = 0;
  }
  if(yP >= 8) {
    yP = 7;
  }
  if(yP < 0) {
    yP = 0;
  }
  if(showPlayer)
  {
    matrix[xP][yP] = 1;
  }
}


void updateMatrix() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}
