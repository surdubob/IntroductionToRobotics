
const int btnPin = 2;
int state = 0;

void setup() {
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  state = digitalRead(btnPin);

  Serial.println(state);
  digitalWrite(LED_BUILTIN, state);

  delay(50);

}
