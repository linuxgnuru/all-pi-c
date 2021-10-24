/*
   Pins:
   shift register:
	      +--------+
	2out -+  1  16 +- Vin
	3out -+  2  15 +- 1out
	4out -+  3  14 +- data (blue)
	5out -+  4  13 +- ground
	6out -+  5  12 +- latch (green + 1u cap if first)
	7out -+  6  11 +- lock (yellow)
	8out -+  7  10 +- Vin
	 gnd -+  8   9 +- serial out
	      +--------+
*/
// pass.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include <wiringPi.h>
#include <sr595.h>

/*
 * USER DEFINABLE STUFF
 *
 * You can change the following to your tastes.
 */

/*
 *   -    5
 *  | |  0 6
 *   -    1
 *  | |  2 4
 *   -    3
 */

const int leds[17][8] = {
			{  0, 30,  2,  3,  4,  5,  6, 30 }, // 0
			{ 30, 30, 30, 30,  4, 30,  6, 30 }, // 1
			{ 30,  1,  2,  3, 30,  5,  6, 30 }, // 2
			{ 30,  1, 30,  3,  4,  5,  6, 30 }, // 3
			{  0,  1, 30, 30,  4, 30,  6, 30 }, // 4
			{  0,  1, 30,  3,  4,  5, 30, 30 }, // 5
			{  0,  1,  2,  3,  4,  5, 30, 30 }, // 6
			{ 30, 30, 30, 30,  4,  5,  6, 30 }, // 7
			{  0,  1,  2,  3,  4,  5,  6, 30 }, // 8
			{  0,  1, 30,  3,  4,  5,  6, 30 }, // 9
			{  0,  1,  2, 30,  4,  5,  6, 40 }, // A.
			{  0,  1,  2,  3,  4,  5,  6, 40 }, // B. (8)
			{  0, 30,  2,  3, 30,  5, 30, 40 }, // C.
			{  0, 30,  2,  3,  4,  5,  6, 40 }, // D. (0) 
			{  0,  1,  2, 30,  4, 30,  6, 40 }, // #.
			{ 30,  1, 30,  3, 30,  5, 30, 40 }, // *.
			{  0,  1,  2,  3, 30,  5, 30, 40 }  // E. (error)
		};

const char MATRIX[4][4] = {
			{ '1','2','3','A' },
			{ '4','5','6','B' },
			{ '7','8','9','C' },
			{ '*','0','#','D' }
	       	};

// Pins for keypad
const int ROW[] = { 0, 1, 2, 3 };
const int COL[] = { 4, 5, 6, 7 };

// Pins for LEDs
const int okPin = 21;
const int noPin = 22;

// Pins for shift register
const int dataPin  = 23; // blue (pin 14)
const int latchPin = 24; // green (pin 12)
const int clockPin = 25; // yellow (pin 11)

// base address to use (i.e. 100 = pin 0, 102 = pin 2, etc)
const int addr = 100;

// Password
const char pass[5] = { '1', '7', '0', '1', 'D' };

// Secret password to get more lives.
const char clearBad[3] = { '#', '*', '#' };

const int MAX_TRIES = 8;

