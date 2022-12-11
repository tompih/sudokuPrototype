#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

//comment this out to disable debug messages to serial monitor
#define DEBUG

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long finalTime = 0;

int playerScore = 0;  // this will be time + penalty, par scoring

const char chars[] = {
  ' ',
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  'V',
  'W',
  'X',
  'Y',
  'Z',
};  //add any additional characters
char taulukko[6] = { ' ', ' ', ' ', ' ', ' ', ' ' };
int rawData[27] = { 0 };  //store data from eeprom here first to do sorting and such with it

int charindex = 0;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int charsSize = 0;
int cx = 0;
int cy = 0;

//name pins
const int keyUp = 2;
const int keyRight = 3;
const int keyDown = 4;
const int keyLeft = 5;
const int keySelect = 6;

LiquidCrystal_I2C lcd(0x27, 20, 4);

//table for our grid
int numberGrid[4][4] = { 0 };

//table for starting position tracking
int startingNumPos[4][2] = {
  { 0, 0 },
  { 0, 0 },
  { 0, 0 },
  { 0, 0 },
};

//custom characters for lcd
//inverted colors for starting numbers
byte inverted1[8] = {
  B11011,
  B10011,
  B11011,
  B11011,
  B11011,
  B11011,
  B10001,
};
byte inverted2[8] = {
  B10001,
  B01110,
  B11110,
  B11101,
  B11011,
  B10111,
  B00000,
};
byte inverted3[8] = {
  B00000,
  B11101,
  B11011,
  B11101,
  B11110,
  B01110,
  B10001,
};
byte inverted4[8] = {
  B11101,
  B11001,
  B10101,
  B01101,
  B00000,
  B11101,
  B11101,
};

//cursor tracking
int cursorX = 0;
int cursorY = 0;

//initialize hardware
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  //get seed from floating port
  randomSeed(analogRead(0));

  //start lcd
  lcd.init();
  lcd.backlight();
  lcd.clear();

  //add our custom characters to the lcd
  lcd.createChar(1, inverted1);
  lcd.createChar(2, inverted2);
  lcd.createChar(3, inverted3);
  lcd.createChar(4, inverted4);

  //pullup mode to detect grounded states
  pinMode(keyUp, INPUT_PULLUP);
  pinMode(keyRight, INPUT_PULLUP);
  pinMode(keyDown, INPUT_PULLUP);
  pinMode(keyLeft, INPUT_PULLUP);
  pinMode(keySelect, INPUT_PULLUP);

  //(re)start the game
  gameReset();
}

//main loop
void loop() {
  //check for button presses
  if (buttonPress() != 0) {
    gameCursor();                     //go to input management
    lcd.setCursor(cursorX, cursorY);  //return cursors to where it belongs

    //wait until the player isn't pressing anything
    while (buttonPress() != 0) {
    }
  }
}

