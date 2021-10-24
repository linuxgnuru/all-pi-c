#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>

#define PIN RPI_GPIO_P1_18

#define TRUE 1
#define FALSE 0

int main(int argc, char **argv)
{
	char c_result[2];
	char turn;
	char n, p, result;
	unsigned long i;
	unsigned long buff[33];

	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("IR Remote Control");
	mvaddstr(2, 0, "IRm Test Start: ");
	refresh();
	if (!bcm2835_init())
		return 1;
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);
	while (1)
	{
		i++; // #i#
		delayMicroseconds(60);
		if (bcm2835_gpio_lev(PIN) == 1)
			turn = 1;
		else
		{
			if (turn == 1)
			{
				if ((n > 0) && (n <= 33))
					buff[n-1] = i; // #i#
				n++;
				i = 0; // #i#
				if (n == 34)
				{
					n = 0;
					for (p = 0; p < 33; p++)
					{
						move(4, 0);
						printw("%d-%d;", p, buff[p]);
						//printw("Result: ox%x", result);
						refresh();
					}
					if (buff[0] > 180 && buff[0] < 250 && buff[1] < 25 && buff[2] < 25 && buff[3] < 25 && buff[4] < 25 && buff[5] < 25 && buff[6] < 25 &&
						buff[7] < 25 && buff[8] < 25 && buff[9] > 25 && buff[10] > 25 && buff[11] > 25 && buff[12] > 25 && buff[13] > 25)
					{
						for (p = 0; p < 8; p++)
						{
							result >>= 1;
							if (buff[25+p] > 25)
								result |= 0x80;
						}
						move(3, 0);
						printw("Result: ox%x", result);
						refresh();
					}
					bcm2835_delay(200);
				}
			}
			turn = 0;
		}
	}
	bcm2835_close();
	endwin();
	return 0;
}

