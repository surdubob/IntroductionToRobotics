const int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);

}
int i = 0;
void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}
