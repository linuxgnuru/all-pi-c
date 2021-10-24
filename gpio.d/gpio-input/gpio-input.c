/*
	1 - 0-7 up
	2 - 7-0 down

 */
#include <stdio.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ncurses.h>

#include <wiringPi.h>

int main(int argc, char **argv)
{
	int i;
	int gpio_in, gpio_out;
	int delayn = 50;

	gpio_in = 17;
	gpio_out = 18;

	wiringPiSetupGpio();
	pinMode(gpio_out, OUTPUT);
	pinMode(gpio_in, INPUT);
	digitalWrite(gpio_out, LOW);

	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("Obstacle Avoidance");
	mvaddstr(2, 0, "using input pin GPIO 0 (BCM 17) and output pin GPIO 1 (BCM 18)");
	mvaddstr(3, 0, "Status: ");
	refresh();
	for (;;)
	{
		if (digitalRead(gpio_in) == LOW)
		{
			move(3, 8);
			printw("HIT");
			digitalWrite(gpio_out, HIGH);
		}
		else
		{
			move(3, 8);
			printw("   ");
			digitalWrite(gpio_out, LOW);
		}
		refresh();
		delay(delayn);
	}
	endwin();
	return(0);
}
