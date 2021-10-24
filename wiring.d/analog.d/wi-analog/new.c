#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8591 100

int rev(int value)
{
	return (abs(value - 255));
}

int main(int argc, char **argv)
{
	int delay_n = 200;
	int key;
	int addr = 0x48;
	int pin = 0;

	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("Value: ");
	refresh();
	while (1)
	{
		move(0, 7);
		printw("%0.3d", rev(analogRead(MY_8591 + pin)));
		refresh();
		delay(delay_n);
		key = getch();
		if (key > -1)
			break;
	}
	endwin();
	return 0;
}

/*
	for (i = 0; i < 8; i++)
	{
		analogRead(MY_8574 + i, 1);
	}
	for (loop = 0; loop < 2; loop++)
	{
		for (i = 0; i < 8; i++)
		{
			swap_v = (swap == 0 ? i : abs(i-7));
			digitalWrite(MY_8574 + swap_v, !digitalRead(MY_8574 + swap_v));
			delay(delay_n);
		}
		swap = (swap == 0 ? 1 : 0);
	}
	return 0;
}
*/
