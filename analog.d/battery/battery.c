#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
//#include <linux/i2c-dev.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8591 100

const int addr = 0x48;
const int del = 500; // delay

float meter(int val)
{
    float Vin = 3.3;      // variable to store the input voltage
    float Vout = 0;   // variable to store the output voltage
    float R1 = 10;    // variable to store the R1 value
    float R2 = 0;     // variable to store the R2 value
    float buffer = 0; // buffer variable for calculation

    Vout = (3.3 / 255.0) * val;
    buffer = (Vin / Vout) - 1;
    R2 = R1 / buffer;
    return R2;
}

int main(int argc, char **argv)
{
	int i;
	int key;
	int delay_n = 200;

	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(1, 0, "Analog output");
	refresh();
	while (1)
	{
        move(3, 0);
		printw("%0.3f", meter(analogRead(MY_8591 + 2)) * 3.3);
        refresh();
		delay(delay_n);
		key = getch();
		if (key > -1)
			break;
    }
    endwin();
    return EXIT_SUCCESS;
}
#if 0
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
	while (1)
	{
		for (i = 0; i < num_sensors; i++)
		{
			// the read is always one step behind the selected input
			move(3 + i, 0);
			printw("Input: %d Data: %0.3d", i, abs(analogRead(MY_8591 + i) - 255));
			refresh();
			delay(delay_n);
		}
		key = getch();
		if (key > -1)
			break;
	}
	endwin();
	return(0);
}
#endif
