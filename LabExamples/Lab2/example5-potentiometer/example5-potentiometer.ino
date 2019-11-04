int ledPin = 9;
int potPin = A0;


void setup() {
  
  pinMode(ledPin, OUTPUT);
  pinMode(potPin, INPUT);

}
int val = 0;
void loop() {
  val = analogRead(potPin);
  val = map(val, 0, 1023, 0, 255);
  
  analogWrite(ledPin, val);

  delay(1);
}
