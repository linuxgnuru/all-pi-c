#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <wiringShift.h>

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

_Bool pm = FALSE;

const int ord = MSBFIRST;

const int dataPin = 0; // blue
const int latchPin = 1; // green
const int clockPin = 2; // yellow

char data[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000 };

const char digitArray[] = {
  //  BCDGE.FA
  0b01111011, // 0
  0b00001010, // 1
  0b11110010, // 2
  0b10111010, // 3
  0b10001011, // 4
  0b10111001, // 5
  0b11111001, // 6
  0b00011010, // 7
  0b11111011, // 8
  0b10111011  // 9
};

// catch signals and clean up GPIO
static void die(int sig)
{
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	//endwin();
	exit(0);
}

#if 0
void ShiftOut(int myDataPin, int myClockPin, char myDataOut)
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
#endif

void printDigit(int pos, int num, _Bool dp)
{
  int ctr = 0;
  char myData;

  if (pos < 4 && (num > -1 && num < 10 || num == 99))
  {
    myData = (num == 99 ? 0b00000000 : digitArray[num]);
    if (dp == TRUE)
        myData |= 1<<2;
    data[pos] = myData;
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, ord, data[ctr++]);
    shiftOut(dataPin, clockPin, ord, data[ctr++]);
    shiftOut(dataPin, clockPin, ord, data[ctr++]);
    shiftOut(dataPin, clockPin, ord, data[ctr++]);
    digitalWrite(latchPin, 1);
  }
}

void clearD(int pos)
{
  int ctr = 0;
  
  data[pos] = 0b00000000;
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, ord, data[ctr++]);
  shiftOut(dataPin, clockPin, ord, data[ctr++]);
  shiftOut(dataPin, clockPin, ord, data[ctr++]);
  shiftOut(dataPin, clockPin, ord, data[ctr++]);
  digitalWrite(latchPin, 1);
}

int main(int argc, char **argv)
{
	struct tm *t;
	time_t tim;
    int h, m;
    int i;

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	(void)signal(SIGTERM, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
    pinMode(latchPin, OUTPUT);
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, ord, 0b00000000);
    shiftOut(dataPin, clockPin, ord, 0b00000000);
    shiftOut(dataPin, clockPin, ord, 0b00000000);
    shiftOut(dataPin, clockPin, ord, 0b10001011);
    digitalWrite(latchPin, 1);
    printDigit(0, 3, FALSE);
#if 0
    while (1)
    {
		tim = time(NULL);
		t = localtime(&tim);
		//sprintf(buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
        h = t->tm_hour;
        m = t->tm_min;
        if (h >= 12)
            pm = TRUE;
        if (h > 12)
            h -= 12;
        printDigit(0, h / 10, FALSE);
        printDigit(1, h % 10, TRUE);
        printDigit(2, m / 10, FALSE);
        printDigit(3, m % 10, (pm == TRUE ? TRUE : FALSE));
    }
#endif
    return EXIT_SUCCESS;
}
