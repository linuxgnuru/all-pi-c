#include <stdio.h>

#include <wiringPi.h>
#include <sr595.h>
//#include <wiringShift.h>

const int dataPin = 0;
const int latchPin = 1;
const int clockPin = 2;
const int addr = 100;

int main(int argc, char **argv)
{
	int i, j;
	int bits = 24;

	/*
	if (argc > 1)
	{
		bits = atoi(argv[1]);
	}
	*/
	wiringPiSetup();
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	printf("Raspberry Pi Shift Register\n");
	//for (j = 0; j < 5; j++)
	for (;;)
	{
		for (i = 0; i < bits; i++)
		{
			//printf("[%d]\n", i);
			digitalWrite(addr + i, 1);
			//digitalWrite(addr + i + 1, 1);
			delay(200);
			digitalWrite(addr + i, 0);
			//digitalWrite(addr + i + 1, 0);
		}
	}
/*
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
*/
	return 0;
}

/*
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
