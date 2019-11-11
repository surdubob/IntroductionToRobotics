const int pushButton = 2; // the number of the pushbutton pin
const int ledPin = 13; // the number of the LED pin

const int readingBuzzer = A5;
const int toneBuzzer = 3;

int ledState = HIGH; // the current state of the output pin
int buttonState; // the current reading from the input pin
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers
unsigned long lastTime = 0;

int tresh = 400;

int soundLevel = 0;

bool ok = 0;

void setup() {
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(readingBuzzer, INPUT);
  pinMode(toneBuzzer, OUTPUT);

  digitalWrite(ledPin, ledState);
  Serial.begin(9600);
}

void checkButtonState() {
  int reading = digitalRead(pushButton);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough

  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce delay, so take it as the actual current stat3e:
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        noTone(toneBuzzer);
      }
    }
  }
  // set the LED:
  digitalWrite(ledPin, ledState);
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

void loop() {
  checkButtonState();

  soundLevel = analogRead(readingBuzzer);

  if (soundLevel > tresh) {
    lastTime = millis();
    ok = 1;
  }

  if (millis() - lastTime > 5000 && ok) {
    tone(toneBuzzer, 440);
    ok = 0;
  }
}
