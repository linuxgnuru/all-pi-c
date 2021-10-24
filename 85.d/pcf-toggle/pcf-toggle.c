#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>

#define MY_8574 100

void pcf_toggle(int pin)
{
	digitalWrite(MY_8574 + pin, !digitalRead(MY_8574 + pin));
}

int main(int argc, char **argv)
{
	int pin;
	int err = 0;

	if (argc > 1)
	{
		pin = atoi(argv[1]);
		if (pin < 0 || pin > 7) err = 1;
	}
	else err = 1;
	if (err == 1)
	{
		printf("usage: %s PIN\nwhere PIN is a number from 0 - 7\n", argv[0]);
		return 0;
	}
	wiringPiSetup();
	pcf8574Setup(MY_8574, 0x20);
	pcf_toggle(pin);

	return 0;
}

/*
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
*/
