
const int row[] = {6, 11, A5, 9, 5, A4, 4, A2};

const int col[] = {10, 3, 2, 7, A3, 8, 12, 13};

int i = 0;

void setup() {
  for(i = 0; i < 8; i++) {
    pinMode(row[i], OUTPUT);
    pinMode(col[i], OUTPUT);
  }
  for(i = 0; i < 8; i++) {
    digitalWrite(row[i], LOW);
    digitalWrite(col[i], HIGH);
  }
  delay(1000);
  for(i = 0; i < 8; i++) {
    digitalWrite(col[i], LOW);
    digitalWrite(row[i], HIGH);
  }
  delay(1000); 
  digitalWrite(col[3], HIGH);
  digitalWrite(row[1], LOW);
}

void loop() {


}
