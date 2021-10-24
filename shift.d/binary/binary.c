#include <stdio.h>

#include <wiringPi.h>
#include <sr595.h>

const int dataPin  = 4; // 0 blue
const int latchPin = 5; // 1 green
const int clockPin = 6; // 2 yellow
const int numBits  = 16;
const int addr     = 100;

int main(int argc, char **argv)
{
	int i, bit;

	wiringPiSetup();
	sr595Setup(addr, numBits, dataPin, clockPin, latchPin);
	//printf("Raspberry Pi - Shift Register Test\n");
	digitalWrite(addr + 5, 0 & (0 << 5));
	/*
	for (i = 0; i < 65536; ++i)
	{
		for (bit = 0; bit < numBits; ++bit)
			digitalWrite(addr + bit, i & (i << bit));
		delay(5);
	}
	*/
	return 0;
}

/*
 * from Arduino:
 */

/*
void blinkAll_2Bytes(int n, int d)
{
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0);
  shiftOut(dataPin, clockPin, 0);
  digitalWrite(latchPin, 1);
  delay(200);
  for (int x = 0; x < n; x++)
  {
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
