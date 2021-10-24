#include <stdio.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <wiringPi.h>

int main(int argc, char **argv)
{
	int i, j;
	int gpio_leds[8];

	gpio_leds[0] = 17;
	gpio_leds[1] = 18;
	gpio_leds[2] = 27;
	gpio_leds[3] = 22;
	gpio_leds[4] = 23;
	gpio_leds[5] = 24;
	gpio_leds[6] = 25;
	gpio_leds[7] = 4;

	wiringPiSetupGpio();
	for (i = 0; i < 8; i++)
	{
		pinMode(gpio_leds[i], OUTPUT);
		digitalWrite(gpio_leds[i], LOW);
	}
	return(0);
}
