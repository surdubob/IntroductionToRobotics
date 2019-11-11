// declare all the joystick pins
const int pinSW = 10; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

// declare all the segments pins
const int pinA = 2;
const int pinB = 3;
const int pinC = 4;
const int pinD = 5;
const int pinE = 6;
const int pinF = 7;
const int pinG = 8;
const int pinDP = 9;

const int segSize = 8;
const int noOfDigits = 10;

// dp on or off
int dpState = LOW;
// states of the button press
int swState = LOW;
int lastSwState = LOW;
int switchValue;
int xValue = 0;
int yValue = 0;

bool joyMoved = false;
int digit = 0;
int minThreshold = 400;
int maxThreshold = 600;

// store the pins in an array for easier access
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte digitMatrix[noOfDigits][segSize - 1] = {
  // a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

void displayNumber(byte digit, byte decimalPoint) {
  for (int i = 0; i < segSize - 1; i++) {
    digitalWrite(segments[i], digitMatrix[digit][i]);
  }

  // write the decimalPoint to DP pin
  digitalWrite(segments[segSize - 1], decimalPoint);
}

void setup() {
  // initialize all the pins
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);

  displayNumber(digit, dpState); // initial value displayed. Choose any value
}

void loop() {
  xValue = analogRead(pinX);
  // On Ox axis, if the value is lower than a chosen min threshold, then
  // decrease by 1 the digit value.
  if (xValue < minThreshold && joyMoved == false) {
    if (digit > 0) {
      digit--;
    } else {
      digit = 9;
    }
    joyMoved = true;
  }

  // On Ox axis, if the value is bigger than a chosen max threshold, then
  // increase by 1 the digit value
  if (xValue > maxThreshold && joyMoved == false) {
    if (digit < 9) {
      digit++;
    } else {
      digit = 0;
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }

  // simple state change detector. I0[deally, use debounce.
  swState = digitalRead(pinSW);
  if (swState !=  lastSwState) {
    if (swState == LOW) {
      dpState = !dpState;
    }
  }
  lastSwState = swState;

  displayNumber(digit, dpState);
  delay(1);
}
