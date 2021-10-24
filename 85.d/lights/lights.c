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

//#include <num2pcf.h>

#define MY8574 100

int gpio_leds[] = { 21, 22, 23, 24, 25, 26, 28, 29, 6, 5 };

int main(int argc, char **argv)
{
	int delayn = 60;
	int swap, tmp_n;
	int pin_num;
	int i, j;
	int z;
	int devid = 0x20;

	wiringPiSetup();
	pcf8574Setup(MY8574, 0x20);
	for (i = 0; i < 10; i++)
	{
		pinMode(gpio_leds[i], OUTPUT);
		digitalWrite(gpio_leds[i], LOW);
	}
	for (i = 0; i < 8; i++)
		digitalWrite(MY8574 + i, 0);
	j = 0;
	swap = 1;
	//for (z = 0; z < 2; z++)
	while (1)
	{
		for (i = 0; i < 18; i++)
		{
			if (i < 10)
				digitalWrite(gpio_leds[i], (swap == 1 ? HIGH : LOW));
			else // pcf8574 pins
			{
				//j = (swap == 1 ? abs(i-16) - 1 : abs(i-7) - 1);
				j = abs(i-18) - 1;
				// digitalWrite(MY7574 + j, !digitalRead(MY8574 + j));
				digitalWrite(MY8574 + j, (swap == 1 ? 1 : 0));
				//printf("j: %d\n", j);
				//num2pcf(devid, j);
			}
			delay(delayn);
			//usleep(delay);
		}
		swap = (swap == 1 ? 0 : 1);
	} // end for (z ...)
	return(0);
}
