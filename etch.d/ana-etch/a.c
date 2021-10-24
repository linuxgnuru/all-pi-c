#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include <wiringPi.h>

#define TRUE 1
#define FALSE 0

#define MY_8591 100
const int addr = 0x48;
const int tiltPin = 6;

int buttonState;
int lastButtonState;
long lastDebounceTime;
long debounceDelay = 50;

double map(double x, double x0, double x1, double y0, double y1)
{
	double y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
	return y;
}

int main(int argc, char **argv)
{
	int i;
	int x, y;
	int ch;
	int maxx, maxy;
	int reading;
	int clearFlag = FALSE;
	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
	pinMode(tiltPin, INPUT);
	pullUpDnControl(tiltPin, PUD_UP);
	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_GREEN);
	noecho();
	cbreak();
	nodelay(stdscr, true);
	wclear(stdscr);
	getmaxyx(stdscr, maxy, maxx);
	ch = ' ';
	y = maxy / 2;
	x = maxx / 2;
	move(y, x);
	refresh();
	while (1)
	{
		reading = digitalRead(tiltPin);
		if (reading != lastButtonState) lastDebounceTime = millis();
		if ((millis() - lastDebounceTime) > debounceDelay)
		{
			if (reading != buttonState)
			{
				buttonState = reading;
				if (buttonState == LOW)
					clear();
			}
		}
		lastButtonState = reading;
			y = map(analogRead(MY_8591), 0, 255, 0, maxy - 1);
			x = map(analogRead(MY_8591 + 1), 0, 255, 0, maxx - 1);
			move(y, x);
			attron(A_BOLD);
			attron(COLOR_PAIR(2));
			printw("*");
			attroff(COLOR_PAIR(2));
			attroff(A_BOLD);
			refresh();
	}
	endwin();
	return EXIT_SUCCESS;
}
