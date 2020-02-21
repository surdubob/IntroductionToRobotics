#include <Servo.h>
const int servoPin = 9;

Servo servo;

int angle = 0;

const int pot = A0;
int potVal = 0;

void setup() {
  servo.attach(servoPin);
  pinMode(pot, INPUT);
}

void loop() {
  potVal = analogRead(pot);
  potVal = map(potVal, 0, 1023, 0, 179);
  servo.write(potVal);
  delay(10); 
}
