unsigned long time;

void setup() {
  Serial.begin(9600);

}

void loop() {
  time = millis();
  Serial.print("Time: ");
  Serial.println(time);
}
