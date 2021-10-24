/*
	1 - 0-7 up
	2 - 7-0 down

 */
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
	int i, j, k;
	int gpio_leds[8];
	int delayn = 200;
	int swap_l2r;
	int swap;
	int ifc;
	int elsec;
	int gpio_status[8];

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
		pinMode(gpio_leds[i], OUTPUT); // set all gpio pins to output
		digitalWrite(gpio_leds[i], LOW); // set all leds to off
	}

/*
	swap = 1;
	//ifc = elsec = k = 0;
	//printf("k:\tifc:\telsec:\tswap_l2r:\n");
//	printf("high: %d\tlow: %d\n", HIGH, LOW);
	for (;;)
	{
		for (k = 0; k < 2; k++)
		{
			for (i = 0; i < 8; i++)
			{
				
				//digitalWrite(gpio_leds[i], k);
				//gpio_status[i] = k;
				if (swap == 1)
				{
					digitalWrite(gpio_leds[i], HIGH);
					gpio_status[i] = 1;
				}
				else
				{
					digitalWrite(gpio_leds[i], LOW);
					gpio_status[i] = 0;
				}
				delay(delayn);
			}
			swap = (swap == 1 ? 0 : 1);
		}
		//printf("%d\t%d\t%d\t%d\n", k++, ifc, elsec, swap_l2r);
	}
	*/
	return(0);
}
