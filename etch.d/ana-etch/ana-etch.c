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

	/*
	if (argc > 1)
	{
		num_sensors = atoi(argv[1]);
		if (num_sensors < 1 || num_sensors > 4)
		{
			printf("Error: number must be between 1 and 4\n");
			return(-1);
		}
	}
	*/
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
	//curs_set(0);
	wclear(stdscr);
	attron(A_BOLD);
	attron(COLOR_PAIR(1));
	printw("Raspberry Pi Etch-A-Sketch");
	attroff(A_BOLD);
	attroff(COLOR_PAIR(1));
	wrefresh(stdscr);
	getmaxyx(stdscr, maxy, maxx);
	/*
	move(1, 0);
	printw("maxy: %d maxx: %d", maxy, maxx);
	*/
	ch = ' ';
	//while ('q' != ch)
	y = maxy / 2;
	x = maxx / 2;
	move(y, x);
	refresh();
	while (1)
	{
		reading = digitalRead(tiltPin);
		if (reading != lastButtonState)
		       	lastDebounceTime = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTime) > debounceDelay)
		{
			if (reading != buttonState)
			{
				buttonState = reading;
				if (buttonState == LOW)
				{
					clear();
					//clearFlag = TRUE;
				}
			}
		}
		lastButtonState = reading;
		//if (clearFlag == FALSE)
		//{
			y = map(analogRead(MY_8591), 0, 255, 0, maxy - 1);
			x = map(analogRead(MY_8591 + 1), 0, 255, 0, maxx - 1);
			//move(2, 0);
			//printw("%02d, %02d", y, x);
			move(y, x);
			attron(A_BOLD);
			attron(COLOR_PAIR(2));
			printw("*");
			attroff(COLOR_PAIR(2));
			attroff(A_BOLD);
			refresh();
		//}
		//clearFlag = FALSE;
	}
	endwin();
	return EXIT_SUCCESS;
}
