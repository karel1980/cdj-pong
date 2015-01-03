/*
 * "Pong"
 * 
 * Adapted from the Conway's Game Of Life example
 * on the arduino.cc website
 *
 * Needs FrequencyTimer2 library
 */

#include <FrequencyTimer2.h>

byte col = 0;
byte leds[8][8];

int ball_x = 0;
int ball_y = 3;

int paddle1_x = 0;
int paddle2_x = 0;

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[17]= {-1, 1, 17, 0, 11, 4, 5, 6, 7,      16, 15, 14, 13, 12, 10, 9, 8};
 
// col[xx] of leds = pin yy on led matrix
int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[06], pins[11], pins[15], pins[16]};

// row[xx] of leds = pin yy on led matrix
int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};

#define DELAY 50
#define SIZE 8
extern byte leds[SIZE][SIZE];
byte world[SIZE][SIZE];
long density = 50;

void setup() {
  setupLeds();
  //pinMode(0, OUTPUT);
  randomSeed(analogRead(5));
  pinMode(19, INPUT);
  pinMode(18, INPUT);
  /*
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (random(100) < density) {
        world[i][j][0] = 1;
      }
      else {
        world[i][j][0] = 0;
      }
      world[i][j][1] = 0;
    }
  }
  */
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      world[i][j] = 0;
    }
  }
  clearLeds();
  //Serial.begin(115200);
}
int paddle1;
int paddle2;
unsigned long last_ball_update = 0;
boolean done = false;
//unsigned long last = 0;
void loop() {
  //Serial.println(last);
  //digitalWrite(0,(last%20)>10);
  //last++;
  paddle1 = analogRead(19);
  paddle2 = analogRead(18);
  //Serial.println(paddle1);
  // Display current generation
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      leds[i][j] = world[i][j];
    }
  }
/*
  // Birth and death cycle 
  for (int x = 0; x < SIZE; x++) { 
    for (int y = 0; y < SIZE; y++) {
      // Default is for cell to stay the same
      world[x][y][1] = world[x][y][0];
      int count = neighbours(x, y); 
      if (count == 3 && world[x][y][0] == 0) {
        // A new cell is born
        world[x][y][1] = 1; 
      } 
      if ((count < 2 || count > 3) && world[x][y][0] == 1) {
        // Cell dies
        world[x][y][1] = 0; 
      }
    }
  }

  // Copy next generation into place
  for (int x = 0; x < SIZE; x++) { 
    for (int y = 0; y < SIZE; y++) {
      world[x][y][0] = world[x][y][1];
    }
  }*/
  
  paddle1_x = paddle1*6/1024;
  paddle2_x = paddle2*6/1024;
  //world[3][ball_x][0]=1;
  //world[5][ball_x][0]=1;
  update_ball_position(); 
  delay(DELAY);
  if(done){
   for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      world[i][j] = 1;
    }
   }
    return; 
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      world[i][j] = 0;
    }
  }
  for(int i = 0; i < 3; ++i){
    world[paddle1_x+i][0]=1;
    world[paddle2_x+i][7]=1;
  }
  world[ball_x][ball_y] = 1;
}
int ball_dy = 1;
int ball_dx = 1;
unsigned long threshold = 300;
void update_ball_position(){
  if(millis() - last_ball_update <= threshold){
    return;
  }
  last_ball_update += threshold;
  ball_x += ball_dx;
  ball_y += ball_dy;
  if(ball_x < 0 || ball_x >= 8){
    ball_x -= 2*ball_dx;
    ball_dx = -ball_dx;
  }
  if(ball_y >= 8){
    if(ball_x <= paddle2_x + 2 && ball_x >= paddle2_x){
      ball_y  -= 2*ball_dy;
      ball_dy -= 2*ball_dy;
    }else{
      done = true;
    }
  }else if(ball_y <= 0){
    if(ball_x <= paddle1_x + 2 && ball_x >= paddle1_x){
      //we hit the paddle
      ball_y  -= 2*ball_dy;
      ball_dy -= 2*ball_dy;
    }else{
      done = true;
    }
  }
}

void setupLeds() {
  // sets the pins as output
  for (int i = 1; i <= 16; i++) {
    pinMode(pins[i], OUTPUT);
  }

  // set up cols and rows
  for (int i = 1; i <= 8; i++) {
    digitalWrite(cols[i - 1], LOW);
  }

  for (int i = 1; i <= 8; i++) {
    digitalWrite(rows[i - 1], LOW);
  }

  clearLeds();

  // Turn off toggling of pin 11 and 3
  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(2000);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(display);

}

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      leds[i][j] = 0;
    }
  }
}

// Interrupt routine
void display() {
  digitalWrite(cols[col], HIGH);  // Turn whole previous column off
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (leds[col][7 - row] != 1) {
      digitalWrite(rows[row], LOW);  // Turn on this led
    }
    else {
      digitalWrite(rows[row], HIGH); // Turn off this led
    }
  }
  digitalWrite(cols[col], LOW); // Turn whole column on at once (for equal lighting times)

  /*for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Serial.print(world[i][j][0]);
      Serial.print("");
    }
    Serial.println();
  }*/
}
