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
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

const int delayTime = 80;

const int dataPin = 23; // blue
const int latchPin = 22; // green
const int clockPin = 21; // yellow
const int addr = 100;

int bits = 16;

// catch signals and clean up GPIO
static void die(int sig)
{
	int i;
	for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int i, j;

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	// turn everything off
	for (i = 0; i < bits; i++) digitalWrite(addr + i, LOW);
	//printf("Raspberry Pi Shift Register\n");
	for (;;)
	{
		// blink them all
		for (j = 0; j < 4; j++)
		{
			for (i = 0; i < bits; i++) digitalWrite(addr + i, HIGH);
			delay(500);
			for (i = 0; i < bits; i++) digitalWrite(addr + i, LOW);
			delay(500);
		}
		// blink every other one
		for (j = 0; j < 4; j++)
		{
			for (i = 0; i < bits; i++)
			{
				if (i % 2 != 0) digitalWrite(addr + i, HIGH);
				else digitalWrite(addr + i, LOW);
			}
			delay(500);
			for (i = 0; i < bits; i++)
			{
				if (i % 2 == 0) digitalWrite(addr + i, 1);
				else digitalWrite(addr + i, 0);
			}
			delay(500);
		}
	}
	return EXIT_SUCCESS;
}

