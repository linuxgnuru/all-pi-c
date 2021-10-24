#include <stdio.h>

#include <wiringPi.h>
#include <sr595.h>
//#include <wiringShift.h>

const int hourDataPin = 0;
const int hourLatchPin = 1;
const int hourClockPin = 2;
const int hourAddr = 100;

const int minuteDataPin = 3;
const int minuteLatchPin = 4;
const int minuteClockPin = 5;
const int minuteAddr = 200;

int main(int argc, char **argv)
{
	int i, j;
	int bits = 12;

	/*
	if (argc > 1)
	{
		bits = atoi(argv[1]);
	}
	*/
	wiringPiSetup();
	sr595Setup(hourAddr,   bits, hourDataPin,   hourClockPin,   hourLatchPin);
	sr595Setup(minuteAddr, bits, minuteDataPin, minuteClockPin, minuteLatchPin);
	printf("Raspberry Pi Shift Register\n");
	//for (j = 0; j < 5; j++)
	for (;;)
	{
		for (i = 0; i < bits; i++)
		{
			//printf("[%d]\n", i);
			digitalWrite(hourAddr + i, 1);
			digitalWrite(minuteAddr + i, 1);
			delay(200);
			digitalWrite(hourAddr + i, 0);
			digitalWrite(minuteAddr + i, 0);
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
