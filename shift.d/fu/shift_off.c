/*
   shift register pins:
	      +--------+
	2out -+  1  16 +- Vin
	3out -+  2  15 +- 1out
	4out -+  3  14 +- data (blue)
	5out -+  4  13 +- ground
	6out -+  5  12 +- latch (green + 1u cap if first)
	7out -+  6  11 +- clock (yellow)
	8out -+  7  10 +- Vin
	 gnd -+  8   9 +- serial out
	      +--------+
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include <wiringPi.h>
#include <sr595.h>
//#include <wiringShift.h>

const int addr = 100;

int dataPin  = 25;
int latchPin = 26;
int clockPin = 28;

int numBits = 16;

void usage(char *s)
{
		printf("usage: %s \n"
			"( [data pin (blue)] [latch pin (green)] [clock pin (yellow)] ) (optional)\n"
			"[number of bits (8 - 32)] \n"
			"default pins:\n"
			"\t data pin: %d\n"
			"\tlatch pin: %d\n"
			"\tclock pin: %d\n", s, dataPin, latchPin, clockPin);
}

static void die(int sig)
{
	int i;
	for (i = 0; i < numBits; i++)
	       	digitalWrite(i, LOW);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int i;

    /*
	if (argc > 1 && argc < 6)
    {
        if (argc > 4)
        {
			dataPin = atoi(argv[1]);
			latchPin = atoi(argv[2]);
			clockPin = atoi(argv[3]);
			numBits = atoi(argv[4]);
        }
        else
    		numBits = atoi(argv[1]);
    }
	else
	{
        usage(argv[0]);
        return EXIT_FAILURE;
	}
    */
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	(void)signal(SIGTERM, die);
	wiringPiSetup();
	sr595Setup(addr, numBits, dataPin, clockPin, latchPin);
	for (i = 0; i < numBits; i++)
	{
		digitalWrite(addr + i, 1);
        delay(250);
	}
	for (i = 0; i < numBits; i++)
	{
		digitalWrite(addr + i, 0);
        delay(250);
	}
	return EXIT_SUCCESS;
}
/*
    printf("argc: %d\n"
           " dataPin: %d\n"
           "latchPin: %d\n"
           "clockPin: %d\n",
           argc,
           dataPin,
           latchPin,
           clockPin);

	for (i = 0; i < 1024; ++i)
	{
		for (bit = 0; bit < 8; ++bit)
			digitalWrite(100 + bit, i & (i << bit));
		delay(200);
	}
	for (bit = 0; bit < 8; ++bit)
		digitalWrite(100 + bit, i & (i << bit));
	while (1)
	{
		for (i = 0; i < 1024; ++i)
		{
			for (bit = 0; bit < 8; ++bit)
				digitalWrite(100 + bit, i & (i << bit));
			delay(5);
		}
	}

void blinkAll_2Bytes(int n, int d)
{
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0);
  shiftOut(dataPin, clockPin, 0);
  digitalWrite(latchPin, 1);
  delay(200);
  for (int x = 0; x < n; x++) {
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 255);
    shiftOut(dataPin, clockPin, 255);
    digitalWrite(latchPin, 1);
    delay(d);
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 0);
    digitalWrite(latchPin, 1);
    delay(d);
  }
}
*/
