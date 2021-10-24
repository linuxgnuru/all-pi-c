// avoid.c
//
// Example program for bcm2835 library
// Tries to tell if something is in the way or not
//
// After installing bcm2835, you can build this
// with something like:
// make or gcc -o avoid avoid.c -lbcm2835
// sudo ./avoid
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

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KEY0 RPI_GPIO_P1_11
#define KEY1 RPI_GPIO_P1_12
#define KEY2 RPI_V2_GPIO_P1_13
#define KEY3 RPI_GPIO_P1_15
#define KEY4 RPI_GPIO_P1_16
#define KEY5 RPI_GPIO_P1_18
#define KEY6 RPI_GPIO_P1_22
#define KEY7 RPI_GPIO_P1_07

int main(int argc, char **argv)
{
	uint8_t value,i;

	if (!bcm2835_init())
		return 1;
	bcm2835_gpio_fsel(KEY5, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(KEY5, BCM2835_GPIO_PUD_UP);
	while (1)
	{
		if (bcm2835_gpio_lev(KEY2) != 0)
		{
			printf("Object detected\n");
			delay(500);
		}
	}
	bcm2835_close();
	return 0;
}