static void die(int sig)
{
	int i;
	digitalWrite(okPin, LOW);
	digitalWrite(noPin, LOW);
	for (i = 0; i < 8; i++)
	       	digitalWrite(addr + i, 0);
	for (i = 0; i < 4; i++)
	       	digitalWrite(COL[i], LOW);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

void cls()
{
	int i;
	for (i = 0; i < 8; i++)
	       	digitalWrite(addr + i, 0);
       	printf("\033[2J\033[H");
}

void printIn(int nt)
{
	printf("Enter 5 key passcode:\n(%d %s left)\n", MAX_TRIES-nt, (MAX_TRIES-nt == 1 ? "try" : "tries"));
	printf("(Enter '#' or '*' to reset)\n");
}

void usage(char *arg)
{
	printf("%s: (no args)\n"
		"Pin out: (from left to right on 4x4 keypad)\n"
		"GPIO\tBCM\n"
		"0\t17\n1\t18\n2\t27\n3\t22\n"
		"4\t23\n5\t24\n6\t25\n7\t4\n"
		"WARNING: The first 4 are all using the internal pull-up resistors\n"
		"         so keep this in mind or use your own resistors to be safe.\n"
		, arg);
	exit(0);
}

void pDigit(char *comp)
{
	int i, digit;
	// clear the display
	for (i = 0; i < 8; i++)
	       	digitalWrite(addr + i, LOW);
	if (strcmp(comp, "#") == 0 || strcmp(comp, "*") == 0 || strcasecmp(comp, "A") == 0 || strcasecmp(comp, "B") == 0 || strcasecmp(comp, "C") == 0 || strcasecmp(comp, "D") == 0 || strcasecmp(comp, "E") == 0)
	{
		if (strcmp(comp, "#") == 0) digit = 14;
		else if (strcmp(comp, "*") == 0) digit = 15;
		else if (strcasecmp(comp, "A") == 0) digit = 10;
		else if (strcasecmp(comp, "B") == 0) digit = 11;
		else if (strcasecmp(comp, "C") == 0) digit = 12;
		else if (strcasecmp(comp, "D") == 0) digit = 13;
		else if (strcasecmp(comp, "E") == 0) digit = 16;
	}
	else
	       	digit = atoi(comp);
	for (i = 0; i < 8; i++)
	{
		if (digit == 60) digitalWrite(addr + i, LOW);
		else
		{
	       		if (leds[digit][i] != 30) digitalWrite(addr + leds[digit][i], HIGH);
	       		if (leds[digit][7] == 40) digitalWrite(addr + 7, HIGH);
			else digitalWrite(addr + 7, LOW);
		}
	}
}

int main(int argc, char **argv)
{
	char new_m[2];
	int i, j;
	int pNum = 0;
	int cNum = 0;
	int loopFlag = 1;
	int numTries = 0;
	int dieFlag = 0;

	if (argc > 1)
	       	usage(argv[0]);
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	sr595Setup(addr, 8, dataPin, clockPin, latchPin);
	pinMode(okPin, OUTPUT);
	pinMode(noPin, OUTPUT);
	digitalWrite(okPin, LOW);
	digitalWrite(noPin, LOW);
	for (i = 0; i < 4; i++)
	       	pinMode(COL[i], OUTPUT);
	for (i = 0; i < 4; i++)
	{
		pinMode(ROW[i], INPUT);
		pullUpDnControl(ROW[i], PUD_UP);
	}
	// clear led
	for (i = 0; i < 8; i++)
	       	digitalWrite(addr + i, LOW);
	cls();
	printIn(numTries);
	while (loopFlag)
	{
		for (i = 0; i < 4; i++)
		{
			digitalWrite(COL[i], LOW);
			for (j = 0; j < 4; j++)
			{
				if (digitalRead(ROW[j]) == LOW)
				{
					new_m[0] = MATRIX[j][i];
					new_m[1] = '\0';
					pDigit(new_m);
					// secret reset code. (shhh...)
				       	if (MATRIX[j][i] == clearBad[cNum])
					       	cNum++;
					else
					       	cNum = 0;
					// reset number of tries.
					if (cNum == 3)
						cNum = numTries = 0;
					if (MATRIX[j][i] == '*' || MATRIX[j][i] == '#')
					{
						pNum = 0;
						digitalWrite(okPin, LOW);
						digitalWrite(noPin, LOW);
						cls();
						printIn(numTries);
					}
					else
					{
					       	if (MATRIX[j][i] == pass[pNum])
						       	pNum++;
						else
						{
							printf("\nInvalid code.\n");
							printf("Enter '#' or '*' to reset\n");
							pNum = 0;
							digitalWrite(noPin, HIGH);
							numTries++;
						}
						if (numTries > MAX_TRIES)
						{
							dieFlag = 1;
							loopFlag = 0;
						}
					}
					if (pNum >= 5 && digitalRead(noPin) != HIGH)
					       	loopFlag = 0;
					while (digitalRead(ROW[j]) == LOW)
					       	delay(20);
					//printf("numTries: %d\n", numTries);
				}
			}
			digitalWrite(COL[i], HIGH);
		}
	}
	if (dieFlag == 1)
	{
		cls();
		printf("Too many mistakes.\n");
		while (1)
		{
			for (i = 0; i < 8; i++) digitalWrite(addr + i, LOW);
			digitalWrite(noPin, HIGH);
			//digitalWrite(okPin, LOW);
			delay(250);
			new_m[0] = 'E';
			new_m[1] = '\0';
			pDigit(new_m);
			digitalWrite(noPin, LOW);
			//digitalWrite(okPin, HIGH);
			delay(250);
		}
	}
	else
	{
		printf("\nSuccess\n");
		digitalWrite(okPin, HIGH);
		delay(2000);
		digitalWrite(okPin, LOW);
		for (i = 0; i < 8; i++) digitalWrite(addr + i, LOW);
	}
	return EXIT_SUCCESS;
}
