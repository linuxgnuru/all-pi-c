#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

int dataPin  = 0; // blue
int latchPin = 1; // green
int clockPin = 2; // yellow

const int baseAddr = 100;

int numBits = 32;
//const int numBits  = 8;

// catch signals and clean up GPIO
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

void usage(char *s)
{
		printf("usage: %s \n"
			"( [data pin (blue)] [latch pin (green)] [clock pin (yellow)] ) (optional)\n"
			"[number of bits (8 - 32)] \n"
			"[toggle pin (0 - bit number)] \n"
			"[1 / 0 (on/off)] \n"
			"default pins:\n"
			"\t data pin: %d\n"
			"\tlatch pin: %d\n"
			"\tclock pin: %d\n", s, dataPin, latchPin, clockPin);
}

int main(int argc, char **argv)
{
	int i;
	int togglePin;
	int h_l;
	int numBits = 8;

	if (argc > 3 && argc < 8)
	{
		if (argc > 6)
		{
			dataPin = atoi(argv[1]);
			latchPin = atoi(argv[2]);
			clockPin = atoi(argv[3]);
			numBits = atoi(argv[4]);
			togglePin = atoi(argv[5]);
			h_l = atoi(argv[6]);
		}
		else
		{
			numBits = atoi(argv[1]);
			togglePin = atoi(argv[2]);
			h_l = atoi(argv[3]);
		}
	}
	else
	{
		printf("Not enough or too many arguments.\n");
		usage(argv[0]);
		return 1;
	}
	if (h_l != 1 && h_l != 0)
	{
		printf("Invalid status.  Status must either be 0 or 1\n");
		usage(argv[0]);
		return EXIT_FAILURE;
	}
    if (togglePin > numBits-1)
    {
		printf("Invalid element\n");
		usage(argv[0]);
		return EXIT_FAILURE;
    }
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	sr595Setup(baseAddr, numBits, dataPin, clockPin, latchPin);
	//printf("Raspberry Pi Shift Register\n");
	//printf("base address: %d\nbits: %d\n", baseAddr, numBits);
	digitalWrite(baseAddr + togglePin, (h_l == 0 ? 0 : 1));
	return EXIT_SUCCESS;
}
