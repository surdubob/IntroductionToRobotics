
const int pinX = A0;
const int pinY = A1;
const int pinSW = 10;

int switchValue = 0, xValue = 0, yValue = 0;

void setup() {
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  switchValue = digitalRead(pinSW);
  
  Serial.print(xValue);
  Serial.print("  ");
  Serial.print(yValue);
  Serial.print("  ");
  Serial.println(switchValue);
  delay(100);
}
