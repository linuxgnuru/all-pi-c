/*
 Mastermind using a 4 x 4 keypad
 7 segment LED + shift register (74HC595) + keypad
 wiring for keypad: with pad facing you and starting from left to right; 

 +---+---+---+---+                shift register Pins:
 | 1 | 2 | 3 | A |                     +--------+
 +---+---+---+---+               1out -+ 1   16 +- Vin
 | 4 | 5 | 6 | B |               2out -+ 2   15 +- 0out
 +---+---+---+---+               3out -+ 3   14 +- data (blue)
 | 7 | 8 | 9 | C |               4out -+ 4   13 +- ground
 +---+---+---+---+               5out -+ 5   12 +- latch (green + 1u cap if first)
 | * | 0 | # | D |               6out -+ 6   11 +- lock (yellow)
 +---+---+---+---+               7out -+ 7   10 +- Vin
 |0|1|2|3|4|5|6|7| (pins used)    gnd -+ 8    9 +- serial out
 +-+-+-+-+-+-+-+-+                     +--------+
  | | | | | | | |
  + + + + + + + +

7-segment LED Pins:
        ____
 A  1--|    |--8  H        --    H
 B  2--|    |--7  G       |  | A   G
 X     |    |     X        --    B
  GND--|    |     X       |  | C   E
 X     |    |     X        -- .  D   F (.)
 C  3--|    |--6  F (.)
 D  4--|____|--5  E

        HGFEDCBA
0out = 0b10000000
1out = 0b01000000
2out = 0b00100000
3out = 0b00010000
4out = 0b00001000
5out = 0b00000100
6out = 0b00000010
7out = 0b00000001
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include <wiringPi.h>
//#include <sr595.h>

//#include "mastermind.h"

typedef enum {
  LED_R, // red
  LED_Y, // yellow
  LED_G, // green
  LED_0  // nothing
} 
led_enum;

// Defines
#define NUM_LEDS 4
#define MAX_DIG 10 // 10 = 0-9, 14 = 0-9, A-D

#define true 1
#define false 0

#define ROWS 4
#define COLS 4

int max_tries = 10;

/*
 * Shift register variables:
 */
const int dClockPin = 22; // Pin connected to SH_CP of 74HC595 - yellow wire
const int dLatchPin = 23; // Pin connected to ST_CP of 74HC595 - green wire
const int dDataPin  = 24;  // Pin connected to DS of 74HC595 - blue wire

const int rDataPin  = 25; // Pin connected to DS of 74HC595 - blue wire
const int rLatchPin = 26; // Pin connected to ST_CP of 74HC595 - green wire
const int rClockPin = 28; // Pin connected to SH_CP of 74HC595 - yellow wire

const int alertPin = 29;

/*
 * Keypad code:
 */

const int ROW[] = { 0, 1, 2, 3 }; // connect to row pinouts 
const int COL[] = { 4, 5, 6, 7 }; // connect to column pinouts

