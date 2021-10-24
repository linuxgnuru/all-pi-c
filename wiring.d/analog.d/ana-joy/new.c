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
	int x;
	int y;
	int delay_n = 200;
	int key;

	wiringPiSetup();
	pcf8591Setup(MY_8591, 0x48);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("X: ");
	move(1,0);
	printw("Y: ");
	refresh();
	while (1)
	{
		x = rev(analogRead(100));
		y = rev(analogRead(101));
		move(0, 3);
		printw("%0.3d", x);
		move(1, 3);
		printw("%0.3d", y);
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
