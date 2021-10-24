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
#include <errno.h>

#include <wiringPi.h>
#include <sr595.h>
//#include <wiringShift.h>

const int dataPin  = 23; // blue (pin 14)
const int latchPin = 24; // green (pin 12)
const int clockPin = 25; // yellow (pin 11)
const int addr = 100;
int bits = 8;

static void die(int sig)
{
	int i;
	for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
	if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int i, j;

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	/*
	if (argc > 1)
	{
		bits = atoi(argv[1]);
	}
	*/
	wiringPiSetup();
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	printf("Raspberry Pi Shift Register\n");
	for (i = 0; i < bits; i++)
	{
		printf("[%d]\n", i);
		digitalWrite(addr + i, 1);
		delay(1000);
		digitalWrite(addr + i, 0);
	}
	return 0;
}
