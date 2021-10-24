#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>

#include <wiringPi.h>

#define beta 4090 //the beta of the thermistor

#define MY_8591 100

const int addr = 0x48;

static void die(int sig)
{
  if (sig != 0 && sig != 2)
      (void)fprintf(stderr, "caught signal %d\n", sig);
  if (sig == 2)
      (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
  endwin();
  exit(0);
}

int main(int argc, char **argv)
{
    int key;
	int delay_n = 200;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
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
#endif
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
		// the read is always one step behind the selected input
		move(3, 0);
		printw("Data: %0.3d", abs(analogRead(MY_8591 + 1) - 255));
		refresh();
		delay(delay_n);
		key = getch();
		if (key > -1)
			break;
	}
	endwin();
	return(0);
}
