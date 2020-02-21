const int en1 = 11;
const int in1 = 10;
const int in2 = 9;
const int pot = A0;
const int btn = 4;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int potVal = 0;
int spd = 0;
int lastButtonState = 0;
bool dir = true;
int buttonState = 0;

void sens(bool s) {
  digitalWrite(in1, s);
  digitalWrite(in2, !s);
}

void setup() {
  pinMode(en1, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(pot, INPUT);
  pinMode(btn, INPUT_PULLUP);
  
}

void loop() {
  potVal = analogRead(pot);
  spd = map(potVal, 0, 1023, 0, 255);
  analogWrite(en1, spd);

  int reading = digitalRead(btn);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        dir = !dir;        
      }
    }
  }
  lastButtonState = reading;
  sens(dir);
}
