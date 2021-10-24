#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <wiringPi.h>
#include <sr595.h>

int init_register(int baseAddr, int bits, int dataPin, int latchPin, int clockPin)
{
    int status = 0; // for return if anything fails

	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
        status = -1;
	}
	sr595Setup(baseAddr, bits, dataPin, clockPin, latchPin);
    return status;
}
