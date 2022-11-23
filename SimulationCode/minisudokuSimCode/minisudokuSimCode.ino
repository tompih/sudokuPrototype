#include <Adafruit_LiquidCrystal.h>

unsigned long myTime;

const int keyUp = 2;
const int keyRight = 3;
const int keyDown = 4;
const int keyLeft = 5;
const int keySelect = 6;

Adafruit_LiquidCrystal lcd(0);

/*const size_t sizeX = 4; //this may not be worthwhile for our purposes
const size_t sizeY = 4;  //change to 4 once we have the proper display*/
int numberGrid[4][4] = {
  /*{ 1, 3, 0, 4 },d swdwxqdes
  { 4, 2, 3, 1 },
  { 3, 4, 1, 2 },
  { 2, 1, 4, 3 }//numbers for testing*/
};

int startingNumPos[4][2] = {
  { 0, 0 },
  { 0, 0 },
  { 0, 0 },
  { 0, 0 },
};

int cursorX = 0;
int cursorY = 0;

void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(0));

  //give 4 starting numbers
  randomStartingNumbers();

  lcd.begin(16, 2);

  pinMode(keyUp, INPUT_PULLUP);
  pinMode(keyRight, INPUT_PULLUP);
  pinMode(keyDown, INPUT_PULLUP);
  pinMode(keyLeft, INPUT_PULLUP);
  pinMode(keySelect, INPUT_PULLUP);


  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (numberGrid[i][j] != 0) {
        lcd.print(numberGrid[i][j]);
      } else {
        lcd.print(" ");
      }
    }
    lcd.setCursor(0, i + 1);
  }
  lcd.rightToLeft();
  lcd.setCursor(13, 0);
  lcd.print("teseR");
  lcd.setCursor(13, 1);
  lcd.print("serocS");
  lcd.leftToRight();

  lcd.setCursor(cursorX, cursorY);

  //lcd.blink(); // turn on the blinking cursor
  lcd.cursor();
}

void loop() {

  if (buttonPress() != 0) {
    gameCursor();
    lcd.setCursor(cursorX, cursorY);

    while (buttonPress() != 0) {
    }
  }
  
}

//functions
int buttonPress(void) {
  int buttonPressed = 0;
  if (digitalRead(keyUp) == 0) {
    buttonPressed = 1;
  }
  if (digitalRead(keyRight) == 0) {
    buttonPressed = 2;
  }
  if (digitalRead(keyDown) == 0) {
    buttonPressed = 3;
  }
  if (digitalRead(keyLeft) == 0) {
    buttonPressed = 4;
  }
  if (digitalRead(keySelect) == 0) {
    buttonPressed = 5;
  }
  
  /*
  while (digitalRead(keyUp) + digitalRead(keyRight) + 
         digitalRead(keyDown) + digitalRead(keyLeft) + 
         digitalRead(keySelect) < 5) {
      Serial.println("key is still pressed");
  }*/

  return buttonPressed;
}

void gameCursor(void) {
  switch (buttonPress()) {
    case 1:
      Serial.println("UP has been pressed\n");
      if (cursorY != 0) {
        cursorY--;
      } else {
        cursorY = 1;
      }
      break;

    case 2:
      Serial.println("RIGHT has been pressed\n");
      if (cursorX != 3) {
        cursorX++;
      } else {
        //cursorX = 0;//replace with go to side menu
        sideMenu();
      }
      break;

    case 3:
      Serial.println("DOWN has been pressed");
      if (cursorY != 1) {
        cursorY++;
      } else {
        cursorY = 0;
      }
      break;

    case 4:
      Serial.println("LEFT has been pressed");
      if (cursorX != 0) {
        cursorX--;
      } else {
        //cursorX = 3;//replace with go to side menu
        sideMenu();
      }
      break;

    case 5:
      Serial.println("SELECT has been pressed");
      //allow input if isn't a starting number
      if (!isStartingNumber()) {
        inputNumber();
      }
      //after input, check if the game is won
      if (numberValidityCheck(numberGrid) == 0) {
        gameVictory();
      }

      break;

    default:
      Serial.println("error during button press");
      break;
  }
}

void inputNumber(void) {
  while (buttonPress() != 0) {
  }
  lcd.blink();
  bool inputCleared = false;

  while (!inputCleared) {
    if (buttonPress() != 0) {
      switch (buttonPress()) {
        case 1:
          Serial.println("UP has been pressed during input");
          lcd.print(1);
          numberGrid[cursorY][cursorX] = 1;
          break;

        case 2:
          Serial.println("RIGHT has been pressed during input");
          lcd.print(2);
          numberGrid[cursorY][cursorX] = 2;
          break;

        case 3:
          Serial.println("DOWN has been pressed during input");
          lcd.print(3);
          numberGrid[cursorY][cursorX] = 3;
          break;

        case 4:
          Serial.println("LEFT has been pressed during input");
          lcd.print(4);
          numberGrid[cursorY][cursorX] = 4;
          break;

        case 5:
          Serial.println("SELECT has been pressed during input");
          break;

        default:
          Serial.println("error during button press during input");
          break;
      }
      while (buttonPress() != 0) {
      }
      inputCleared = true;
    }
  }
  lcd.noBlink();
}

