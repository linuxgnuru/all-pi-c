#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8591A 100
#define MY_8591B 200

int rev(int val) { return abs(val - 255); }

int main(int argc, char **argv)
{
	int addr1 = 0x49;
//	int addr2 = 0x48;
	int i;
	int key;
	int num_sensors = 4;
	int delay_n = 200;
	int data1;
//	int data2;

	if (argc > 1)
	{
		num_sensors = atoi(argv[1]);
		if (num_sensors < 1 || num_sensors > 4)
		{
			printf("Error: number must be between 1 and 4\n");
			return(-1);
		}
	}
	wiringPiSetup();
	pcf8591Setup(MY_8591A, addr1);
//	pcf8591Setup(MY_8591B, addr2);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(1, 0, "Analog output");
	mvaddstr(2, 0, "0x49");
//	mvaddstr(8, 0, "0x48");
	refresh();
	while (1)
	{
		for (i = 0; i < num_sensors; i++)
		{
			// the read is always one step behind the selected input
			move(3 + i, 0);
			printw("Input: %d Data: %0.3d", i, rev(analogRead(MY_8591A + i)));
			refresh();
			delay(delay_n);
		}
		/*
		for (i = 0; i < num_sensors; i++)
		{
			// the read is always one step behind the selected input
			move(9 + i, 0);
			printw("Input: %d Data: %0.3d", i, rev(analogRead(MY_8591B + i)));
			refresh();
			delay(delay_n);
		}
		*/
		key = getch();
		if (key > -1)
			break;
	}
	endwin();
	return(0);
}
