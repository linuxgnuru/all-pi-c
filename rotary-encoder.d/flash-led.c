#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>

int pins[] = { 21, 22, 23, 24, 25, 26, 28, 29, 6, 5 };
int rpins[] = { 5, 6, 29, 28, 26, 25, 24, 23, 22, 21 };

int abs(int num)
{
	return abs(10 - num);
}

int main(int argc, char **argv)
{
	int i, j;
	int delay_n = 15;
	int swap;
	int cpins[10];
	int ctr;

	ctr = swap = 0;
	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	for (i = 0; i < 10; i++)
	{
		pinMode(pins[i], OUTPUT);
		digitalWrite(pins[i], LOW);
	}
	for (;;)
	{
		for (i = 0; i < 10; i++)
		{
			for (j = 0; j < 10; j++)
				cpins[j] = (swap == 0 ? pins[j] : rpins[j]);
			digitalWrite(cpins[i], !digitalRead(cpins[i]));
			delay(delay_n);
		}
		if (ctr % 2 == 0)
			swap = (swap == 0 ? 1 : 0);
		ctr++;
	}
	return 0;
}
/*
bcm2835_gpio_fsel(KEY[i], BCM2835_GPIO_FSEL_INPT);
bcm2835_gpio_set_pud(KEY[i], BCM2835_GPIO_PUD_UP);
//if (bcm2835_gpio_lev(KEY[i]) == 0)        
*/
