#define encoder0PinA 18
#define encoder0PinB 19
#define encoder0Btn 46
int encoder0Pos = 0;
void setup() {
  Serial.begin(9600);
  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  pinMode(encoder0Btn, INPUT_PULLUP);
  attachInterrupt(0, doEncoder, CHANGE);
}
int valRotary, lastValRotary;
void loop() {
  int btn = digitalRead(encoder0Btn);
  Serial.print(btn);
  Serial.print(" ");
  Serial.print(valRotary);
  if (valRotary > lastValRotary)
  {
    Serial.print("  CW");
  }
  if (valRotary < lastValRotary)  {

    Serial.print("  CCW");
  }
  lastValRotary = valRotary;
  Serial.println(" ");
  delay(25);
}
void doEncoder()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB))
  {
    encoder0Pos--;
  }
  else
  {
    encoder0Pos++;
  }
  valRotary = encoder0Pos / 2.5;
}
