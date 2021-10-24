#include <stdio.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>

int devid = 0x20;
int fd;

int rev(int val) { return abs(val - 255); }

int main(int argc, char **argv)
{
	int max = 0x200;
	int x, i, j, k, l, m;
	int swap, tmp_n;
	int pscale;
	int nums[9];
	int gpio_leds[8];
	int rev_nums[9];
	int delayn = 300;

	gpio_leds[0] = 17; gpio_leds[1] = 18; gpio_leds[2] = 27; gpio_leds[3] = 22;
	gpio_leds[4] = 23; gpio_leds[5] = 24; gpio_leds[6] = 25; gpio_leds[7] = 4;

	j = max;
	for (i = 9; i >= 0; i--)
	{
		j /= 2;
		k = j - 1;
		nums[i] = k;
	}
	l = 256;
	k = 1;
	for (i = 0; i <= 8; i++)
	{
		m = l - k;
		k *= 2;
		rev_nums[i] = m;
	}
	wiringPiSetupGpio();
	for (i = 0; i < 8; i++)
		pinMode(gpio_leds[i], OUTPUT);
	if ((fd=wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror (errno));
	    return 1;
	}
	j = 0;
	x = 1;
	swap = 1;
	while (x == 1)
	{
		//usleep(delay);
		for (i = 0; i < 16; i++)
		{
			/*
			  NOTE: TODO
			  	work on the swaping mechanism.
			j = i;
			if (swap == 1)
				j = abs(i - 16);
			//printf("i: %d\n", i);
			// Normal GPIO pins
			if (j < 8)
			*/
			if (i < 8)
			{
				//printf("gpio_leds[i]: %d\n", gpio_leds[i]);
				if (swap == 1) digitalWrite(gpio_leds[i], HIGH);
				else digitalWrite(gpio_leds[i], LOW);
			}
			else // pcf8574 pins
			{
				pscale = abs(i - 16);
				printf("swap: %d\t", swap);
				printf("pscale: %d\t", pscale);
				tmp_n = (swap == 1 ? nums[pscale] : rev_nums[pscale]);
				printf("tmp_n: %d\n", tmp_n);
				if ((wiringPiI2CWrite(fd,tmp_n)) < 0)
				{
				    fprintf (stderr, "write badly %s\n", strerror (errno));
				    return 1;
				}
			}
			delay(delayn);
			//usleep(delay);
		}
		swap = (swap == 1 ? 0 : 1);
		if (j++ == 1) x = 2;
	}
	if ((wiringPiI2CWrite(fd,255)) < 0)
	{
	    fprintf (stderr, "write badly %s\n", strerror (errno));
	    return 1;
	}
	return(0);
}
