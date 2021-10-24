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

#include <num2pcf.h>

int main(int argc, char **argv)
{
	int gpio_leds[8];
	int delayn = 300;
	int swap, tmp_n;
	int pin_num;
	int i, j, z;
	int devid = 0x20;

	gpio_leds[0] = 17; gpio_leds[1] = 18; gpio_leds[2] = 27; gpio_leds[3] = 22;
	gpio_leds[4] = 23; gpio_leds[5] = 24; gpio_leds[6] = 25; gpio_leds[7] = 4;

	wiringPiSetupGpio();
	for (i = 0; i < 8; i++)
	{
		pinMode(gpio_leds[i], OUTPUT);
		digitalWrite(gpio_leds[i], LOW);
	}
	j = 0;
	swap = 1;
	//while (1)
	for (z = 0; z < 2; z++)
	{
		for (i = 0; i < 16; i++)
		{
			if (i < 8)
			{
				if (swap == 1) digitalWrite(gpio_leds[i], HIGH);
				else digitalWrite(gpio_leds[i], LOW);
			}
			else // pcf8574 pins
			{
				//j = (swap == 1 ? abs(i-16) - 1 : abs(i-7) - 1);
				j = abs(i-16) - 1;
				num2pcf(devid, j);
			}
			delay(delayn);
			//usleep(delay);
		}
		swap = (swap == 1 ? 0 : 1);
	} // end while (1)
	return(0);
}
