#include <Adafruit_Keypad.h>
#include <FastLED.h>

const byte ROWS = 9; // rows
const byte COLS = 4; // columns

//define the symbols on the buttons of the keypads
byte keys[ROWS][COLS] = {
  {0, 9 , 18, 27},
  {1, 10, 19, 28},
  {2, 11, 20, 29},
  {3, 12, 21, 30},
  {4, 13, 22, 31},
  {5, 14, 23, 32},
  {6, 15, 24, 33},
  {7, 16, 25, 34},
  {8, 17, 26, 35},
};
byte rowPins[ROWS] = {38, 36, 34, 32, 30, 28, 26, 24, 22}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {37, 35, 33, 31}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);


#define MATRIX_LED_NUMBER 36
#define CONTROL_LED_NUMBER 3

const int DATA_PIN_MATRIX = 5;
const int DATA_PIN_CONTROL = 6;

CRGB leds[MATRIX_LED_NUMBER + CONTROL_LED_NUMBER];

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN_MATRIX>(leds, 0, MATRIX_LED_NUMBER);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_CONTROL>(leds, MATRIX_LED_NUMBER, CONTROL_LED_NUMBER);
  Serial.begin(9600);
  customKeypad.begin();

}

void loop() {
  customKeypad.tick();

  while(customKeypad.available()){
    keypadEvent e = customKeypad.read();
    Serial.print(e.bit.KEY);
    if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
    else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");

    leds[e.bit.KEY] = CRGB::Green;
    FastLED.show();
    delay(300);
    leds[e.bit.KEY] = CRGB::Black;
    FastLED.show();
  }

}