//checks for button presses
int buttonPress(void) {
  delay(20);  //slight delay to combat duplicate inputs

  int buttonPressed = 0;

  //assigns return values based on key pressed
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

//input system while in main grid area
void gameCursor(void) {
  switch (buttonPress()) {
    case 1:  //up
#ifdef DEBUG
      Serial.println("UP has been pressed\n");
#endif
      //if cursor is at the edge, roll over to the bottom, else move up
      if (cursorY != 0) {
        cursorY--;
      } else {
        cursorY = 3;
      }
      break;

    case 2:  //right
#ifdef DEBUG
      Serial.println("RIGHT has been pressed\n");
#endif
      //if cursor is at the edge, go to side menu , else move right
      if (cursorX != 3) {
        cursorX++;
      } else {
        sideMenu();
      }
      break;

    case 3:  //down
#ifdef DEBUG
      Serial.println("DOWN has been pressed");
#endif
      //if cursor is at the edge, roll over to the top, else move down
      if (cursorY != 3) {
        cursorY++;
      } else {
        cursorY = 0;
      }
      break;

    case 4:  //left
#ifdef DEBUG
      Serial.println("LEFT has been pressed");
#endif
      //if cursor is at the edge, go to side menu, else move left
      if (cursorX != 0) {
        cursorX--;
      } else {
        sideMenu();
      }
      break;

    case 5:  //select
#ifdef DEBUG
      Serial.println("SELECT has been pressed");
#endif
      //allow input if selection isn't a starting number
      if (!isStartingNumber()) {
        inputNumber();
      } else {
        //alert player that the current selection is a starting number
        //this also works as an additional penalty since it takes up some time
        for (int i = 0; i < 5; i++) {
          lcd.setCursor(5, 0);
          lcd.print("Cant");
          lcd.setCursor(5, 1);
          lcd.print("change");
          lcd.setCursor(5, 2);
          lcd.print("starting");
          lcd.setCursor(5, 3);
          lcd.print("number!");

          delay(350);

          lcd.setCursor(5, 0);
          lcd.print("    ");
          lcd.setCursor(5, 1);
          lcd.print("      ");
          lcd.setCursor(5, 2);
          lcd.print("        ");
          lcd.setCursor(5, 3);
          lcd.print("       ");

          delay(50);
        }

        //bring back cursor
        lcd.setCursor(cursorX, cursorY);
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

//input system while square in grid has been selected
void inputNumber(void) {
  //wait until the player isn't pressing anything
  while (buttonPress() != 0) {
  }

  lcd.blink();                //enable blinking cursor
  bool inputCleared = false;  //track if player has entered an input

  //loop as long as no inputs have been received
  while (!inputCleared) {
    if (buttonPress() != 0) {
      switch (buttonPress()) {
        case 1:  //up
#ifdef DEBUG
          Serial.println("UP has been pressed during input");
#endif
          lcd.print(1);                      //prints to lcd (visual)
          numberGrid[cursorY][cursorX] = 1;  //stores value to table
          break;

        case 2:  //right
#ifdef DEBUG
          Serial.println("RIGHT has been pressed during input");
#endif
          lcd.print(2);                      //prints to lcd (visual)
          numberGrid[cursorY][cursorX] = 2;  //stores value to table
          break;

        case 3:  //down
#ifdef DEBUG
          Serial.println("DOWN has been pressed during input");
#endif
          lcd.print(3);                      //prints to lcd (visual)
          numberGrid[cursorY][cursorX] = 3;  //stores value to table
          break;

        case 4:  //left
#ifdef DEBUG
          Serial.println("LEFT has been pressed during input");
#endif
          lcd.print(4);                      //prints to lcd (visual)
          numberGrid[cursorY][cursorX] = 4;  //stores value to table
          break;

        case 5:  //select
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
      //wait until the player isn't pressing anything
      while (buttonPress() != 0) {
      }
      inputCleared = true;  //we can now exit the loop
    }
  }
  lcd.noBlink();  //disable blinking cursor

  playerScore += 5;  //add 5 seconds to player time each select input
                     //12 inputs is the minimum amount so
                     //subtract 12 * 5 at the end to award player with 0 penalty
}

//input system while in side menu
void sideMenu(void) {
  lcd.setCursor(18, 0);      //move cursor to right side of side menu
  int sideMenuPosition = 0;  //default side menu position is 'Reset'

  //wait until the player isn't pressing anything
  while (buttonPress() != 0) {
  }

  lcd.blink();                //enable blinking cursor
  bool inputCleared = false;  //track if player has entered an input

  //loop as long as no inputs have been received
  while (!inputCleared) {
    if (buttonPress() != 0) {
      switch (buttonPress()) {
        case 1:  //up
#ifdef DEBUG
          Serial.println("UP has been pressed during sidemenu");
#endif
          //switch between side menu positions
          if (sideMenuPosition == 0) {
            lcd.setCursor(18, 1);
            sideMenuPosition = 1;
          } else {
            lcd.setCursor(18, 0);
            sideMenuPosition = 0;
          }
          break;

        case 2:  //right
#ifdef DEBUG
          Serial.println("RIGHT has been pressed during sidemenu");
#endif
          //move to left side of the game grid / back
          cursorX = 0;
          inputCleared = true;  //we can set this to true if input was not up or down
          break;

        case 3:  //down
#ifdef DEBUG
          Serial.println("DOWN has been pressed during sidemenu");
#endif
          //switch between side menu positions
          if (sideMenuPosition == 1) {
            lcd.setCursor(18, 0);
            sideMenuPosition = 0;
          } else {
            lcd.setCursor(18, 1);
            sideMenuPosition = 1;
          }
          break;

        case 4:  //left
#ifdef DEBUG
          Serial.println("LEFT has been pressed during sidemenu");
#endif
          //move to right side of game grid / back
          cursorX = 3;
          inputCleared = true;  //we can set this to true if input was not up or down
          break;

        case 5:  //select
#ifdef DEBUG
          Serial.println("SELECT has been pressed during sidemenu");
#endif
          if (sideMenuPosition == 0) {
            gameReset();  //reset the game
          } else {
            playerScore = 9999;  //make player score so high it won't beat any existing ones
            highscoreSystem();   //show highscores
          }
          inputCleared = true;  //we can set this to true if input was not up or down
          break;

        default:
#ifdef DEBUG
          Serial.println("error during button press during sidemenu");
#endif
          break;
      }

      //wait until the player isn't pressing anything
      while (buttonPress() != 0) {
      }
    }
  }
  lcd.noBlink();  //disable blinking cursor
}

//creates starting numbers
void randomStartingNumbers(void) {
  int tempSubgrids[4] = { 0 };  //temporary array to track which subgrids have been taken
  int randomSubgrid = 0;
  int row = 0;
  int col = 0;

  //assign numbers 1-4 to random positions
  for (int i = 0; i < 4; i++) {
    //what subgrid we're using and which row and column we're placing the number in
    randomSubgrid = random(4);
    row = random(2);
    col = random(2);

    //subgrid has already been used
    if (tempSubgrids[randomSubgrid] != 0) {
      i--;  //run this loop until all subgrids have a number
    } else {
      tempSubgrids[randomSubgrid] = 1;  //mark subgrid as taken

      //3 and 4 are on the next row of subgrids
      if (randomSubgrid >= 2) {
        row += 2;
      }
      //4 and 2 are on the next column of subgrids
      if (randomSubgrid % 2 == 1) {
        col += 2;
      }

      numberGrid[row][col] = i + 1; //place the number

      //log our starting number position
      startingNumPos[i][0] = row;
      startingNumPos[i][1] = col;

#ifdef DEBUG
      Serial.println((String)i + ". starting num pos:" + startingNumPos[i][0] + " and " + startingNumPos[i][1]);
#endif
    }
  }
}

//checks if current position of cursor is a starting number
bool isStartingNumber(void) {
  bool isStarter = false;

  //rolls through all the starting number positions to see if y and x coordinates match
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

//checks if the game is valid and won
//there must be no duplicate numbers for any row, columns and subgrids, and the grid's sum has to be correct
int numberValidityCheck(int gridNums[4][4]) {
  int numVal = 0;  //set to 1 on fail

  //make subgrids easier to manage / visualize
  //if we had a bigger screen that facilitated bigger grids
  //we should make rework this into a more generalized solution
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

        //if number is a match AND isn't comparing to itself
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

        //if number is a match AND isn't comparing to itself
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

        //if number is a match AND isn't comparing to itself
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

  if (sum != 40) numVal = 1;  //sum was incorrect so the grid must be invalid

  return numVal;
}

//game victory sequence
void gameVictory(void) {
  endTime = millis();
  finalTime = (endTime - startTime) / 1000;
  playerScore -= 12 * 5;     //subtract minimum penalty from score
  playerScore += finalTime;  //add time to score
#ifdef DEBUG
  Serial.print("Time: ");
  Serial.println(finalTime);
  Serial.print("Penalty: ");
  Serial.println(playerScore - finalTime);
#endif

#ifdef DEBUG
  Serial.println("YOU WON!!!!!!!!!!!!!!!!!!!!!!\n");
#endif
  //laskurit liikkumista ja koordinaatteja varten
  int count = 0;
  int loc = 0;
  int locY = 0;
  //tulostetaan voiton onnittelut
  lcd.clear();
  lcd.print("YOU WON!");
  lcd.setCursor(0, 1);
  lcd.print("score:");
  lcd.setCursor(6, 1);
  lcd.print(playerScore);
  delay(5000);
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

  //Painikkeiden painallusten toiminnot
  while (buttonPress() != 0) {
  }
  lcd.blink();
  bool inputCleared = false;

  while (!inputCleared) {
    if (buttonPress() != 0) {
      switch (buttonPress()) {
        //UP painikkeen toiminnot
        cursorX = 0;
        cursorY = 0;
        case 1:
#ifdef DEBUG
          Serial.println("UP has been pressed during input");
#endif
          //Kirjaimen valinta, jos kursori ylärivillä

          if (locY == 1) {
            loc = 0;
            locY = 0;
            lcd.setCursor(loc, locY);
            //break;
          }
          //Kirjaimen valinta eteenpäin, jos kirjaimet char-taulukon lopussa, siirry alkuun
          else if (count != 26) {


            count = count + 1;         //kirjainlaskuria eteenpäin
            lcd.setCursor(loc, locY);  //asetetaan sarake, johon tulostetaan
            lcd.print(chars[count]);   //tulostetaan kirjain
            lcd.setCursor(loc, locY);
            taulukko[loc] = chars[count];
            // break;
          }

          else {
            count = 0;

            lcd.setCursor(loc, locY);  //asetetaan sarake, johon tulostetaan
            lcd.print(chars[count]);   //tulostetaan kirjain
            lcd.setCursor(loc, locY);
            taulukko[loc] = chars[count];
          }



          break;

          //RIGHT-painikkeen toiminnot
        case 2:
#ifdef DEBUG
          Serial.println("RIGHT has been pressed during input");
#endif
          count == 0;

          //Jos kursori nimisarakkeen päässä, siirretään laskuri nollaan ja kursori alkuun
          if (loc == 5 && locY == 0) {
            loc = 0;
            lcd.setCursor(loc, locY);
          }
          //Jos kursori on ylärivillä, siirrä askel oikealle
          else if (locY == 0) {
            loc++;
            lcd.setCursor(loc, locY);

            break;
          }
          //Jos kursori alarivillä CANCEL-painikkeen kohdalla, siirretään kohtaan SUBMIT
          else if (locY == 1 && loc == 5) {
            loc = loc + 10;
            lcd.setCursor(15, 1);
            break;
          }
          //Jos kursori alarivillä SUBMIT-painikkeen kohdalla, ns. älä tee mitään
          else if (locY == 1 && loc == 15) {
            //lcd.setCursor(loc, locY);
            break;
          }

          break;


          //DOWN-painikkeen toiminnot
        case 3:
#ifdef DEBUG
          Serial.println("DOWN has been pressed during input");
#endif
          if (locY == 1) {
            loc = 0;
            locY = 0;
            lcd.setCursor(loc, locY);
            //break;
          }
          //Kirjaimen valinta taaksepäin, jos kirjaimet char-taulukon alussa, siirry loppuun
          else if (count != 0) {


            count = count - 1;         //kirjainlaskuria taaksepäin
            lcd.setCursor(loc, locY);  //asetetaan sarake, johon tulostetaan
            lcd.print(chars[count]);   //tulostetaan kirjain
            lcd.setCursor(loc, locY);
            taulukko[loc] = chars[count];
            // break;
          }

          else {
            count = 26;

            lcd.setCursor(loc, locY);  //asetetaan sarake, johon tulostetaan
            lcd.print(chars[count]);   //tulostetaan kirjain
            lcd.setCursor(loc, locY);
            taulukko[loc] = chars[count];
          }


          break;
          //LEFT-painikkeen toiminnot
        case 4:
#ifdef DEBUG
          Serial.println("LEFT has been pressed during input");
#endif

          //Jos kursori nimisarakkeen alussa, siirrä loppuun
          if (loc == 0 && locY == 0) {
            loc = 5;
          }
          //Jos kursori alarivillä, SUBMIT-painikkeen kohdalla, siirrä kohtaan CANCEL
          else if (locY == 1 && loc == 15) {
            loc = loc - 10;
            lcd.setCursor(loc, locY);
            break;
          }
          //Jos kursori alarivillä, CANCEL-painikkeen kohdalla, ns. älä tee mitään
          else if (locY == 1 && loc == 5) {
            //lcd.setCursor(loc, locY);
            break;
          }
          //Muuten siirrä kursoria askel taaksepäin
          else if (locY == 0) {
            loc = loc - 1;
            lcd.setCursor(loc, locY);
          }
          break;
          //SELECT-painikkeen toiminnot, toisin sanoen siirry alariville

        case 5:
#ifdef DEBUG
          Serial.println("SELECT has been pressed during input");
#endif
          //Serial.println(taulukko);
          if (locY == 1 && loc == 5) {  //Jos kursori CANCEL-kohdassa, resetoi painamalla
            inputCleared = true;
            gameReset();
          }

          else if (locY == 1 && loc == 15) {  //Jos kursori SUBMIT-kohdassa, jatka highscoresysteemiin painamalla
            inputCleared = true;
            highscoreSystem();
          }

          if (locY == 0) {
            locY++;

            loc = 5;
            lcd.setCursor(loc, locY);  //siirretään alariville kursori
            //Serial.println(loc, locY); for testing purposes only
            for (int asd = 0; asd < 6; asd++) {  //tulostetaan sarjaväylälle kirjoitettu nimi
              if (taulukko[asd] != ' ') {
                Serial.print(taulukko[asd]);
              } else Serial.print(' ');
            }
            break;
          }



          break;
        default:
#ifdef DEBUG
          Serial.println("error during button press during input");
#endif
          break;
      }
      while (buttonPress() != 0) {
        //inputCleared = true;
      }
    }
  }
  lcd.noBlink();
}

//restarts the game by reassigning starting numbers, zeroing flags, counters, clearing screen etc.
void gameReset(void) {
  startTime = 0;
  endTime = 0;
  finalTime = 0;
  playerScore = 0;

  //fill array with zeros
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      numberGrid[i][j] = 0;
    }
  }

  //fill name array with spaces
  for (int i = 0; i < 6; i++) {
    taulukko[i] = ' ';
  }

#ifdef DEBUG
  //debug grid
  numberGrid[0][0] = 1;
  numberGrid[0][1] = 3;
  numberGrid[0][2] = 0;
  numberGrid[0][3] = 4;
  numberGrid[1][0] = 4;
  numberGrid[1][1] = 2;
  numberGrid[1][2] = 3;
  numberGrid[1][3] = 1;
  numberGrid[2][0] = 3;
  numberGrid[2][1] = 4;
  numberGrid[2][2] = 1;
  numberGrid[2][3] = 2;
  numberGrid[3][0] = 2;
  numberGrid[3][1] = 1;
  numberGrid[3][2] = 4;
  numberGrid[3][3] = 3;
#endif

#ifdef DEBUG
  for (int i = 0; i < 4; i++) {
    Serial.println();
    for (int j = 0; j < 4; j++) {
      Serial.print(numberGrid[i][j]);
    }
  }
#endif

  //setup(); //this alone works for about 4 games and then the board supposedly runs out of memory

#ifdef DEBUG
  //randomStartingNumbers();
#else
  //give 4 starting numbers from 1 to 4 at random positions
  randomStartingNumbers();
#endif

  lcd.clear();  //clear the screen

  //print starting numbers
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (numberGrid[i][j] != 0) {
        lcd.write(byte(numberGrid[i][j]));  //prints inverted numbers
      } else {
        lcd.print(" ");
      }
    }
    lcd.setCursor(0, i + 1);
  }

  //print side menu
  lcd.rightToLeft();
  lcd.setCursor(17, 0);
  lcd.print("teseR");
  lcd.setCursor(17, 1);
  lcd.print("serocS");
  lcd.leftToRight();

  lcd.setCursor(cursorX, cursorY);  //set cursor to default / last position

  lcd.cursor();  //enable cursor

  startTime = millis();
}

//EEPROM / highscore management
void highscoreSystem() {
#ifdef DEBUG
  Serial.println("Begin highscores...");
#endif

  getFromEEPROM();

  parseScores();

  sortTable();

  if (compareScoreToTable() == 1) {
    overwriteWorstScore();
  }

  sortTable();

  printTable();

  while (buttonPress() == 0) {
  }
  delay(50);
  while (buttonPress() != 0) {
  }
  gameReset();

#ifdef DEBUG
  Serial.println("End highscores...");
#endif
}

//get names from EEPROM addresses to the array
void getFromEEPROM() {
#ifdef DEBUG
  Serial.println("Begin getFromEEPROM...");
#endif

  //read from data at ith address in eeprom and put it in our runtime table
  for (int i = 0; i < 27; i++) {
    rawData[i] = EEPROM.read(i);  //get one byte from address i
  }

#ifdef DEBUG
  for (int i = 0; i < 27; i++) {
    Serial.print(rawData[i]);
  }
#endif

#ifdef DEBUG
  Serial.println("End getFromEEPROM...");
#endif
}

void parseScores() {
#ifdef DEBUG
  Serial.println("Begin parseScores...");
#endif

  for (int i = 1; i < 4; i++) {
    int sum = 0;
    int multiplier = 100;
    for (int j = 3; j > 0; j--) {
      //for example 2 * 9 - 3 = 15 which is the highest digit for the second score
      sum += rawData[i * 9 - j] * multiplier;  //multiplier starts from hundreds
      multiplier /= 10;                        //and keeps going down
    }
    rawData[i * 9 - 3] = sum;  //put whole parsed number to 7th place
    rawData[i * 9 - 2] = i;    //index score
  }

#ifdef DEBUG
  for (int i = 1; i < 4; i++) {
    Serial.print(rawData[i * 9 - 2]);
    Serial.print(". : ");
    Serial.println(rawData[i * 9 - 3]);
  }
#endif

#ifdef DEBUG
  Serial.println("End parseScores...");
#endif
}

//returns how many existing scores the player's score beats
int compareScoreToTable() {
#ifdef DEBUG
  Serial.println("Begin compareScoreToTable...");
#endif

  int betterThan = 0;

  //check if any existing score is worse than player's score
  for (int i = 1; i < 4; i++) {
#ifdef DEBUG
    Serial.print(rawData[i * 9 - 3]);
    Serial.print(" > ");
    Serial.print(playerScore);
    Serial.println(" ? ");
#endif
    if (rawData[i * 9 - 3] > playerScore) {

      betterThan = 1;
    }
  }

#ifdef DEBUG
  Serial.println("End compareScoreToTable...");
#endif

  return betterThan;
}

//replace worst score in table and also send it to EEPROM
void overwriteWorstScore() {
#ifdef DEBUG
  Serial.println("Begin overwriteWorstScore...");
#endif

  //overwrite name
  for (int k = 6; k > 0; k--) {
    rawData[3 * 9 - 3 - k] = taulukko[6 - k];

    //index * 9 - numberrange - k
    EEPROM.update((rawData[3 * 9 - 2] * 9 - 3 - k), rawData[3 * 9 - 3 - k]);
  }

  //overwrite score from table
  rawData[3 * 9 - 3] = playerScore;

  //unparse scores
  int temp[3] = { 0 };
  //for example
  //273 - 73 = 200 / 100 = 2
  //73 - 3 = 70 / 10 = 7
  //3                = 3
  temp[0] = ((playerScore) - (playerScore % 100)) / 100;
  temp[1] = ((playerScore % 100) - (playerScore % 10)) / 10;
  temp[2] = (playerScore % 10);

#ifdef DEBUG
  Serial.print("Overwriting: ");
  for (int k = 0; k < 3; k++) {
    Serial.print(EEPROM.read((rawData[3 * 9 - 2] * 9 - 3 + k)));
  }
  Serial.println("");
  Serial.print("With: ");
  for (int k = 0; k < 3; k++) {
    Serial.print(temp[k]);
  }
  Serial.println("");
#endif

  //overwrite score from EEPROM
  for (int k = 0; k < 3; k++) {
    //index * 9 - numberrange + k
    EEPROM.update((rawData[3 * 9 - 2] * 9 - 3 + k), temp[k]);
  }

#ifdef DEBUG
  Serial.println("End overwriteWorstScore...");
#endif
}

//bubblesort
void sortTable() {
#ifdef DEBUG
  Serial.println("Begin sortTable...");
#endif

  int numbertemp = 0;
  int nametemp[27] = { 0 };
  for (int i = 1; i < 4; i++) {
    for (int j = 1; j < 4; j++) {
      if (rawData[j * 9 - 3] > rawData[i * 9 - 3]) {
        //sort numbers
        numbertemp = rawData[i * 9 - 3];
        rawData[i * 9 - 3] = rawData[j * 9 - 3];
        rawData[j * 9 - 3] = numbertemp;

        //sort addresses
        numbertemp = rawData[i * 9 - 2];
        rawData[i * 9 - 2] = rawData[j * 9 - 2];
        rawData[j * 9 - 2] = numbertemp;

        //sort names
        for (int k = 6; k > 0; k--) {
          //for example 2 * 9 - 3 - 6 = 9 which is the start of second name
          nametemp[k] = rawData[i * 9 - 3 - k];
        }
        for (int k = 6; k > 0; k--) {
          rawData[i * 9 - 3 - k] = rawData[j * 9 - 3 - k];
        }
        for (int k = 6; k > 0; k--) {
          rawData[j * 9 - 3 - k] = nametemp[k];
        }
      }
    }
  }

#ifdef DEBUG
  char ch = '\0';
  for (int i = 1; i < 4; i++) {
    Serial.print(rawData[i * 9 - 2]);
    Serial.print(". address: ");
    for (int k = 6; k > 0; k--) {
      ch = rawData[i * 9 - 3 - k];
      Serial.print(ch);
    }
    Serial.println(rawData[i * 9 - 3]);
  }
#endif

#ifdef DEBUG
  Serial.println("End sortTable...");
#endif
}


void printTable() {


  //Print table of names and scores
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print("HIGH SCORES");
  lcd.setCursor(0, 1);
  lcd.print("#1:");
  lcd.setCursor(0, 2);
  lcd.print("#2:");
  lcd.setCursor(0, 3);
  lcd.print("#3:");



  char name;

  //henkilön nimi ja score
  for (int i = 1; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      lcd.setCursor(j + 3, i);
      name = rawData[i * 9 - 9 + j];
      lcd.print(name);
    }
    lcd.setCursor(10, i);
    lcd.print(rawData[i * 9 - 3]);
  }
}