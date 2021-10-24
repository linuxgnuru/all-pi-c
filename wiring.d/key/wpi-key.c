// input.c
//
// Example program for bcm2835 library
// Blinks a pin on an off every 0.5 secs
//
// After installing bcm2835, you can build this
// with something like:
// make or gcc -o led led.c -lbcm2835
// sudo ./led
/*
             define from bcm2835.h                       define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
    RPI_V2_GPIO_P1_03 | | 5V               ->                  SDA | | 5V 
    RPI_V2_GPIO_P1_05 | | GND              ->                  SCL | | GND
       RPI_GPIO_P1_07 | | RPI_GPIO_P1_08   ->                  IO7 | | TX
                  GND | | RPI_GPIO_P1_10   ->                  GND | | RX
       RPI_GPIO_P1_11 | | RPI_GPIO_P1_12   ->                  IO0 | | IO1
    RPI_V2_GPIO_P1_13 | | GND              ->                  IO2 | | GND
       RPI_GPIO_P1_15 | | RPI_GPIO_P1_16   ->                  IO3 | | IO4
                  VCC | | RPI_GPIO_P1_18   ->                  VCC | | IO5
       RPI_GPIO_P1_19 | | GND              ->                 MOSI | | GND
       RPI_GPIO_P1_21 | | RPI_GPIO_P1_22   ->                 MISO | | IO6
       RPI_GPIO_P1_23 | | RPI_GPIO_P1_24   ->                  SCK | | CE0
                  GND | | RPI_GPIO_P1_26   ->                  GND | | CE1

::if your raspberry Pi is version 1 or rev 1 or rev A
RPI_V2_GPIO_P1_03->RPI_GPIO_P1_03
RPI_V2_GPIO_P1_05->RPI_GPIO_P1_05
RPI_V2_GPIO_P1_13->RPI_GPIO_P1_13
::
*/
#include <stdio.h>
#include <errno.h>

#include <wiringPi.h>

#define KEY0 0
#define KEY1 1
#define KEY2 2
//#define KEY3 3
//#define KEY4 4
#define KEY5 5
//#define KEY6 6
#define KEY7 7

//char KEY[] = { KEY0, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7 };
int KEY[] = { KEY0, KEY1, KEY2, KEY5, KEY7 };

int main(int argc, char **argv)
{
	int i;

	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	for (i = 0; i < 5; i++)
	{
		pinMode(KEY[i], INPUT);
		pullUpDnControl(KEY[i], PUD_UP);
		/*
		bcm2835_gpio_fsel(KEY[i], BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_set_pud(KEY[i], BCM2835_GPIO_PUD_UP);
		*/
	}
	while (1)
	{
	        for (i = 0; i < 5; i++)
		{
			//if (bcm2835_gpio_lev(KEY[i]) == 0)        
			if (digitalRead(KEY[i]) == 0)
			{
				printf("key pressed: %d\n", KEY[i]);
				delay(500);
			}
		}
    	}
	return 0;
}
