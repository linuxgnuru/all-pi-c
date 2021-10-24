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
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <sr595.h>

/*
 *   -    5
 *  | |  0 6
 *   -    1
 *  | |  2 4
 *   -    3
 */

const int leds[22][8] = {
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
			{  0,  1,  2,  3, 30,  5, 30, 40 }, // E. (error)
			{  0,  1,  2, 30, 30,  5, 30, 40 }, // F.
			{  0,  1,  2,  3,  4,  5, 30, 40 }, // G.
			{  0,  1,  2, 30, 30,  5,  6, 40 }, // P.
			{  0,  1, 30,  3,  4,  5, 30, 40 }, // S.
			{  0, 30,  2,  3,  4, 30,  6, 40 }  // U.
		};

const int dataPin  = 23; // blue (pin 14)
const int latchPin = 24; // green (pin 12)
const int clockPin = 25; // yellow (pin 11)
const int addr = 100;
int bits = 8;
int Clear = 0;

static void die(int sig)
{
	int i;
	for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
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
	if (strcmp(comp, "#") == 0 || strcmp(comp, "*") == 0 ||
	       	strcasecmp(comp, "A") == 0 || strcasecmp(comp, "B") == 0 || strcasecmp(comp, "C") == 0 ||
	       	strcasecmp(comp, "D") == 0 || strcasecmp(comp, "E") == 0 || strcasecmp(comp, "F") == 0 ||
	       	strcasecmp(comp, "G") == 0 || strcasecmp(comp, "P") == 0 || strcasecmp(comp, "S") == 0 ||
	       	strcasecmp(comp, "U") == 0)
	{
		if (strcmp(comp, "#") == 0) digit = 14;
		else if (strcmp(comp, "*") == 0) digit = 15;
		else if (strcasecmp(comp, "A") == 0) digit = 10;
		else if (strcasecmp(comp, "B") == 0) digit = 11;
		else if (strcasecmp(comp, "C") == 0) digit = 12;
		else if (strcasecmp(comp, "D") == 0) digit = 13;
		else if (strcasecmp(comp, "E") == 0) digit = 16;
		else if (strcasecmp(comp, "F") == 0) digit = 17;
		else if (strcasecmp(comp, "G") == 0) digit = 18;
		else if (strcasecmp(comp, "P") == 0) digit = 19;
		else if (strcasecmp(comp, "S") == 0) digit = 20;
		else if (strcasecmp(comp, "U") == 0) digit = 21;
	}
	else
		digit = atoi(comp);
	for (i = 0; i < 7; i++)
	{
		if (Clear == 60) digitalWrite(addr + i, LOW);
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
	int i;
	char td[2];

	if (argc > 1)
	{
		if (strcmp(argv[1], "60") == 0)
			Clear = 60;
		strncpy(td, argv[1], 1);
		td[1] = '\0';
	}
	else
	{
		printf("usage: %s num/letter (0-9, (a-g, p, s, u), *, or #)\n", argv[0]);
		printf("60 clears the LED\n");
		return EXIT_FAILURE;
	}
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return 1;
	}
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	// clear led
	for (i = 0; i < 7; i++) digitalWrite(addr + i, LOW);
	pDigit(td);
	return 0;
}