void sideMenu(void) {
  lcd.setCursor(14, 0);
  int sideMenuPosition = 0;

  while (buttonPress() != 0) {
  }
  lcd.blink();
  bool inputCleared = false;

  while (!inputCleared) {
    if (buttonPress() != 0) {
      switch (buttonPress()) {
        case 1:
          Serial.println("UP has been pressed during sidemenu");
          if (sideMenuPosition == 0) {
            lcd.setCursor(14, 1);
            sideMenuPosition = 1;
          } else {
            lcd.setCursor(14, 0);
            sideMenuPosition = 0;
          }
          break;

        case 2:
          Serial.println("RIGHT has been pressed during sidemenu");
          //move to left side of game grid / back
          cursorX = 0;
          inputCleared = true;
          break;

        case 3:
          Serial.println("DOWN has been pressed during sidemenu");
          if (sideMenuPosition == 1) {
            lcd.setCursor(14, 0);
            sideMenuPosition = 0;
          } else {
            lcd.setCursor(14, 1);
            sideMenuPosition = 1;
          }
          break;

        case 4:
          Serial.println("LEFT has been pressed during sidemenu");
          //move to right side of game grid / back
          cursorX = 3;
          inputCleared = true;
          break;

        case 5:
          Serial.println("SELECT has been pressed during sidemenu");
          if (sideMenuPosition == 0) {
            gameReset();
          } else {
            //show highscores
          }
          inputCleared = true;
          break;

        default:
          Serial.println("error during button press during sidemenu");
          break;
      }
      while (buttonPress() != 0) {
      }
    }
  }
  lcd.noBlink();
}

void randomStartingNumbers(void) {
  /* //Print 50 random numbers from 0 to 3
  for (int i = 0; i < 50; i++) {
    Serial.println(random(4));
  }*/

  int randomNum1 = 0;
  int randomNum2 = 0;

  for (int i = 0; i < 4; i++) {
    randomNum1 = random(4);
    randomNum2 = random(4);

    startingNumPos[i][0] = randomNum1;
    startingNumPos[i][1] = randomNum2;

    Serial.println((String) "TEST:" + startingNumPos[i][0] + " and " + startingNumPos[i][1]);

    numberGrid[randomNum1][randomNum2] = i + 1;
  }
}

bool isStartingNumber(void) {
  bool isStarter = false;

  for (int i = 0; i < 4; i++) {
    if (cursorY == startingNumPos[i][0] && cursorX == startingNumPos[i][1]) {
      isStarter = true;
      Serial.println("Can't change a starting number!");
    }
  }

  return isStarter;
}

int numberValidityCheck(int gridNums[4][4]) {
  int numVal = 0;  //set to 1 on fail

  //make subgrids easier to manage
  int subgridNums[4][4] = {
    { gridNums[0][0], gridNums[0][1], gridNums[1][0], gridNums[1][1] },
    { gridNums[0][2], gridNums[0][3], gridNums[1][2], gridNums[1][3] },
    { gridNums[2][0], gridNums[2][1], gridNums[3][0], gridNums[3][1] },
    { gridNums[2][2], gridNums[2][3], gridNums[3][2], gridNums[3][3] }
  };

  //compare numbers in rows
  //i=row, j=current number, k=number to compare against
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        if (gridNums[i][j] == gridNums[i][k] && j != k) {
          Serial.println((String) "on row " + i + ": " + j + ".num = " + k + ".num");
          numVal = 1;
        }
      }
    }
  }

  //compare numbers in columns
  //i=column, j=current number, k=number to compare against
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        if (gridNums[j][i] == gridNums[k][i] && j != k) {
          Serial.println((String) "on col " + i + ": " + j + ".num = " + k + ".num");
          numVal = 1;
        }
      }
    }
  }

  //compare numbers in subgrids
  //since subgrids were converted into easy form we can reuse this easy code
  //i=subgrid, j=current number, k=number to compare against
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        if (subgridNums[i][j] == subgridNums[i][k] && j != k) {
          Serial.println((String) "on subgrid " + i + ": " + j + ".num = " + k + ".num");
          numVal = 1;
        }
      }
    }
  }

  //sum up the numbers and see if the grid is full (1+2+3+4)*4=40
  int sum = 0;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      sum += gridNums[i][j];
    }
  }
  //printf("%d\n", sum); //for debugging
  if (sum != 40) numVal = 1;

  return numVal;
}

void gameVictory(void) {
  Serial.println("YOU WON!!!!!!!!!!!!!!!!!!!!!!\n");

  lcd.setCursor(5, 0);
  lcd.print("YOU");
  lcd.setCursor(5, 1);
  lcd.print("WON");
  lcd.setCursor(cursorX, cursorY);

  for (int i = 0; i < 10; i++) {
    lcd.setBacklight(0);
    delay(50);
    lcd.setBacklight(1);
    delay(50);
  }
}

void gameReset(void) {
  lcd.clear();

  //fill array with zeros
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      numberGrid[i][j] = 0;
    }
  }

  //debug
  for (int i = 0; i < 4; i++) {
    Serial.println();
    for (int j = 0; j < 4; j++) {
      Serial.print(numberGrid[i][j]);
    }
  }

  setup();
}
