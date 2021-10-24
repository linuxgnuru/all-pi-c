/*
   shift register pins:
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
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

const int dataPin  = 23; // blue (pin 14)
const int latchPin = 24; // green (pin 12)
const int clockPin = 25; // yellow (pin 11)
const int addr = 100;
int bits = 8;

static const int ledPins[8] = { 4, 6, 5, 0, 1, 2, 3, 7 };

//  5      24
// 0 6   25  23
//  1      26
// 2 4   28  21
//  3      22
//    7
static const int leds[16][8] = {
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

static void die(int sig)
{
	int i;
	for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
	if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

void usage(char *name)
{
	printf("usage: %s [digit]\n", name);
	printf("(to clear, use 11 for digit)\n\n");
	printf( "pins:\n"
		"   -   \t    24\n"
		" |   | \t  25  23\n"
		"   -   \t    26\n"
		" |   | \t  28  21\n"
		"   -   \t    22\n");
}

int main(int argc, char **argv)
{
	int h, i, j;
	int digit = 100;
	char c;
	int badFlag = 0;

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	if (argc > 1)
	{
		if (strcmp(argv[1], "#") == 0 || strcmp(argv[1], "*") == 0 ||
	       	    strcasecmp(argv[1], "A") == 0 || strcasecmp(argv[1], "B") == 0 ||
	       	    strcasecmp(argv[1], "C") == 0 || strcasecmp(argv[1], "D") == 0)
		{
			if (strcmp(argv[1], "#") == 0) digit = 14;
			else if (strcmp(argv[1], "*") == 0) digit = 15;
			else if (strcasecmp(argv[1], "A") == 0) digit = 10;
			else if (strcasecmp(argv[1], "B") == 0) digit = 11;
			else if (strcasecmp(argv[1], "C") == 0) digit = 12;
			else if (strcasecmp(argv[1], "D") == 0) digit = 13;
			if (digit == 100)
				badFlag = 1;
		}
		else
		{
			digit = atoi(argv[1]);
			if (digit > 10 && digit != 60)
				badFlag = 1;
		}
	}
	else
		badFlag = 1;
	/*
	printf("digit: %d\n", digit);
	for (i = 0; i < 7; i++) printf("%d ", leds[digit][i]);
	printf("\n");
	*/
	if (badFlag == 1)
	{
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	for (i = 0; i < 7; i++)
	{
		if (digit == 60)
			digitalWrite(addr + i, LOW);
		else
		{
	       		if (leds[digit][i] != 30)
			{
				digitalWrite(addr + leds[digit][i], HIGH);
			}
	       		if (leds[digit][7] == 40)
			{
				digitalWrite(addr + 7, HIGH);
			}
			else
			{
				digitalWrite(addr + 7, LOW);
			}
		}
	}
	return EXIT_SUCCESS;
}
