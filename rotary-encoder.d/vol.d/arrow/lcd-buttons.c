#include <stdlib.h>
#include <stdio.h>

#include <wiringPi.h>
#include <lcd.h>

#ifndef TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

const int pinA = 6;
const int pinB = 5;
const int pinC = 4;
const int pinD = 3;

const int RS = 3;
const int EN = 14;
const int D0 = 4;
const int D1 = 12;
const int D2 = 13;
const int D3 = 6;

static unsigned char newChar[8] =
{
	0b01010,
	0b00100,
	0b01010,
	0b10001,
	0b01010,
	0b00100,
	0b01010,
	0b10001,
};

int buttonStateA, buttonStateB, buttonStateC, buttonStateD;
int lastButtonStateA, lastButtonStateB, lastButtonStateC, lastButtonStateD;
long lastDebounceTimeA, lastDebounceTimeB, lastDebounceTimeC, lastDebounceTimeD;
long debounceDelay = 50;

int main(int argc, char **argv)
{
	char buf[32];
	int lcdFD;
	int i;
	int reading;
	int col, row;
	int max_col, max_row;

	max_col = 15;
	max_row = 1;
	row = col = 0;
	lastButtonStateA  = lastButtonStateB  = lastButtonStateC  = lastButtonStateD  = HIGH;
	lastDebounceTimeA = lastDebounceTimeB = lastDebounceTimeC = lastDebounceTimeD = 0;
	//if (argc > 1) { num_sensors = atoi(argv[1]); }
	wiringPiSetup();
	lcdFD = lcdInit(max_row + 1, max_col + 1, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	lcdPosition(lcdFD, col, row);
	lcdCursor(lcdFD, TRUE);
	lcdCursorBlink(lcdFD, TRUE);
	lcdCharDef(lcdFD, 2, newChar);
	for (i = 3; i < 7; i++)
	{
		pinMode(i, INPUT);
		pullUpDnControl(i, PUD_UP);
	}
	while (1)
	{
		// A - down (up if non-lcd)
		reading = digitalRead(pinA);
		if (reading != lastButtonStateA)
		       	lastDebounceTimeA = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeA) > debounceDelay)
		{
			if (reading != buttonStateA)
			{
				lcdPuts(" ");
				if (row + 1 > max_row)
					row = -1;
				lcdPosition(lcdFD, col, ++row);
			}
		}
		lastButtonStateA = reading;
		// B - left (right if non-lcd)
		reading = digitalRead(pinB);
		if (reading != lastButtonStateB)
		       	lastDebounceTimeB = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeB) > debounceDelay)
		{
			if (reading != buttonStateB)
			{
				lcdPuts(" ");
				if (col - 1 <= 0)
					col = max_col + 1;
				lcdPosition(lcdFD, --col, row);
			}
		}
		lastButtonStateB = reading;
		// C - right
		reading = digitalRead(pinC);
		if (reading != lastButtonStateC)
		       	lastDebounceTimeC = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeC) > debounceDelay)
		{
			if (reading != buttonStateC)
			{
				lcdPuts(" ");
				if (col + 1 > max_col)
					col = -1;
				lcdPosition(lcdFD, ++col, row);
			}
		}
		lastButtonStateC = reading;
		// D - up
		reading = digitalRead(pinD);
		if (reading != lastButtonStateD)
		       	lastDebounceTimeD = millis(); // reset the debouncing timer
		if ((millis() - lastDebounceTimeD) > debounceDelay)
		{
			if (reading != buttonStateD)
			{
				lcdPuts(" ");
				if (row - 1 <= 0)
					row = max_row + 1;
				lcdPosition(lcdFD, col, --row);
			}
		}
		lastButtonStateD = reading;
		lcdPutchar(lcdFD, 2);
		/*
		move(3 + i, 0);
		printw("Input: %d Data: %0.3d", i, abs(analogRead(MY_8591A + i) - 255));
		refresh();
		delay(delay_n);
		*/
	}
	return EXIT_SUCCESS;
}
