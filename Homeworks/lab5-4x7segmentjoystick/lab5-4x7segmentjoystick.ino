const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

// declare all the joystick pins
const int pinSW = A2; // digital pin connected to switch output
const int pinX = A1; // A1 - analog pin connected to X output
const int pinY = A0; // A0 - analog pin connected to Y output

const int segSize = 8;

const int noOfDisplays = 4;
const int noOfDigits = 10;

byte editingDigit = -1;
byte dpState = HIGH;

int swState = LOW;
int lastSwState = LOW;
int switchValue;
int xValue = 0;
int yValue = 0;

int currentNumber = 0;
unsigned long delayBlinking = 300;  // incrementing interval
unsigned long lastBlinking = 0;

int minThreshold = 400;
int maxThreshold = 600;
bool joyMoved = false;

enum State {
  NO_SELECTION, SELECTING_DIGIT, EDITING_DIGIT
}currentState;

int pow10(int n)
{
    static int pow10[10] = {
        1, 10, 100, 1000
    };

    return pow10[n]; 
}

// segments array, similar to before
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
// digits array, to switch between them easily
int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
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
  digitalWrite(pinDP, decimalPoint);
}

// activate the display no. received as param
void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);
  }
  digitalWrite(digits[num], LOW);
}


void setup() {
  for (int i = 0; i < segSize - 1; i++)
  {
    pinMode(segments[i], OUTPUT);
  }
  for (int i = 0; i < noOfDisplays; i++)
  {
    pinMode(digits[i], OUTPUT);
  }
  Serial.begin(9600);
  pinMode(pinDP, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);

  currentState = NO_SELECTION;
}

void loop() {
  int number;
  int digit;
  int lastDigit;
  // Serial.println(currentNumber);

  number = currentNumber;
  digit = 0;

  while (number != 0) {
    lastDigit = number % 10;  // get the last digit
    showDigit(digit);
    displayNumber(lastDigit, (editingDigit == digit && dpState));
    // increase this delay to see multiplexing in action. At about 100 it becomes obvious
    delay(5);
    digit++;            // move to next display
    number = number / 10;
  }
  if(currentNumber < 1000) {
    showDigit(3);
    displayNumber(0, editingDigit == 3 && dpState);
    delay(5);
  }
  if(currentNumber < 100){
    showDigit(2);
    displayNumber(0, editingDigit == 2 && dpState);
    delay(5);
  }
  if(currentNumber < 10){
    showDigit(1);
    displayNumber(0, editingDigit == 1 && dpState);
    delay(5);
  }
  if(currentNumber == 0) {
    showDigit(0);
    displayNumber(0, editingDigit == 0 && dpState);
    delay(5);
  }
 
  if(currentState == NO_SELECTION) {
    swState = digitalRead(pinSW);
    if (swState !=  lastSwState) {
      if (swState == LOW) {
        currentState = SELECTING_DIGIT;
        dpState = HIGH;
        // Serial.println("Now selecting digit");
        editingDigit = 0;
      }
    }
    lastSwState = swState;
  }

  if(currentState == SELECTING_DIGIT) {
    xValue = 1023 - analogRead(pinX);
    if (xValue < minThreshold && joyMoved == false) {
      if (editingDigit > 0) {
        editingDigit--;
      } else {
        editingDigit = 3;
      }
      Serial.println(editingDigit);
      joyMoved = true;
    }
    if (xValue > maxThreshold && joyMoved == false) {
      if (editingDigit < 3) {
        editingDigit++;
      } else {
        editingDigit = 0;
      }
      Serial.println(editingDigit);
      joyMoved = true;
    }

    if (xValue >= minThreshold && xValue <= maxThreshold) {
      joyMoved = false;
    }

    swState = digitalRead(pinSW);
    if (swState !=  lastSwState) {
      if (swState == LOW) {
        currentState = EDITING_DIGIT;
      }
    }
    lastSwState = swState;
  }

  if(currentState == EDITING_DIGIT) {
    yValue = analogRead(pinY);
    int p10 = pow10(editingDigit);
    if (yValue < minThreshold && joyMoved == false) {
      // joystick moved down
      if(currentNumber / p10 % 10 > 0) {
        currentNumber -= p10;
        //Serial.print("A scazut ");
        //Serial.println(p10);
      } else {
        currentNumber += 9 * p10;
        //Serial.print("A adunat ");
        //Serial.println(p10 * 9);
      }
      
      joyMoved = true;
    }
    if (yValue > maxThreshold && joyMoved == false) {
      // joystick moved up
      if(currentNumber / p10 % 10 < 9) {
        currentNumber += p10;
        //Serial.print("A adunat ");
        //Serial.println(p10);
      } else {
        currentNumber -= p10 * 9;
        //Serial.print("A scazut ");
        //Serial.println(p10 * 9);
      }
      joyMoved = true;
    }
    
    if (yValue >= minThreshold && yValue <= maxThreshold) {
      joyMoved = false;
    }

    swState = digitalRead(pinSW);
    if (swState !=  lastSwState) {
      if (swState == LOW) {
        currentState = SELECTING_DIGIT;
        dpState = HIGH;
      }
    }
    lastSwState = swState;
  }

  if(currentState == EDITING_DIGIT) {
    if(millis() - lastBlinking >= delayBlinking) {
      dpState = !dpState;
      lastBlinking = millis();
    }
  }
}
