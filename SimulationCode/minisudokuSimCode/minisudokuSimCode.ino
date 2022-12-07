#include <LiquidCrystal_I2C.h>

//comment this out to disable debug messages to serial monitor
//#define DEBUG


unsigned long myTime;
const char chars[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '.', ',', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };  //add any additional characters


int charindex = 0;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int charsSize = 0;
int cx = 0;
int cy = 0;
const int keyUp = 2;
const int keyRight = 3;
const int keyDown = 4;
const int keyLeft = 5;
const int keySelect = 6;

LiquidCrystal_I2C lcd(0x27, 20, 4);

/*const size_t sizeX = 4; //this may not be worthwhile for our purposes
const size_t sizeY = 4;  //change to 4 once we have the proper display*/
int numberGrid[4][4] = {
  0
  // { 1, 3, 0, 4 },
  // { 4, 2, 3, 1 },
  // { 3, 4, 1, 2 },
  // { 2, 1, 4, 3 }//numbers for testing
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
#ifdef DEBUG
  Serial.begin(9600);
#endif

  randomSeed(analogRead(0));

  //give 4 starting numbers from 1 to 4 at random positions
  randomStartingNumbers();

  lcd.init();
  lcd.backlight();
  lcd.clear();

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
  delay(20);
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

  return buttonPressed;
}

void gameCursor(void) {
  switch (buttonPress()) {
    case 1:
#ifdef DEBUG
      Serial.println("UP has been pressed\n");
#endif
      if (cursorY != 0) {
        cursorY--;
      } else {
        cursorY = 3;
      }
      break;

    case 2:
#ifdef DEBUG
      Serial.println("RIGHT has been pressed\n");
#endif
      if (cursorX != 3) {
        cursorX++;
      } else {
        //cursorX = 0;//replace with go to side menu
        sideMenu();
      }
      break;

    case 3:
#ifdef DEBUG
      Serial.println("DOWN has been pressed");
#endif
      if (cursorY != 3) {
        cursorY++;
      } else {
        cursorY = 0;
      }
      break;

    case 4:
#ifdef DEBUG
      Serial.println("LEFT has been pressed");
#endif
      if (cursorX != 0) {
        cursorX--;
      } else {
        //cursorX = 3;//replace with go to side menu
        sideMenu();
      }
      break;

    case 5:
#ifdef DEBUG
      Serial.println("SELECT has been pressed");
#endif
      //allow input if isn't a starting number
      if (!isStartingNumber()) {
        inputNumber();
      }
      //after input, check if the game is won

      lcd.noCursor();  //hide cursor during calculation
      if (numberValidityCheck(numberGrid) == 0) {
        lcd.cursor();  //enable cursor again
        gameVictory();
      }
      lcd.cursor();  //enable cursor again

      break;

    default:
#ifdef DEBUG
      Serial.println("error during button press");
#endif
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
#ifdef DEBUG
          Serial.println("UP has been pressed during input");
#endif
          lcd.print(1);
          numberGrid[cursorY][cursorX] = 1;
          break;

        case 2:
#ifdef DEBUG
          Serial.println("RIGHT has been pressed during input");
#endif
          lcd.print(2);
          numberGrid[cursorY][cursorX] = 2;
          break;

        case 3:
#ifdef DEBUG
          Serial.println("DOWN has been pressed during input");
#endif
          lcd.print(3);
          numberGrid[cursorY][cursorX] = 3;
          break;

        case 4:
#ifdef DEBUG
          Serial.println("LEFT has been pressed during input");
#endif
          lcd.print(4);
          numberGrid[cursorY][cursorX] = 4;
          break;

        case 5:
#ifdef DEBUG
          Serial.println("SELECT has been pressed during input");
#endif
          break;

        default:
#ifdef DEBUG
          Serial.println("error during button press during input");
#endif
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
#ifdef DEBUG
          Serial.println("UP has been pressed during sidemenu");
#endif
          if (sideMenuPosition == 0) {
            lcd.setCursor(14, 1);
            sideMenuPosition = 1;
          } else {
            lcd.setCursor(14, 0);
            sideMenuPosition = 0;
          }
          break;

        case 2:
#ifdef DEBUG
          Serial.println("RIGHT has been pressed during sidemenu");
#endif
          //move to left side of game grid / back
          cursorX = 0;
          inputCleared = true;
          break;

        case 3:
#ifdef DEBUG
          Serial.println("DOWN has been pressed during sidemenu");
#endif
          if (sideMenuPosition == 1) {
            lcd.setCursor(14, 0);
            sideMenuPosition = 0;
          } else {
            lcd.setCursor(14, 1);
            sideMenuPosition = 1;
          }
          break;

        case 4:
#ifdef DEBUG
          Serial.println("LEFT has been pressed during sidemenu");
#endif
          //move to right side of game grid / back
          cursorX = 3;
          inputCleared = true;
          break;

        case 5:
#ifdef DEBUG
          Serial.println("SELECT has been pressed during sidemenu");
#endif
          if (sideMenuPosition == 0) {
            gameReset();
          } else {
            //show highscores
          }
          inputCleared = true;
          break;

        default:
#ifdef DEBUG
          Serial.println("error during button press during sidemenu");
#endif
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

#ifdef DEBUG
    Serial.println((String) "TEST:" + startingNumPos[i][0] + " and " + startingNumPos[i][1]);
#endif

    //check if position already has a number
    if (numberGrid[randomNum1][randomNum2] != 0) {
      i--;  //decrement i because we want to run this process until all four positions are unique
      //this has a theoretical possibility to never resolve, but it is too unlikely
      //if this seems slow we can make it so it's hardwired to go to x square and if that fails then y and so on
    } else {
      //there was no starting number here so we place it here
      numberGrid[randomNum1][randomNum2] = i + 1;
    }
  }
}

