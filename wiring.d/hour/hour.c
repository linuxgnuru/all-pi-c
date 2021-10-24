#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <wiringPi.h>

static const int ledPins[7] = { 21, 23, 24, 25, 26, 28, 22 };
static const int decimalPin = 6;

static const int leds[10][7] = {
				{ 21, 23, 24, 25, 30, 28, 22 }, // 0
				{ 21, 23, 30, 30, 30, 30, 30 }, // 1
				{ 30, 23, 24, 30, 26, 28, 22 }, // 2
				{ 21, 23, 24, 30, 26, 30, 22 }, // 3
				{ 21, 23, 30, 25, 26, 30, 30 }, // 4
				{ 21, 30, 24, 25, 26, 30, 22 }, // 5
				{ 21, 30, 24, 25, 26, 28, 22 }, // 6
				{ 21, 23, 24, 30, 30, 30, 30 }, // 7
				{ 21, 23, 24, 25, 26, 28, 22 }, // 8
				{ 21, 23, 24, 25, 26, 30, 22 }  // 9
				};

int tmp_reading;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

void usage(char *name)
{
	printf("usage: %s [digit]\n", name);
	printf("(to clear, use 11 for digit)\n\n");
	printf( "pins:\n"
		"   -   \t    24\n"
		" |   | \t  25  23\n"
		"   -   \t    26\n"
		" |   | \t  28  21\n"
		"   -   \t    22\n");
}

void digit(int num)
{
	int i;

	for (i = 0; i < 7; i++)
		digitalWrite(ledPins[i], (leds[num][i] != 30 ? HIGH : LOW));
}

void clear()
{
	int i;
	for (i = 0; i < 7; i++) digitalWrite(ledPins[i], LOW);
}

int main(int argc, char **argv)
{
	struct tm *t;
	time_t tim;
	int i, j;
	int my_hour;
	int tens;

	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	pinMode(6, OUTPUT);
	pinMode(3, OUTPUT);
	for (i = 0; i < 7; i++)
		pinMode(ledPins[i], OUTPUT);
	/*
	pinMode(7, INPUT);
	pinMode(28, OUTPUT);
	pullUpDnControl(7, PUD_UP);
	*/
	while (1)
	{
		tim = time(NULL);
		t = localtime(&tim);
		my_hour = t->tm_hour;
		digitalWrite(3, (my_hour > 12 ? HIGH : LOW));
    		my_hour = (my_hour > 12 || my_hour == 0 ? abs(my_hour - 12) : my_hour);
		tens = my_hour % 10;
		digitalWrite(6, (tens == 0 ? HIGH : LOW));
		if (my_hour >= 10)
			my_hour -= 10;
		digit(my_hour);
		//printf("my_hour: %d\ntens: %d\n", my_hour, tens);
		delay(500);
	}
	return EXIT_SUCCESS;
}
