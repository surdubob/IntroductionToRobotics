#include <LiquidCrystal.h>
#include <LedControl.h>

// lcd variables
const int RS = 7;
const int EN = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(RS, EN, d4, d5, d6, d7);

// matrix variables
const int dinPin = 12;
const int clkPin = 11;
const int loadPin = 10;
const int brightness = 1;
const int numberOfDrivers = 1;

LedControl lc = LedControl(dinPin, clkPin, loadPin, numberOfDrivers);

// joystick variables
const int jsX = A0, jsY = A1, jsSW = 8;
int xValue = 0, yValue = 0;
int minThreshold = 400, maxThreshold = 600;
bool joyMovedX = false, joyMovedY = false, swState = false, lastSwState = false;


byte mat[30][30];

int width = 9, height = 9;


void setup() {
  pinMode(jsSW, INPUT_PULLUP);

  lcd.begin(2, 16);
  lcd.print("Hello world");

  lc.shutdown(0, false); //turn off power saving , enables display
  lc.setIntensity(0, brightness);
  lc.clearDisplay(0);

  Serial.begin(9600);
}

void loop() {
  readInputs();
  processEvents();


  renderMatrix();
}


void readInputs() {
  xValue = analogRead(jsX);
  yValue = analogRead(jsY);
  swState = !digitalRead(jsSW);
}

void processEvents() {
  

  
  if (swState !=  lastSwState) {
    if (swState == LOW) {
      
    }
  }
  lastSwState = swState;
}

void renderMatrix() {
  for (int i = 0 ; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, mat[i][j]);
    }
  }
}

void printMatrix() {
  for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      Serial.print(mat[i][j]);
    }
    Serial.println();
  }
  Serial.println();
  Serial.println();
}


/*  Carve the maze starting at x, y. */
void CarveMaze(char *maze, int width, int height, int x, int y) {

   int x1, y1;
   int x2, y2;
   int dx, dy;
   int dir, count;

   dir = rand() % 4;
   count = 0;
   while(count < 4) {
      dx = 0; dy = 0;
      switch(dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
      }
      x1 = x + dx;
      y1 = y + dy;
      x2 = x1 + dx;
      y2 = y1 + dy;
      if(x2 > 0 && x2 < width && y2 > 0 && y2 < height
         && maze[y1 * width + x1] == 1 && maze[y2 * width + x2] == 1) {
         maze[y1 * width + x1] = 0;
         maze[y2 * width + x2] = 0;
         x = x2; y = y2;
         dir = rand() % 4;
         count = 0;
      } else {
         dir = (dir + 1) % 4;
         count += 1;
      }
   }

}

void newMaze() {
  char *maze;
   
   /* Allocate the maze array. */
   maze = (char*)malloc(width * height * sizeof(char));
   if(maze == NULL) {
      Serial.println("No memory available");
   }

   /* Generate and display the maze. */
   GenerateMaze(maze);
   //ShowMaze(maze, width, height);

   for(int i = 0; i < width; i++) {
    for(int j = 0; j < height; j++) {
      mat[i][j] = maze[width * i + j];
    }
   }
   
   free(maze);
}

/* Generate maze in matrix maze with size width, height. */
void GenerateMaze(char *maze) {

   int x, y;

   /* Initialize the maze. */
   for(x = 0; x < width * height; x++) {
      maze[x] = 1;
   }
   maze[1 * width + 1] = 0;


   /* Carve the maze. */
   for(y = 1; y < height; y += 2) {
      for(x = 1; x < width; x += 2) {
         CarveMaze(maze, width, height, x, y);
      }
   }

   /* Set up the entry and exit. */
   maze[0 * width + 1] = 0;
   maze[(height - 1) * width + (width - 2)] = 0;

}
