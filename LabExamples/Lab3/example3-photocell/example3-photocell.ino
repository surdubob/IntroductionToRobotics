const int photoCellPin = A0;
const int ledPin = 13;

int photoCellValue = 0;
int ledState = 0;

int lowThreshold = 300;
int highThreshold = 450;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  photoCellValue = analogRead(photoCellPin);
  Serial.println(photoCellValue);

  if (photoCellValue < lowThreshold) {
  ledState = HIGH;
  }
  else if (photoCellValue > highThreshold) {
  ledState = LOW;
  }
  digitalWrite(ledPin, ledState);
  delay(10);
}
