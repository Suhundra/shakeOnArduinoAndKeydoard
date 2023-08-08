#include "SPI.h"
#define UP 48
#define DOWN 49
#define RIGHT 50
#define LEFT 51
#define X 0
#define Y 1
const int latchPin = 10;
const int clockPin = 13;
const int dataPin = 11;
uint8_t picture[8][8];
bool gameOver = false;
const int playGame = 2;
const int stopGame = 3;
const int eatApple = 4;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
  startGame();
  generateApple();
  pinMode(playGame, OUTPUT);
  pinMode(stopGame, OUTPUT);
  pinMode(eatApple, OUTPUT);
  randomSeed(analogRead(A0));
}

unsigned long int timeDelayMoving = 0;
unsigned long int timeDelayRPS = 0;
unsigned long int timeWhereAppleEaten = 0;
bool appleWasEaten = false;

int direction;
void loop() {

  if(gameOver){
    digitalWrite(stopGame, 1);
    digitalWrite(playGame, 0);
  }else{
    digitalWrite(playGame, 1);
  }


  if(appleWasEaten){
    if(millis() - timeWhereAppleEaten > 100){
      appleWasEaten = false;
      digitalWrite(eatApple, 0);
    }
  }

  if(millis() - timeDelayMoving > 500 && !gameOver){
    timeDelayMoving = millis();
    movingSnake();
  }
  movingSnakeFromKeyboard();
  render();
}

void render(){
  byte b = 0;
  for(uint8_t i = 0; i < 8; i++){
    for(uint8_t j  = 0; j < 8; j++){
      bitWrite(b, j, picture[i][j]);
    }
    digitalWrite(SS, LOW);
    SPI.transfer(~(1 << i));
    SPI.transfer(b);
    digitalWrite(SS, HIGH);
  }
}


uint8_t directionMoving = RIGHT;
uint8_t snake[64][2];
uint8_t lengthSnake = 2;

void startGame(){
  for(int i = lengthSnake; i >= 0; i--){
    snake[i][X] = 4; 
    snake[i][Y] = i; 
    setPixel(4, i);
  }  
}



void movingSnakeFromKeyboard(){
  while (Serial.available() > 0) {
    int readChar = Serial.read();
    Serial.println(readChar);
    switch(readChar){
      case UP: 
        if(directionMoving != DOWN){
          directionMoving = UP;
        }
        break;
      case DOWN:
        if(directionMoving != UP){
          directionMoving = DOWN;
        }
        break;
      case RIGHT:
        if(directionMoving != LEFT){
          directionMoving = RIGHT;
        }
        break;
      case LEFT:
        if(directionMoving != RIGHT){
          directionMoving = LEFT;
        }
        break;
    }
  }
}

uint8_t apple[2];
bool doneGenerateApple = true;

void generateApple() {
  while (true) {
    apple[X] = random(0, 8);
    apple[Y] = random(0, 8);
    
    bool intersectsWithSnake = false;
  
    for (int i = 0; i < lengthSnake; i++) {
      if (snake[i][X] == apple[X] && snake[i][Y] == apple[Y]) {
        intersectsWithSnake = true;
        break;
      }
    }
    
    if (!intersectsWithSnake) {
      break;
    }
  }

  setPixel(apple[X], apple[Y]);
}

void movingSnake(){
  if(snake[lengthSnake][X] == apple[X] && snake[lengthSnake][Y] == apple[Y]){
    appleWasEaten = true;
    timeWhereAppleEaten = millis();
    digitalWrite(eatApple, 1);
    
    lengthSnake++;

    snake[lengthSnake][X] = apple[X];
    snake[lengthSnake][Y] = apple[Y];

    clearPixel(apple[X], apple[Y]);

    generateApple();

    setPixel(snake[lengthSnake][X], snake[lengthSnake][Y]);
  }else{
    clearPixel(snake[0][X], snake[0][Y]);
    shiftElements();
    }
  
  switch(directionMoving){
      case UP: 
        snake[lengthSnake][X] = snake[lengthSnake-1][X] + 1;
        break;
      case DOWN:
        snake[lengthSnake][X] = snake[lengthSnake-1][X] - 1;
        break;
      case RIGHT:
        snake[lengthSnake][Y] = snake[lengthSnake][Y] + 1;
        break;
      case LEFT:
        snake[lengthSnake][Y] = snake[lengthSnake][Y] - 1;
        break;
    }
    checkCollision();
     
  setPixel(snake[lengthSnake][X], snake[lengthSnake][Y]);
}

void shiftElements(){
  for(int i = 0; i < lengthSnake; i++){
    snake[i][X] = snake[i+1][X];
    snake[i][Y] = snake[i+1][Y];
  }
}

void checkCollision(){
  if(snake[lengthSnake][X] > 7 || snake[lengthSnake][Y] > 7 || 
      snake[lengthSnake][X] < 0 || snake[lengthSnake][Y] < 0){
        gameOver = true;
  }

   for (int i = 0; i < lengthSnake - 1; i++) {
    if (snake[i][X] == snake[lengthSnake - 1][X] && 
    snake[i][Y] == snake[lengthSnake - 1][Y]) {
      gameOver = true;
    }
  }

}

void setPixel(uint8_t x, uint8_t y){
  picture[x][y] = 1;
}
void clearPixel(uint8_t x, uint8_t y){
  picture[x][y] = 0;
}

