#include <LiquidCrystal.h>

const int rsPin = 12;
const int ePin = 11;
const int d4Pin = 5;
const int d5Pin = 4;
const int d6Pin = 3;
const int d7Pin = 2;

LiquidCrystal lcd(rsPin, ePin, d4Pin, d5Pin, d6Pin, d7Pin);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Hello world");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}
