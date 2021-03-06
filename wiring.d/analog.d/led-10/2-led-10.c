#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ncurses.h>

#include <linux/i2c-dev.h>
#include <wiringPi.h>

#define MY_8591 100

const int gpio_leds[] = { 21, 22, 23, 24, 29, 28, 26, 25, 5, 2 };

int main(int argc, char **argv)
{
	unsigned char value[4];
	int addr = 0x48;
	int i;
	int key;
	int delay_n = 200;
	int data1;
	int data2;
	int oneORtwo = 2;
	int new_val;

	if (argc == 2)
	{
		oneORtwo = atoi(argv[1]);
		if (oneORtwo != 1 && oneORtwo != 2)
		{
			printf("input pin must be either 1 or 2\n");
			return 1;
		}
	}
	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(1, 0, "Analog output");
	mvaddstr(2, 0, "0x48");
	refresh();
	for (i = 0; i < 10; i++)
		pinMode(gpio_leds[i], OUTPUT);
	while (1)
	{
		data1 = analogRead(MY_8591);
		data2 = analogRead(MY_8591 + 1);
		move(3, 0);
		printw("   10k pot: %0.3d", data1);
		move(4, 0);
		printw("   50k pot: %0.3d", data2);
		refresh();
		// the read is always one step behind the selected input
		new_val = (oneORtwo == 1 ? data1 : data2);
		new_val /= 25;
		for (i = 0; i < 10; i++)
		{
			if (i < new_val) digitalWrite(gpio_leds[i], HIGH);
			else digitalWrite(gpio_leds[i], LOW);
		}
		delay(delay_n);
		key = getch();
		if (key > -1)
			break;
	}
	endwin();
	return(0);
}
