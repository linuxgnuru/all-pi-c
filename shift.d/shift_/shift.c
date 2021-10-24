#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

int dataPin  = 0; // blue
int latchPin = 1; // green
int clockPin = 2; // yellow

int addr = 100;

int bits = 32;

// catch signals and clean up GPIO
static void die(int sig)
{
	int i;
	for (i = 0; i < bits; i++)
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

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	(void)signal(SIGTERM, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	//printf("Raspberry Pi Shift Register\n");
	//printf("base address: %d\nbits: %d\n", addr, bits);
    for (i = 0; i < 32; i++)
    {
        digitalWrite(addr + i, HIGH);
        delay(100);
        digitalWrite(addr + i, LOW);
        delay(100);
    }
	return EXIT_SUCCESS;
}