const char MATRIX[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

/*
 --    H
|  | A   G
 --    B
|  | C   E
 -- .  D   F (.)
*/
const int winAnim[] = {
  0b10000001,
  0b11000000,
  0b01000010,
  0b00000110,
  0b00001100,
  0b00011000,
  0b00010010,
  0b00000011
};

const int alphabetArray[] = {
  0b11010111, // A
  0b11011111, // B
  0b10001101, // C
  0b11111101, // D
  0b10001111, // E
  0b10000111, // F
  0b10011101, // G
  0b01010111, // H
  0b01010000, // I
  0b01011100, // J
  0b01110111, // K (H.)
  0b00001101, // L
  0b00110110, // M (n.) c b e f
  0b00010110, // N (n)  c b e
  0b11011101, // O
  0b11000111, // P
  0b00000000, // Q XX NOTHING
  0b00000110, // R (r)
  0b10011011, // S
  0b11010000, // T
  0b01011101, // U
  0b00011100, // V
  0b00000000, // W XX NOTHING
  0b00000000, // X XX NOTHING
  0b01011011, // Y
  0b00000000  // Z XX NOTHING
};

const int digitArray[] = {
  0b11011101, // 0
  0b01010000, // 1
  0b11001110, // 2
  0b11011010, // 3
  0b01010011, // 4
  0b10011011, // 5
  0b10011111, // 6
  0b11010000, // 7
  0b11011111, // 8
  0b11011011  // 9
};

/* ARDUINO ONLY */
//Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

long winPreviousMillis = 0;
long losePreviousMillis = 0;

const long interval = 300;

// Flags
int resetFlag = false;
int firstFlag = true;
int winFlag = false;
int loseFlag = false;
int winSwapFlag = false;
int loseSwapFlag = false;
int shuffleFlag = true;

// this will be randomly generated
char passcode[NUM_LEDS];
// users input
char answer[NUM_LEDS];
// result
int result[NUM_LEDS];
// what digits were picked
int used[MAX_DIG];

// for shift register leds:
int data1;
int data2;

// counters
int correct_index = 0;
int correct_counter = 0;
int num_tries = 0;

// Functions
void shiftOut(int myDataPin, int myClockPin, int myDataOut)
{
  int pinState;
  int i;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
  for (i = 7; i >= 0; i--)
  {
    digitalWrite(myClockPin, 0);
    pinState = (myDataOut & (1<<i) ? 1 : 0);
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
  }
  digitalWrite(myClockPin, 0);
}

// possibly combine both clears; and have a if flag
void shiftClear(int latchP, int dataP, int clockP, int bNum)
{
  digitalWrite(latchP, 0);
  shiftOut(dataP, clockP, 0b00000000);
  if (bNum == 2)
    shiftOut(dataP, clockP, 0b00000000);
  digitalWrite(latchP, 1);
}


void swap(int *a, int *b)
{
  int temp = *a;
  
  *a = *b;
  *b = temp;
}

void create_passcode()
{
    int i;
    int cur_index;
    int ran;

  for (i = 0; i < MAX_DIG; i++)
    used[i] = 0;
  for (cur_index = 0; cur_index < NUM_LEDS; cur_index++)
  {
    //ran = random(0, MAX_DIG); /* ARDUINO */
    ran = rand() % MAX_DIG;
    used[ran] = 1;
    passcode[cur_index] = ran + 48;
  }
}

void g2r(int d)
{
  int d1 = 0b00000000;
  int d2 = 0b00000000;
  int i;

  // loop through all printable digits for 7 segment LED
  for (i = 0; i < MAX_DIG; i++)
  {
    digitalWrite(dLatchPin, 0);
    shiftOut(dDataPin, dClockPin, digitArray[i]);
    digitalWrite(dLatchPin, 1);
    delay(250);
  }
  for (i = 7; i > -1; i--)
  {
    d1 |= 1 << i;
    digitalWrite(rLatchPin, 0);
    shiftOut(rDataPin, rClockPin, d1);
    shiftOut(rDataPin, rClockPin, d2);
    digitalWrite(rLatchPin, 1);
    delay(d);
    if (i != 5 && i != 2)
    {
      d1 &= ~(1 << i);
      digitalWrite(rLatchPin, 0);
      shiftOut(rDataPin, rClockPin, d1);
      shiftOut(rDataPin, rClockPin, d2);
      digitalWrite(rLatchPin, 1);
    }
  }
  d1 = 0b00100100;
  for (i = 7; i > 0; i--)
  {
    d2 |= 1 << i;
    digitalWrite(rLatchPin, 0);
    shiftOut(rDataPin, rClockPin, d1);
    shiftOut(rDataPin, rClockPin, d2);
    digitalWrite(rLatchPin, 1);
    delay(d);
    if (i != 7 && i != 4)
    {
      d2 &= ~(1 << i);
      digitalWrite(rLatchPin, 0);
      shiftOut(rDataPin, rClockPin, d1);
      shiftOut(rDataPin, rClockPin, d2);
      digitalWrite(rLatchPin, 1);
    }
  }
}

void blip(int times, int d)
{
  int i;

  for (i = 0; i < times; i++)
  {
    digitalWrite(alertPin, HIGH);
    delay(d);
    digitalWrite(alertPin, LOW);
    delay(d);
  }
}

void changeBit(int pos, int color)
{
  /*
  data1 = 0b00000000;
   data2 = 0b00000000;
   */
  switch (pos)
  {
  case 0:
    switch (color)
    {
    case LED_G:
      data1 |= 1 << 7;
      data1 &= ~(1 << 6);
      data1 &= ~(1 << 5);
      break;
    case LED_Y:
      data1 |= 1 << 6;
      data1 &= ~(1 << 7);
      data1 &= ~(1 << 5);
      break;
    case LED_R:
      data1 |= 1 << 5;
      data1 &= ~(1 << 7);
      data1 &= ~(1 << 6);
      break;
    case LED_0:
      data1 &= ~(1 << 5);
      data1 &= ~(1 << 7);
      data1 &= ~(1 << 6);
      break;
    }
    break;
  case 1:
    switch (color)
    {
    case LED_G:
      data1 |= 1 << 4;
      data1 &= ~(1 << 3);
      data1 &= ~(1 << 2);
      break;
    case LED_Y:
      data1 |= 1 << 3;
      data1 &= ~(1 << 4);
      data1 &= ~(1 << 2);
      break;
    case LED_R:
      data1 |= 1 << 2;
      data1 &= ~(1 << 4);
      data1 &= ~(1 << 3);
      break;
    case LED_0:
      data1 &= ~(1 << 2);
      data1 &= ~(1 << 3);
      data1 &= ~(1 << 4);
      break;
    }
    break;
  case 2:
    switch (color)
    {
    case LED_G:
      data1 |= 1 << 1;
      data1 &= ~(1 << 0);
      data2 &= ~(1 << 7);
      break;
    case LED_Y:
      data1 |= 1 << 0;
      data1 &= ~(1 << 1);
      data2 &= ~(1 << 7);
      break;
    case LED_R:
      data2 |= 1 << 7;
      data1 &= ~(1 << 1);
      data1 &= ~(1 << 0);
      break;
    case LED_0:
      data1 &= ~(1 << 1);
      data1 &= ~(1 << 0);
      data2 &= ~(1 << 7);
      break;
    }
    break;
  case 3:
    switch (color)
    {
    case LED_G:
      data2 |= 1 << 6;
      data2 &= ~(1 << 5);
      data2 &= ~(1 << 4);
      break;
    case LED_Y:
      data2 |= 1 << 5;
      data2 &= ~(1 << 6);
      data2 &= ~(1 << 4);
      break;
    case LED_R:
      data2 |= 1 << 4;
      data2 &= ~(1 << 6);
      data2 &= ~(1 << 5);
      break;
    case LED_0:
      data2 &= ~(1 << 4);
      data2 &= ~(1 << 5);
      data2 &= ~(1 << 6);
      break;
    }
    break;
  }
  digitalWrite(rLatchPin, 0);
  shiftOut(rDataPin, rClockPin, data1);
  shiftOut(rDataPin, rClockPin, data2);
  digitalWrite(rLatchPin, 1);
}


void printSingleDigit(char k, int cf)
{
  if (k != '*' && k != '#' && k != 'A' && k != 'B' && k != 'C' && k != 'D')
  {
    digitalWrite(dLatchPin, 0);
    shiftOut(dDataPin, dClockPin, digitArray[k - 48]);
    digitalWrite(dLatchPin, 1);
    if (cf == true)
    {
      delay(500);
      shiftClear(dLatchPin, dDataPin, dClockPin, 1);
    }
  }
}

void printMultDigit(char *text, int text_len)
{
  int i;

  for (i = 0; i < text_len; i++)
  {
    printSingleDigit(text[i], true);
    delay(100);
  }
}

void printNumLeft()
{
  int disp_tries = 0;
  char ten[2];

  disp_tries = abs(max_tries - num_tries);
  ten[0] = (max_tries > 9 ? (disp_tries / 10) + 48 : '0');
  ten[1] = (disp_tries % 10) + 48;
  printMultDigit(ten, 2);
}

void printMessage(char *message, int sz)
{
  int i;

  for (i = 0; i < sz; i++)
  {
    digitalWrite(dLatchPin, 0);
    shiftOut(dDataPin, dClockPin, alphabetArray[message[i] - 65]);
    digitalWrite(dLatchPin, 1);
    delay(300);
    shiftClear(dLatchPin, dDataPin, dClockPin, 1);
    delay(100);
  }
}

void showWin()
{
  int i, j;

  for (j = 0; j < 6; j++)
  {
    for (i = 0; i < 8; i++)
    {
      digitalWrite(dLatchPin, 0);
      shiftOut(dDataPin, dClockPin, winAnim[i]);
      digitalWrite(dLatchPin, 1);
      delay(90);
      shiftClear(dLatchPin, dDataPin, dClockPin, 1);
    }
  }
}

static void die(int sig)
{
    int i;

    // TODO add stuff to turn off the lights and stuff
    digitalWrite(alertPin, LOW);
	for (i = 0; i < 4; i++)
	       	digitalWrite(COL[i], LOW);
    shiftClear(rLatchPin, rDataPin, rClockPin, 2);
    shiftClear(dLatchPin, dDataPin, dClockPin, 1);
	if (sig != 0 && sig != 2)
		(void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

int main(int argc, char **argv)
{
    int i, j;
    char key;
    unsigned long loseCurrentMillis = millis();
    unsigned long winCurrentMillis = millis();
    //char key = keypad.getKey(); /* ARDUINO ONLY */
    int loopFlag = true;

    srand((unsigned)time(NULL));
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    wiringPiSetup();
	for (i = 0; i < 4; i++)
		pinMode(COL[i], OUTPUT);
	for (i = 0; i < 4; i++)
	{
		pinMode(ROW[i], INPUT);
		pullUpDnControl(ROW[i], PUD_UP);
	}
    // begin setup
    pinMode(alertPin, OUTPUT);
    pinMode(rLatchPin, OUTPUT);
    pinMode(dLatchPin, OUTPUT);
    for (i = 0; i < NUM_LEDS; i++)
    {
      answer[i] = 'E';
      result[i] = LED_0;
    }
    for (i = 0; i < MAX_DIG; i++)
      used[i] = 0;
//    printMessage("HELLOQJOHNQPLAY", 15);
    create_passcode();
    shiftClear(rLatchPin, rDataPin, rClockPin, 2);
    shiftClear(dLatchPin, dDataPin, dClockPin, 1);
    printf("passcode: %s\n", passcode);
    // end setup
    g2r(150);
#if 0
    while (loopFlag == true)
    {
		for (i = 0; i < 4; i++)
        {
            digitalWrite(COL[i], LOW);
            for (j = 0; j < 4; j++)
            {
                if (digitalRead(ROW[j]) == LOW)
                {
                    key = MATRIX[j][i];
                    printf("%c\n", key);
                    if (key != '*' && key != '#')
                    {
                      shiftClear(dLatchPin, dDataPin, dClockPin, 1);
                      printSingleDigit(key, false);
                    }
                    if (key == '*' || key == '#')
                    {
                       shiftClear(dLatchPin, dDataPin, dClockPin, 1);
                      if (loseFlag == false && winFlag == false)
                      {
                         correct_index = correct_counter = 0;
                        for (i = 0; i < NUM_LEDS; i++)
                        {
                           result[i] = LED_0;
                           answer[i] = 'E';
                           changeBit(i, LED_0);
                           shiftClear(rLatchPin, rDataPin, rClockPin, 2);
                        }
                        if (key == '*')
                        {
                           shuffleFlag = true;
                           printNumLeft();
                           blip(2, 100);
                        }
                      }
                      if (key == '#')
                      {
                          //printMessage("RESET", 5);
                          shiftClear(rLatchPin, rDataPin, rClockPin, 2);
                          shiftClear(dLatchPin, dDataPin, dClockPin, 1);
                          num_tries = correct_index = correct_counter = 0;
                          blip(3, 100);
                          winFlag = loseFlag = false;
                          resetFlag = true;
                          for (i = 0; i < NUM_LEDS; i++)
                              result[i] = LED_0;
                          for (i = 0; i < MAX_DIG; i++)
                              used[i] = 0;
                          create_passcode();
                      }
                  }
                  // Special keys
                  if (key == 'A' || key == 'B' || key == 'C' || key == 'D')
                  {
                      correct_index = correct_counter = 0;
                      switch (key)
                      {
                          // display number of tries
                          case 'A':
                              printNumLeft();
                              break;
                          // display the password
                          case 'B':
                              //printMessage("PASS", 4);        delay(200);
                              printMultDigit(passcode, NUM_LEDS);
                              break;
                          case 'C':
                              showWin();
                              break;
                          // display demo
                          case 'D':
                              g2r(150);
                              shiftClear(rLatchPin, rDataPin, rClockPin, 2);
                              shiftClear(dLatchPin, dDataPin, dClockPin, 1);
                              break;
                      }
                  }
                  else
                  {
                      if (firstFlag == true)
                      {
                          firstFlag = false;
                          blip(2, 150);
                      }
                      if (winFlag == false && (key != '*' && key != '#'))
                      {
                          answer[correct_index++] = key;
                      }
                      // we've gotten all the digits we need
                      if (correct_index == NUM_LEDS && winFlag == false)
                      {
                          correct_index = 0;
                          // test for a correct code
                          for (i = 0; i < NUM_LEDS; i++)
                          {
                              if (passcode[i] == answer[i])
                              {
                                  correct_counter++;
                                  result[i] = LED_G;
                              }
                          }
                          if (correct_counter >= NUM_LEDS)
                              winFlag = true;
                          else
                          {
                              correct_counter = 0;
                              for (i = 0; i < NUM_LEDS; i++)
                              {
                                  if ((result[i] != LED_G) && used[(answer[i] - 48)] == 1)
                                      result[i] = LED_Y;
                                  if (result[i] == LED_0)
                                      result[i] = LED_R;
                              }
                              num_tries++;
                          }
                          // Test to see if you lost
                          if (num_tries > max_tries)
                          {
                              loseFlag = true;
                              winFlag = false;
                          }
                      }
                      // lost
                      if (loseFlag == true)
                      {
                          shiftClear(dLatchPin, dDataPin, dClockPin, 1);
                          if (loseSwapFlag == false)
                          {
                              data1 = 0b00100100;
                              data2 = 0b10010000;
                              digitalWrite(rLatchPin, 0);
                              shiftOut(rDataPin, rClockPin, data1);
                              shiftOut(rDataPin, rClockPin, data2);
                              digitalWrite(rLatchPin, 1);
                          }
                          else
                          {
                              data1 = 0b01001001;
                              data2 = 0b00100000;
                              digitalWrite(rLatchPin, 0);
                              shiftOut(rDataPin, rClockPin, data1);
                              shiftOut(rDataPin, rClockPin, data2);
                              digitalWrite(rLatchPin, 1);
                          }
                          if (loseCurrentMillis - losePreviousMillis > interval)
                          {
                              losePreviousMillis = loseCurrentMillis;
                              loseSwapFlag = (loseSwapFlag == false ? true : false);
                          }
                      }
                      // win
                      if (winFlag == true)
                      {
                          shiftClear(dLatchPin, dDataPin, dClockPin, 1);
                          if (winSwapFlag == false)
                          {
                              data1 = 0b10010010;
                              data2 = 0b01000000;
                              digitalWrite(rLatchPin, 0);
                              shiftOut(rDataPin, rClockPin, data1);
                              shiftOut(rDataPin, rClockPin, data2);
                              digitalWrite(rLatchPin, 1);
                          }
                          else
                          {
                              data1 = 0b01001001;
                              data2 = 0b00100000;
                              digitalWrite(rLatchPin, 0);
                              shiftOut(rDataPin, rClockPin, data1);
                              shiftOut(rDataPin, rClockPin, data2);
                              digitalWrite(rLatchPin, 1);
                          }
                          if (winCurrentMillis - winPreviousMillis > interval)
                          {
                              winPreviousMillis = winCurrentMillis;
                              winSwapFlag = (winSwapFlag == false ? true : false);
                          }
                      }
                      if (winFlag == false && loseFlag == false)
                      {
                          if (result[NUM_LEDS-1] != LED_0)
                          {
                              // Shuffle the result
                              if (shuffleFlag == true)
                              {
//                                      swap(&result[i], &result[ random(0, i) ]);
                                  for (i = NUM_LEDS - 1; i > 0; i--)
                                      swap(&result[i], &result[rand() % i]);
                                  for (i = 0; i < NUM_LEDS; i++)
                                      changeBit(i, result[i]);
                                  shuffleFlag = false;
                              }
                          }
                      }
                  }
                  while (digitalRead(ROW[j]) == LOW)
                    delay(20);
                }
            }
			digitalWrite(COL[i], HIGH);
        }
    }
#endif
    return 0;
}
