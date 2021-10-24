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
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

const int delayTime = 80;

const int AdataPin = 0; // blue
const int AlatchPin = 1; // green
const int AclockPin = 2; // yellow
const int Aaddr = 100;

const int BdataPin = 3; // blue
const int BlatchPin = 4; // green
const int BclockPin = 5; // yellow
const int Baddr = 200;

//int chipNum;
int Abits = 32;
int Bbits = 32;

// catch signals and clean up GPIO
static void die(int sig)
{
	int i;
	for (i = 0; i < Abits; i++) digitalWrite(Aaddr + i, 0);
	for (i = 0; i < Bbits; i++) digitalWrite(Baddr + i, 0);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int i, j;

	/*
	chipNum = 8;
	switch (chipNum)
	{
		case 1: case 2: case 3: case 4: Abits = chipNum * 8; Bbits = 0; break
		case 5: case 6: case 7: case 8: Abits = Bbits = (chipNum / 2) * 8; break;
	}
	*/
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	sr595Setup(Aaddr, Abits, AdataPin, AclockPin, AlatchPin);
	sr595Setup(Baddr, Bbits, BdataPin, BclockPin, BlatchPin);
	// turn everything off
	for (i = 0; i < Abits; i++) digitalWrite(Aaddr + i, 0);
	for (i = 0; i < Bbits; i++) digitalWrite(Baddr + i, 0);
	//printf("Raspberry Pi Shift Register\n");
	for (;;)
	{
		// blink
		for (j = 0; j < 3; j++)
		{
			for (i = 0; i < Abits; i++) digitalWrite(Aaddr + i, 1);
			for (i = 0; i < Bbits; i++) digitalWrite(Baddr + i, 1);
			delay(500);
			for (i = 0; i < Abits; i++) digitalWrite(Aaddr + i, 0);
			for (i = 0; i < Bbits; i++) digitalWrite(Baddr + i, 0);
			delay(500);
		}
		// go down the line
		for (i = 0; i < Abits; i++)
		{
			digitalWrite(Aaddr + i, 1);
			delay(delayTime);
			digitalWrite(Aaddr + i, 0);
		}
		// for some reason; the last Abit line isn't turned off.
		digitalWrite(Aaddr + Abits - 1, 0);
		for (i = 0; i < Bbits; i++)
		{
			digitalWrite(Baddr + i, 1);
			delay(delayTime);
			digitalWrite(Baddr + i, 0);
		}
		// now go back the other way
		for (i = Bbits; i != 0; i--)
		{
			digitalWrite(Baddr + i, 1);
			delay(delayTime);
			digitalWrite(Baddr + i, 0);
		}
		for (i = Abits; i != 0; i--)
		{
			digitalWrite(Aaddr + i, 1);
			delay(delayTime);
			digitalWrite(Aaddr + i, 0);
		}
		// blink every other one
		for (j = 0; j < 4; j++)
		{
			for (i = 0; i < Abits; i++)
			{
				if (i % 2 != 0) digitalWrite(Aaddr + i, 1);
				else digitalWrite(Aaddr + i, 0);
			}
			for (i = 0; i < Bbits; i++)
			{
				if (i % 2 != 0) digitalWrite(Baddr + i, 1);
				else digitalWrite(Baddr + i, 0);
			}
			delay(delayTime);
			for (i = 0; i < Abits; i++)
			{
				if (i % 2 == 0) digitalWrite(Aaddr + i, 1);
				else digitalWrite(Aaddr + i, 0);
			}
			for (i = 0; i < Bbits; i++)
			{
				if (i % 2 == 0) digitalWrite(Baddr + i, 1);
				else digitalWrite(Baddr + i, 0);
			}
			delay(delayTime);
		}
	}
	return EXIT_SUCCESS;
}

