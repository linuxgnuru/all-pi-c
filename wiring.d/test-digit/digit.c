#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>


int main(int argc, char **argv)
{
	int num;

	if (argc > 1)
	{
		num = atoi(argv[1]);
	}
	else
	{
	    printf("usage: %s [num]\n", argv[0]);
	    return 1;
	}
	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	for (i = 21; i < 29; i++)
		pinMode(i, OUTPUT);
	for (i = 0; i < 10; i++)
		digit(i);
	/*
	while (1)
	{
		
	}
	*/
	return 0;
}

void digit(int num)
{
	int i;

	switch (num)
	{
		case 0:
			for (i = 21; i < 24; i++)
				digitalWrite(i, HIGH);
			digitalWrite(25, HIGH);
			digitalWrite(27, HIGH);
			digitalWrite(28, HIGH);
			break;
		case 1:
			digitalWrite(21, HIGH);
			digitalWrite(23, HIGH);
			break;
		case 2:
			digitalWrite(21, HIGH);
			break;
		case 3:
			digitalWrite(21, HIGH);
			break;
		case 4:
			digitalWrite(21, HIGH);
			break;
		case 5:
			digitalWrite(21, HIGH);
			break;
		case 6:
			digitalWrite(21, HIGH);
			break;
		case 7:
			digitalWrite(21, HIGH);
			break;
		case 8:
			for (i = 21; i < 29; i++)
				digitalWrite(i, HIGH);
			break;
		case 9:
			digitalWrite(21, HIGH);
			break;
		default:
			break;
	}
}

/*
bcm2835_gpio_fsel(KEY[i], BCM2835_GPIO_FSEL_INPT);
bcm2835_gpio_set_pud(KEY[i], BCM2835_GPIO_PUD_UP);
//if (bcm2835_gpio_lev(KEY[i]) == 0)        
*/
