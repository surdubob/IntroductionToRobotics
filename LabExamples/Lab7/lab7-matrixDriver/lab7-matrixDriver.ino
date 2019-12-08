#include <LedControl.h>

const int brightness = 1;
const int dinPin = 12;
const int clkPin = 11;
const int loadPin = 10;
const int numberOfDrivers = 1;

LedControl lc = LedControl(dinPin, clkPin, loadPin, numberOfDrivers);

bool matrix[8][8] = {
  {1, 0, 0, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 1}
};

const byte mat[8] = 
{
  B00000000,
  B10000000,
  B01000000,
  B00010000,
  B00000100,
  B00000000,
  B00000000,
  B00000000,
};

void setup() {
  lc.shutdown(0, false); //turn off power saving , enables display
  lc.setIntensity(0, brightness);
  lc.clearDisplay(0);

  /*for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }*/
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, mat[row]);
  }
}

void loop() {
  /*for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, true);
      delay(25);
    }
  }

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, false);
      delay(25);
    }
  }*/

}
