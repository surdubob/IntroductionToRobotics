#include <LiquidCrystal.h>

const int rsPin = 12;
const int ePin = 11;
const int d4Pin = 5;
const int d5Pin = 4;
const int d6Pin = 3;
const int d7Pin = 2;

LiquidCrystal lcd(rsPin, ePin, d4Pin, d5Pin, d6Pin, d7Pin);

const int trigPin = 9;
const int echoPin = 10;

int dist = 0;

char msg[16];

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  lcd.begin(16, 2);
  ///lcd.print("Distance: ");
}

void loop() {
  dist = readDistance();
  lcd.setCursor(0, 0);
  sprintf(msg, "Distance: %-4d", dist); 
  lcd.print(msg);
  delay(200);
}

int readDistance() {
  long duration = 0;
  int distance = 0;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}
