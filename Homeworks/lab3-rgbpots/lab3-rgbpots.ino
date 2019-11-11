const int potR = A0, potG = A1, potB = A2;
const int ledR = 6, ledG = 5, ledB = 3;

int r = 0, g = 0, b = 0;

void setup() {
  pinMode (potR, INPUT);
  pinMode (potG, INPUT);
  pinMode (potB, INPUT);

  pinMode (ledR, OUTPUT);
  pinMode (ledG, OUTPUT);
  pinMode (ledB, OUTPUT);

  //Serial.begin (9600);
}

void loop() {
  r = map (analogRead (potR), 0, 1023, 0, 255);
  g = map (analogRead (potG), 0, 1023, 0, 255);
  b = map (analogRead (potB), 0, 1023, 0, 255);

  /*
  Serial.print ("R: ");
  Serial.print (r);
  Serial.print ("  G: ");
  Serial.print (g);
  Serial.print ("  B: ");
  Serial.println (b);
  */

  analogWrite (ledR, r);
  analogWrite (ledG, g);
  analogWrite (ledB, b);
  // delay (10);
}
