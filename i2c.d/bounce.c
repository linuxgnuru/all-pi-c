#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <libgen.h>

#include <wiringPi.h>

/*
 * Debounce tracking stuff
 */
int  leftButtonState, rightButtonState;
int  lastLeftButtonState,  lastRightButtonState;
long lastLeftDebounceTime, lastRightDebounceTime;
long debounceDelay = 50;

const int buttonPins[] = { 21, 22 };

int main(int argc, char **argv)
{
	int reading;
	lastLeftButtonState = lastRightButtonState = HIGH;
	lastLeftDebounceTime = lastRightDebounceTime = 0;
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return 1;
	}
	pinMode(21, INPUT);
	pinMode(22, INPUT);
	pullUpDnControl(21, PUD_UP);
	pullUpDnControl(22, PUD_UP);
	reading = digitalRead(leftButtonPin);
	// check to see if you just pressed the button 
	// (i.e. the input went from HIGH to LOW),  and you've waited 
	// long enough since the last press to ignore any noise:  
	// If the switch changed, due to noise or pressing:
	if (reading != lastLeftButtonState)
		lastLeftDebounceTime = millis(); // reset the debouncing timer
	if ((millis() - lastLeftDebounceTime) > debounceDelay)
	{
		// whatever the reading is at, it's been there for longer
		// than the debounce delay, so take it as the actual current state:
		// if the button state has changed:
		if (reading != leftButtonState)
		{
			leftButtonState = reading;
			if (leftButtonState == LOW)
			{
			}
		}
	}
	// save the reading. Next time through the loop, it'll be the lastButtonState:
	lastLeftButtonState = reading;
	reading = digitalRead(rightButtonPin);
	if (reading != lastRightButtonState)
		lastRightDebounceTime = millis();
	if ((millis() - lastRightDebounceTime) > debounceDelay)
	{
		if (reading != rightButtonState)
		{
			rightButtonState = reading;
			if (rightButtonState == LOW)
			{
			}
		}
	}
	lastRightButtonState = reading;
	return 0;
}
