#include <LiquidCrystal.h>

// LCD variables
const int rsPin = 12;
const int ePin = 11;
const int d4Pin = 5;
const int d5Pin = 4;
const int d6Pin = 3;
const int d7Pin = 2;

LiquidCrystal lcd(rsPin, ePin, d4Pin, d5Pin, d6Pin, d7Pin);

byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
};

byte left[8] = {
  B00011,
  B00111,
  B01110,
  B11100,
  B01110,
  B00111,
  B00011,
};

byte right[8] = {
  B11000,
  B11100,
  B01110,
  B00111,
  B01110,
  B11100,
  B11000,
};

#define LCD_WIDTH 16

//Jojystick variables

const int jsX = A0;
const int jsY = A1;
const int jsSW = 7;

int xValue = 0, yValue = 0;
int minThreshold = 400, maxThreshold = 600;
bool joyMovedX = false, joyMovedY = false;
bool swState = false, lastSwState = false;

int lives = 3;
int startingLevelValue = 1;
int level = 1;
int score = 0;
int hiscore = 0;

long playMillis = 0;
long levelMillis = 0;

enum MenuStates {
  START_GAME, HIGH_SCORE, SETTINGS, PLAYING, ADJUSTING
} menuState;

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, heart);
  lcd.createChar(1, left);
  lcd.createChar(2, right);

  pinMode(jsSW, INPUT_PULLUP);
  
  menuState = START_GAME;
}

void loop() {
  switch(menuState) {
    case START_GAME:
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      centerTextOnLcd("Start game", 0);
      lcd.setCursor(14, 0);
      lcd.write(byte(2));
      centerTextOnLcd("JS button=start", 1);
      break;
      
    case HIGH_SCORE:
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      centerTextOnLcd("HiScore", 0);
      lcd.setCursor(14, 0);
      lcd.write(byte(2));
      centerTextOnLcd(String("Your HiScore:") + String(hiscore), 1);
      break;
      
    case SETTINGS:
      lcd.setCursor(1, 0);
      lcd.write(byte(1));
      centerTextOnLcd("Settings", 0);
      lcd.setCursor(14, 0);
      lcd.write(byte(2));
      centerTextOnLcd("JS button=enter", 1);
      break;
      
    case ADJUSTING:
      centerTextOnLcd("Starting level", 0);
      lcd.setCursor(0, 1);
      lcd.print(String("JS -Y"));
      lcd.write(byte(1));
      lcd.print((startingLevelValue < 10 ? String("  "):String(" ")) + String(startingLevelValue) + String(" "));
      lcd.write(byte(2));
      lcd.print("JS +Y");
      break;

    case PLAYING:
      if(millis() - playMillis > 10000) {
        centerTextOnLcd("Level finished!", 0);
        lcd.setCursor(0, 1);
        lcd.print("Press button.");
        hiscore = score;
      } else {
        if(millis() - levelMillis > 2000) {
          level++;
          score = level * 3;
          levelMillis = millis();
        }
        lcd.setCursor(0, 0);
        lcd.print("Level: ");
        lcd.print(level);
        lcd.print("  ");
        lcd.print(lives);
        lcd.write(byte(0));
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(score);
      }
      break;
  }

  joystickEventCheck();
  
}

void centerTextOnLcd(String text, short line) {
  short l = text.length();
  short spaces = (LCD_WIDTH - l) / 2;
  lcd.setCursor(spaces, line);
  lcd.print(text);
}

void changeMenuState(bool fw) {
  if(fw) {
    if(menuState == 2) {
      menuState = 0;   
    } else {
      menuState = menuState + 1;  
    }
  } else {
    if(menuState == 0) {
      menuState = 2;   
    } else {
      menuState = menuState - 1;  
    }
  }
  lcd.clear();
}

void joystickEventCheck() {
  xValue = analogRead(jsX);
  yValue = analogRead(jsY);
  
  if (xValue < minThreshold && joyMovedX == false) {
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

  if (xValue > maxThreshold && joyMovedX == false) {
    switch(menuState) {
      case ADJUSTING:
        if(startingLevelValue < 90)
        {
          startingLevelValue++;
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
    switch(menuState)
    {
      case START_GAME:
      case HIGH_SCORE:
      case SETTINGS:
        changeMenuState(false);
        break;
    }
    joyMovedY = true;
  }

  if (yValue > maxThreshold && joyMovedY == false) {
    switch(menuState)
    {
      case START_GAME:
      case HIGH_SCORE:
      case SETTINGS:
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
      switch(menuState) {
        case SETTINGS:
          menuState = ADJUSTING;
          startingLevelValue = level;
          lcd.clear();
          break;
        case ADJUSTING:
          menuState = SETTINGS;          
          lcd.clear();
          break;
        case START_GAME:
          menuState = PLAYING;
          lcd.clear();
          playMillis = millis();
          levelMillis = millis();
          level = startingLevelValue;
          break;
        case PLAYING:
          if(millis() - playMillis > 10000) {
            menuState = START_GAME;
            lcd.clear();
          }
          break;
      }
    }
  }
  lastSwState = swState;
  
}
