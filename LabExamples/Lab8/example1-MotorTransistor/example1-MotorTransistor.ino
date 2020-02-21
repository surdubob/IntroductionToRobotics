const int motorPin = 3;

void setup()
{
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Speed 0 to 255");
}

void loop()
{
  if (Serial.available())
  {
    int motorSpeed  = Serial.parseInt();
    if (motorSpeed >= 0 && motorSpeed <= 255)
    {
      analogWrite(motorPin, motorSpeed);
    }
  }
}
