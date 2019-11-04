const int ledPin = 13;
int ledState = LOW;

unsigned long previousMillis = 0;

const long interval = 1000;

void setup() {
  pinMode(ledPin, OUTPUT);  
}

void loop() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    if(ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
    
    previousMillis = currentMillis;
  }

}
