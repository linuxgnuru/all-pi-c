#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//#include <wiringPi.h>
#include <lcd.h>

#define PIN RPI_GPIO_P1_12

const int RS = 3;	// 
const int EN = 14;	// 
const int D0 = 4;	// 
const int D1 = 12;	// 
const int D2 = 13;	// 
const int D3 = 6;	// 

#define TRUE 1
#define FALSE 0

int main(int argc, char **argv)
{
	char c_result[2];
	char turn;
	char n, p, result;
	unsigned long i;
	unsigned long buff[33];
	char key[5];
	char tmpBuff[32];
	int debug_flag;
	int result_int;
	int lcd_fd;

	if (wiringPiSetup() < 0)
	{
	    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
	    return 1;
	}
	if (!bcm2835_init()) return 1;
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);
	//            rows, cols, bit, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	lcd_fd = lcdInit(2,   16,   4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	lcdHome(lcd_fd);
	//                 01234567890123456
	lcdPrintf(lcd_fd, " -=[ IR test ]=-");
	debug_flag = FALSE;
	while (1)
	{
		i++; // #i#
		delayMicroseconds(60);
		if (bcm2835_gpio_lev(PIN) == 1) turn = 1;
		else
		{
			if (turn == 1)
			{
				if ((n > 0) && (n <= 33)) buff[n-1] = i; // #i#
				n++;
				i = 0; // #i#
				if (n == 34)
				{
					n = 0;
					/*
					for (p = 0; p < 33; p++)
						printf("%d-%d;", p, buff[p]);
					*/
					if (buff[0] > 180 && buff[0] < 250 && buff[1] < 25 && buff[2] < 25 && buff[3] < 25 && buff[4] < 25 && buff[5] < 25 && buff[6] < 25 &&
						buff[7] < 25 && buff[8] < 25 && buff[9] > 25 && buff[10] > 25 && buff[11] > 25 && buff[12] > 25 && buff[13] > 25)
					{
						for (p = 0; p < 8; p++)
						{
							result >>= 1;
							if (buff[25+p] > 25) result |= 0x80;
						}
						sprintf(tmpBuff, "[%x]", result);
						lcdPosition(lcd_fd, 0, 1);
						lcdPrintf(lcd_fd, tmpBuff);
					}
					bcm2835_delay(200);
				}
			}
			turn = 0;
		}
	}
	bcm2835_close();
	return 0;
}
