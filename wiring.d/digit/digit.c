#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
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

int main(int argc, char **argv)
{
	int h, i, j;
	int digit;
	int badFlag = 0;

	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	if (argc > 1)
	{
		digit = atoi(argv[1]);
		if (digit > 10 && digit != 11)
			badFlag = 1;
	}
	else
		badFlag = 1;
	if (badFlag == 1)
	{
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	pinMode(6, OUTPUT);
	for (i = 0; i < 7; i++)
		pinMode(ledPins[i], OUTPUT);
	if (digit != 11)
		for (i = 0; i < 7; i++)
			digitalWrite(ledPins[i], (leds[digit][i] != 30 ? HIGH : LOW));
	else
		for (i = 0; i < 7; i++)
			digitalWrite(ledPins[i], LOW);
	/*
	pinMode(7, INPUT);
	pinMode(28, OUTPUT);
	pullUpDnControl(7, PUD_UP);
	*/
/*
	//while (1)
	for (h = 0; h < 20; h++)
	{
		for (i = 0; i < 10; i++)
		{
			for (j = 0; j < 7; j++)
				digitalWrite(ledPins[j], LOW);
			for (j = 0; j < 7; j++)
				digitalWrite(ledPins[j], (leds[i][j] != 30 ? HIGH : LOW));
			//for (j = 0; j < 7; j++)
			//{
			//	tmp[j] = leds[i][j];
			//	//printf("j: %d\tval: %d\n", j, tmp[j]);
			//}
			delay(500);
		}
		//tmp_reading = digitalRead(7);
		//if (tmp_reading != lastButtonState) lastDebounceTime = millis();
		//if ((millis() - lastDebounceTime) > debounceDelay)
		//{
			//if (tmp_reading != buttonState)
			//{
				//buttonState = tmp_reading;
				//if (buttonState == HIGH) digitalWrite(28, !digitalRead(28));
			//}
		//}
		//lastButtonState = tmp_reading;
	}
	for (i = 0; i < 7; i++) digitalWrite(ledPins[i], LOW);
*/
	return EXIT_SUCCESS;
}
