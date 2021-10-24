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
const int leds[16][8] = {
			{  4,  6,  5,  0, 30,  2,  3, 30 }, // 0
			{  4,  6, 30, 30, 30, 30, 30, 30 }, // 1
			{ 30,  6,  5, 30,  1,  2,  3, 30 }, // 2
			{  4,  6,  5, 30,  1, 30,  3, 30 }, // 3
			{  4,  6, 30,  0,  1, 30, 30, 30 }, // 4
			{  4, 30,  5,  0,  1, 30,  3, 30 }, // 5
			{  4, 30,  5,  0,  1,  2,  3, 30 }, // 6
			{  4,  6,  5, 30, 30, 30, 30, 30 }, // 7
			{  4,  6,  5,  0,  1,  2,  3, 30 }, // 8
			{  4,  6,  5,  0,  1, 30,  3, 30 }, // 9
			{  4,  6,  5,  0,  1,  2, 30, 40 }, // A
			{  4,  6,  5,  0,  1,  2,  3, 40 }, // B
			{ 30, 30,  5,  0, 30,  2,  3, 40 }, // C
			{  4,  6,  5,  0, 30,  2,  3, 40 }, // D
			{  4,  6, 30,  0, 30,  2, 30, 40 }, // #
			{ 30, 30,  5, 30,  1, 30,  3, 40 }  // *
		};
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

const int ROW[] = { 0, 1, 2, 3 };
const int COL[] = { 4, 5, 6, 7 };

const int okPin = 21;
const int noPin = 22;

const int dataPin  = 23; // blue (pin 14)
const int latchPin = 24; // green (pin 12)
const int clockPin = 25; // yellow (pin 11)
const int addr = 100;
int bits = 8;

static void die(int sig)
{
	int i;
	digitalWrite(okPin, LOW);
	digitalWrite(noPin, LOW);
	for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
	for (i = 0; i < 4; i++) digitalWrite(COL[i], LOW);
	if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

void pDigit(char *comp)
{
	int i, digit;
	// clear the display
	for (i = 0; i < 7; i++)
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
	//printf("comp: %s\tdigit: %d\n", comp, digit);
	for (i = 0; i < 7; i++)
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
	int i, j;
	char new_m[2];

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return 1;
	}
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	pinMode(okPin, OUTPUT);
	pinMode(noPin, OUTPUT);
	digitalWrite(okPin, LOW);
	digitalWrite(noPin, LOW);
	for (i = 0; i < 4; i++) pinMode(COL[i], OUTPUT);
	for (i = 0; i < 4; i++)
	{
		pinMode(ROW[i], INPUT);
		pullUpDnControl(ROW[i], PUD_UP);
	}
	// clear led
	for (i = 0; i < 7; i++) digitalWrite(addr + i, LOW);
	while (1)
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
				}
				while (digitalRead(ROW[j]) == LOW)
				       	delay(20);
			}
			digitalWrite(COL[i], HIGH);
		}
	}
	return 0;
}
