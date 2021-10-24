#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>

int main(int argc, char **argv)
{
	int led_pin;

	if (argc > 1)
	{
		led_pin = atoi(argv[1]);
	}
	else
	{
	    printf("usage: %s [pin num]\n", argv[0]);
	    return 1;
	}
	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	pinMode(led_pin, OUTPUT);
	digitalWrite(led_pin, (digitalRead(led_pin) == 0 ? HIGH : LOW));
	return 0;
}
/*
bcm2835_gpio_fsel(KEY[i], BCM2835_GPIO_FSEL_INPT);
bcm2835_gpio_set_pud(KEY[i], BCM2835_GPIO_PUD_UP);
//if (bcm2835_gpio_lev(KEY[i]) == 0)        
*/
