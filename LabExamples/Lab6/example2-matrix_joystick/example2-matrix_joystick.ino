
const int row[] = {6, 11, A5, 9, 5, A4, 4, A2};

const int col[] = {10, 3, 2, 7, A3, 8, 12, 13};

const int joyX = A0;
const int joyY = A1;

int prevX = 3;
int prevY = 3;
int posX = 3;
int posY = 3;

int valX = 0;
int valY = 0;

bool movedX = false;
bool movedY = false;

int i = 0;

void setup() {
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  for(i = 0; i < 8; i++) {
    pinMode(row[i], OUTPUT);
    pinMode(col[i], OUTPUT);
  }
  
  for(i = 0; i < 8; i++) {
    digitalWrite(col[i], LOW);
    digitalWrite(row[i], HIGH);
  }

  digitalWrite(row[posX], LOW);
  digitalWrite(col[posY], HIGH);

}

void loop() {
  valX = analogRead(joyX);
  valY = analogRead(joyY);

  if(valX < 400) {
    if(movedX == false) {
      prevX = posX;
      posX++;
      movedX = true;
    }
  }
  else {
    if(valX > 600) {
      if(movedX == false) {
        prevX = posX;
        posX--;
        movedX = true;
      }
    }
    else {
      movedX = false;
    }
  }

  if(valY < 400) {
    if(movedY == false) {
      prevY = posY;
      posY--;
      movedY = true;
    }
  }
  else {
    if(valY > 600) {
      if(movedY == false) {
        prevY = posY;
        posY++;
        movedY = true;
      }
    }
    else {
      movedY = false;
    }
  }

  if(posX == 8){
    posX = 0;
  }
  if(posX == -1) {
    posX = 7;
  }

  if(posY == 8){
    posY = 0;
  }
  if(posY == -1) {
    posY = 7;
  }

  digitalWrite(row[prevX], HIGH);
  digitalWrite(col[prevY], LOW);

  digitalWrite(row[posX], LOW);
  digitalWrite(col[posY], HIGH);

  delay(10);
}
