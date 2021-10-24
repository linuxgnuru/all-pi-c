#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>

static const int leds[10][7] = {
				{ 0, 1, 2, 7, 4, 5, 6 }, // 0
				{ 7, 7, 2, 7, 7, 5, 7 }, // 1
				{ 0, 7, 2, 3, 4, 7, 6 }, // 2
				{ 0, 7, 2, 3, 7, 5, 6 }, // 3
				{ 7, 1, 2, 3, 7, 5, 0 }, // 4
				{ 0, 1, 7, 3, 7, 5, 6 }, // 5
				{ 0, 1, 7, 3, 4, 5, 6 }, // 6
				{ 0, 7, 2, 7, 7, 5, 0 }, // 7
				{ 0, 1, 2, 3, 4, 5, 6 }, // 8
				{ 0, 1, 2, 3, 7, 5, 6 }  // 9
				};

int tmp_reading;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

int main()
{
	//int tmp[7];
	int i, j;
	/*
	int btnCtr;
	int buttonState;
	*/

	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	for (i = 0; i < 7; i++)
		pinMode(i, OUTPUT);
	/*
	pinMode(7, INPUT);
	pinMode(28, OUTPUT);
	pullUpDnControl(7, PUD_UP);
	*/
	while (1)
	{
		for (i = 0; i < 10; i++)
		{
			for (j = 0; j < 7; j++)
				digitalWrite(j, LOW);
			for (j = 0; j < 7; j++)
				digitalWrite(j, (leds[i][j] != 7 ? HIGH : LOW));
			/*
			for (j = 0; j < 7; j++)
			{
				tmp[j] = leds[i][j];
				//printf("j: %d\tval: %d\n", j, tmp[j]);
			}
			*/
			delay(500);
		}
		/*
		tmp_reading = digitalRead(7);
		if (tmp_reading != lastButtonState) lastDebounceTime = millis();
		if ((millis() - lastDebounceTime) > debounceDelay)
		{
			if (tmp_reading != buttonState)
			{
				buttonState = tmp_reading;
				if (buttonState == HIGH) digitalWrite(28, !digitalRead(28));
			}
		}
		lastButtonState = tmp_reading;
		*/
	}
	for (i = 0; i < 7; i++)
		digitalWrite(i, LOW);
	return 0;
}
