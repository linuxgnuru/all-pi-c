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
	int data;
	int new_val;

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
		data = abs(analogRead(MY_8591) - 255);
		move(3, 0);
		printw("   ldr: %0.3d", data);
		refresh();
		// the read is always one step behind the selected input
		new_val = data;
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
