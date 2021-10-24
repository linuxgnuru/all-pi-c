#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#include <wiringPi.h>

#define TRUE 1
#define FALSE 0

const static int pinA = 6;
const static int pinB = 5;
const static int pinC = 4;
const static int pinD = 3;

int buttonStateA, buttonStateB, buttonStateC, buttonStateD;
int lastButtonStateA, lastButtonStateB, lastButtonStateC, lastButtonStateD;
long lastDebounceTimeA, lastDebounceTimeB, lastDebounceTimeC, lastDebounceTimeD;
long debounceDelay = 50;

int main(int argc, char **argv)
{
	int i;
	int reading;
	int x, y;
	int ch;
	int maxx, maxy;

	lastButtonStateA = lastButtonStateB = lastButtonStateC = lastButtonStateD = HIGH;
	lastDebounceTimeA = lastDebounceTimeB = lastDebounceTimeC = lastDebounceTimeD = 0;
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
	for (i = 3; i < 7; i++)
	{
		pinMode(i, INPUT);
		pullUpDnControl(i, PUD_UP);
	}
	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLUE);
	noecho();
	cbreak();
	nodelay(stdscr, true);
	//curs_set(0);
	wclear(stdscr);
	attron(A_BOLD);
	attron(COLOR_PAIR(1));
	printw("ncurses - button");
	attroff(A_BOLD);
	attroff(COLOR_PAIR(1));
	/*
	mvaddstr(1, 0, "Analog output");
	mvaddstr(2, 0, "0x49");
	mvaddstr(8, 0, "0x48");
	*/
	wrefresh(stdscr);
	getmaxyx(stdscr, maxy, maxx);
	move(1, 0);
	printw("maxy: %d maxx: %d", maxy, maxx);
	ch = ' ';
	//while ('q' != ch)
	y = maxy / 2;
	x = maxx / 2;
	move(y, x);
	refresh();
	while (1)
	{
		//mvaddstr(maxy/2, maxx/2, "middle");
		//ch = wgetch(stdscr);
		// A
		reading = digitalRead(pinA);
		if (reading != lastButtonStateA)
		       	lastDebounceTimeA = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeA) > debounceDelay)
		{
			if (reading != buttonStateA)
			{
				buttonStateA = reading;
				if (buttonStateA == LOW)
				{
					if (y + 1 < maxy)
						move(++y, x);
				}
			}
		}
		lastButtonStateA = reading;
		// B
		reading = digitalRead(pinB);
		if (reading != lastButtonStateB)
		       	lastDebounceTimeB = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeB) > debounceDelay)
		{
			if (reading != buttonStateB)
			{
				buttonStateB = reading;
				if (buttonStateB == LOW)
				{
					if (x - 1 >= 0)
						move(y, --x);
				}
			}
		}
		lastButtonStateB = reading;
		// C
		reading = digitalRead(pinC);
		if (reading != lastButtonStateC)
		       	lastDebounceTimeC = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeC) > debounceDelay)
		{
			if (reading != buttonStateC)
			{
				buttonStateC = reading;
				if (buttonStateC == LOW)
				{
					if (x + 1 < maxx)
						move(y, ++x);
				}
			}
		}
		lastButtonStateC = reading;
		// D
		reading = digitalRead(pinD);
		if (reading != lastButtonStateD)
		       	lastDebounceTimeD = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeD) > debounceDelay)
		{
			if (reading != buttonStateD)
			{
				buttonStateD = reading;
				if (buttonStateD == LOW)
				{
					if (y - 1 >= 0)
						move(--y, x);
				}
			}
		}
		lastButtonStateD = reading;
		move(2, 0);
		printw("%02d, %02d", y, x);
		move(y, x);
		attron(A_BOLD);
		attron(COLOR_PAIR(2));
		printw("*");
		attroff(COLOR_PAIR(2));
		attroff(A_BOLD);
		refresh();
		/*
		move(3 + i, 0);
		printw("Input: %d Data: %0.3d", i, abs(analogRead(MY_8591A + i) - 255));
		refresh();
		delay(delay_n);
		*/
	}
	endwin();
	return EXIT_SUCCESS;
}