bool isStartingNumber(void) {
  bool isStarter = false;

  for (int i = 0; i < 4; i++) {
    if (cursorY == startingNumPos[i][0] && cursorX == startingNumPos[i][1]) {
      isStarter = true;

#ifdef DEBUG
      Serial.println("Can't change a starting number!");
#endif
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

          //if any number encountered is zero, just exit immediately
          if (gridNums[i][j] == 0) {
            return 1;
          }

#ifdef DEBUG
          Serial.println((String) "on row " + i + ": " + j + ".num = " + k + ".num");
#endif

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
#ifdef DEBUG
          Serial.println((String) "on col " + i + ": " + j + ".num = " + k + ".num");
#endif

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
#ifdef DEBUG
          Serial.println((String) "on subgrid " + i + ": " + j + ".num = " + k + ".num");
#endif

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
  //laskurit liikkumista ja koordinaatteja varten
  int count = -1;
  int loc = 0;
  int locY = 0;
  //tulostetaan voiton onnittelut
  lcd.begin(16, 2);
  lcd.print("YOU WON!");
  lcd.setCursor(2, 4);
  delay(1000);
  lcd.clear();
  lcd.print("GIVE PLAYER NAME");
  delay(1000);
  //tulostetaan nimensyöttönäkymä ja peruutus/submit painikkeet
  lcd.clear();
  lcd.print("______");
  lcd.setCursor(0, 1);
  lcd.print("cancel    submit");
  lcd.setCursor(0, 0);

  lcd.blink();

  while (buttonPress() != 0) {
  }
  lcd.blink();
  bool inputCleared = false;

  while (!inputCleared) {
    if (buttonPress() != 0) {
      switch (buttonPress()) {
        //UP painikkeen toiminnot
        case 1:
          Serial.println("UP has been pressed during input");
          //Kirjaimen valinta, jos kursori ylärivillä
          if (locY == 0) {  //hypätään kirjainten alkuun, jos lopussa
            if (count == sizeof(chars) / sizeof(char) - 1) {
              count = -1;
            }
            count = count + 1;         //kirjaimen valintalaskuri eteenpäin
            lcd.setCursor(loc, locY);  // asetetaan sarake, johon tulostetaan
            lcd.print(chars[count]);   // tulostetaan kirjain
            break;
          }
          //Jos kursori alarivillä SUBMIT-kohdassa, siirrytään yläriville nimisarakkeen loppuun
          else if (locY == 1 && loc == 12) {
            loc = 5;
            locY = 0;
            lcd.setCursor(loc, locY);
          }
          //Kursori muualla, siirrytään yläriville
          else {
            locY = 0;
            lcd.setCursor(loc, locY);
          }
          break;

          //RIGHT-painikkeen toiminnot
        case 2:
          Serial.println("RIGHT has been pressed during input");
          count == 0;
          //Jos kursori nimisarakkeen päässä, siirretään laskuri nollaan
          if (loc == 5) {
            loc = 0;
          }
          //Jos kursori alarivillä CANCEL-painikkeen kohdalla, siirretään kohtaan SUBMIT
          else if (locY == 1 && loc == 2) {
            loc = 12;
            lcd.setCursor(loc, locY);
          }
          //Jos kursori alarivillä SUBMIT-painikkeen kohdalla, ns. älä tee mitään
          else if (locY == 1 && loc == 12) {
            lcd.setCursor(loc, locY);
          }
          //Jos kursori on ylärivillä, siirrä askel oikealle
          else {
            loc++;
            lcd.setCursor(loc, locY);
          }

          break;
          //DOWN-painikkeen toiminnot
        case 3:
          Serial.println("DOWN has been pressed during input");
          //Kirjaimen valinta taaksepäin, jos kirjaimet char-taulukon alussa, siirry loppuun
          if (count == 38 || count <= 0) {
            count = 38;
          }
          count = count - 1;        //kirjainlaskuria taaksepäin
          lcd.setCursor(loc, 0);    //asetetaan sarake, johon tulostetaan
          lcd.print(chars[count]);  //tulostetaan kirjain
          break;
          //LEFT-painikkeen toiminnot
        case 4:
          Serial.println("LEFT has been pressed during input");
          //Jos kursori nimisarakkeen alussa, siirrä loppuun
          if (loc == 0) {
            loc = 5;
          }
          //Jos kursori alarivillä, SUBMIT-painikkeen kohdalla, siirrä kohtaan CANCEL
          else if (locY == 1 && loc == 12) {
            loc = 2;
            lcd.setCursor(loc, locY);

          }
          //Jos kursori alarivillä, CANCEL-painikkeen kohdalla, ns. älä tee mitään
          else if (locY == 1 && loc == 2) {
            lcd.setCursor(loc, locY);

          }
          //Muuten siirrä kursoria askel taaksepäin
          else {
            loc = loc - 1;
            lcd.setCursor(loc, locY);
          }
          break;
          //SELECT-painikkeen toiminnot, toisin sanoen siirry alariville
        case 5:
          Serial.println("SELECT has been pressed during input");
          locY++;
          lcd.setCursor(2, locY);
          if (locY == 2 && loc == 0) {

            gameReset();
          }
          break;


          //virheviesti
        default:
          Serial.println("error during button press during input");
          break;
      }
      while (buttonPress() != 0) {
      }
      //inputCleared = true;
    }
  }
  lcd.noBlink();
}

void gameReset(void) {
  lcd.clear();

  //fill array with zeros
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      numberGrid[i][j] = 0;
    }
  }

#ifdef DEBUG
  for (int i = 0; i < 4; i++) {
    Serial.println();
    for (int j = 0; j < 4; j++) {
      Serial.print(numberGrid[i][j]);
    }
  }
#endif

  //setup(); //this alone works for about 4 games and then the board supposedly runs out of memory
  
  //new attempt
  //give 4 starting numbers from 1 to 4 at random positions
  randomStartingNumbers();

  // lcd.init();
  // lcd.backlight();
  lcd.clear();

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
  
  lcd.cursor();
}