#include <stdio.h>

#include <wiringPi.h>
#include <sr595.h>
//#include <wiringShift.h>

const int dataPin = 0;
const int latchPin = 1;
const int clockPin = 2;
const int addr = 100;
const int bits = 12;

int main(int argc, char **argv)
{
	int i, j;

	wiringPiSetup();
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	for (i = 0; i < bits; i++)
	{
		digitalWrite(addr + i, 0);
	}
	return 0;
}

