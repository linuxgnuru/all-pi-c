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

#define beta 4090 //the beta of the thermistor
#define resistance 10 //the value of the pull-down resistor

const int del = 500; // delay

float getTemp(long val)
{
  //float tempC = beta /(log((1025.0 * 10 / val - 10) / 10) + beta / 298.0) - 273.0;
  float tempC = beta /(log((257.0 * 10 / val - 10) / 10) + beta / 298.0) - 273.0;
  float tempF = ((tempC * 9) / 5) + 32;
  float curTemp;
  curTemp = tempC;
  return curTemp;
}

int main(int argc, char **argv)
{
	int addr = 0x48;
	int i;
	int key;
//	int num_sensors = 4;
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
    }
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
