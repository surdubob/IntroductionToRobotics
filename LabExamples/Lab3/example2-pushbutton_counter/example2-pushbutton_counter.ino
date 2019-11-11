const int pushButton = 2, ledPin = 13;
int buttonState = 0;

int ledState = 1;
int lastButtonState = 0;
int counter = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup () {
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  Serial.begin(9600);
}

void loop () {
  int reading = digitalRead(pushButton);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        ledState = !ledState;
        counter++;
      }
    }
  }

  digitalWrite(ledPin, ledState);
  lastButtonState = reading;
  Serial.println(counter);
}
