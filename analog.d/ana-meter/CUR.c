#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8591 100

int main(int argc, char **argv)
{
	int addr = 0x48;
	int i;
	int key;
	int num_sensors = 4;
	int delay_n = 200;

	if (argc > 1)
	{
		num_sensors = atoi(argv[1]);
		if (num_sensors < 1 || num_sensors > 4)
		{
			printf("Error: number must be between 1 and 4\n");
			return(-1);
		}
	}
	else
		num_sensors = 1;
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
	mvaddstr(3, 0, "--------------------------");
	mvaddstr(4,  0, "Input");
	for (i = 0; i < num_sensors; i++)
	{
		move(5, i * 5);
		printw("%d", i);
	}
	refresh();
	while (1)
	{
		for (i = 0; i < num_sensors; i++)
		{
			// the read is always one step behind the selected input
			move(6, 0 + (i * 5));
			//printw("%0.3d", abs(analogRead(MY_8591 + i) - 255));
			printw("%0.3d", analogRead(MY_8591 + i));
			refresh();
			delay(delay_n);
		}
		key = getch();
		if (key > -1) break;
	}
	endwin();
	return(0);
}
