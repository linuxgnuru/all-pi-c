#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>

#define MY_8574 100

int main(int argc, char **argv)
{
	int i;
	int loop;
	int delay_n = 200;
	int swap = 0;
	int swap_v;

	wiringPiSetup();
	pcf8574Setup(MY_8574, 0x20);
	for (i = 0; i < 8; i++)
	{
		digitalWrite(MY_8574 + i, 1);
	}
	for (loop = 0; loop < 2; loop++)
	{
		for (i = 0; i < 8; i++)
		{
			swap_v = (swap == 0 ? i : abs(i-7));
			digitalWrite(MY_8574 + swap_v, !digitalRead(MY_8574 + swap_v));
			delay(delay_n);
		}
		swap = (swap == 0 ? 1 : 0);
	}
	return 0;
}
