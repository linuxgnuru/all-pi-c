/*
   shift register pins:
	      +--------+
	2out -+  1  16 +- Vin
	3out -+  2  15 +- 1out
	4out -+  3  14 +- data (blue)
	5out -+  4  13 +- ground
	6out -+  5  12 +- latch (green + 1u cap if first)
	7out -+  6  11 +- clock (yellow)
	8out -+  7  10 +- Vin
	 gnd -+  8   9 +- serial out
	      +--------+
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sr595.h>

#define MY_8574 100

// debounce

int buttonState[8];
int lastButtonState[8];
long lastDebounceTime[8];
/*
int  ButtonState;
int  LasButtonState;
long LastDebounceTime;
*/
long debounceDelay = 50;

//void pcf_toggle(int pin) { digitalWrite(MY_8574 + pin, !digitalRead(MY_8574 + pin)); }
int devid = 0x20;   //you cam use "i2cdetect" to find this devid
int fd;	// 
int delayTime = 50;

int bits = 30;

const int dataPin = 0; // blue
const int latchPin = 1; // green
const int clockPin = 2; // yellow
const int addr = 200;

// catch signals and clean up GPIO
static void die(int sig)
{
	int i;
	for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
	//for (i = 0; i < 8; i++) digitalWrite(MY_8574 + i, LOW);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "\n\ncaught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "\n\nExiting due to Ctrl + C\n");
	exit(0);
}

void LtoR(int bitStop, int snakeLen, int times)
{
	int i, j;

	for (j = 0; j < times; j++)
	{
		for (i = 0; i < bitStop; i++)
		{
			if (i == 1)
			{
		       		digitalWrite(addr + i - 1, HIGH);
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i - 1, HIGH);
		       		digitalWrite(addr + i - 2, HIGH);
			}
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 30)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
				}
				else
				{
					if (i == 30)
					{
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
					}
					if (i == 31)
					{
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
					}
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
			       		digitalWrite(addr + i - 5, HIGH);
				}
				if (i >= 6 && i < 30)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
			       		digitalWrite(addr + i - 5, HIGH);
			       		digitalWrite(addr + i - 6, HIGH);
				}
				else
				{
					switch (i)
					{
						case 30:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
			       			digitalWrite(addr + 24, HIGH);
						break;
						case 31:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
						break;
						case 32:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
						break;
						case 33:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
						break;
						case 34:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
						break;
					}
				}
			}
		       	digitalWrite(addr + i, HIGH);
			/*
			 * l to r - off
			 */
			delay(delayTime);
			if (i == 1)
			{
		       		digitalWrite(addr + i - 1, LOW);
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i - 1, LOW);
		       		digitalWrite(addr + i - 2, LOW);
			}
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 30)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
				}
				else
				{
					if (i == 30)
					{
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
					}
					if (i == 31)
					{
			       			digitalWrite(addr + 28, LOW);
					}
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
			       		digitalWrite(addr + i - 5, LOW);
				}
				if (i >= 6 && i < 30)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
			       		digitalWrite(addr + i - 5, LOW);
			       		digitalWrite(addr + i - 6, LOW);
				}
				else
				{
					switch (i)
					{
						case 30:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
			       			digitalWrite(addr + 24, LOW);
						break;
						case 31:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
						break;
						case 32:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
						break;
						case 33:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
						break;
						case 34:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
						break;
					}
				}
			}
		       	digitalWrite(addr + i, LOW);
		}
	}
	for (i = 0; i < bits; i++)
	{
		digitalWrite(addr + i, LOW);
	}
}

void RtoL(int bitStop, int snakeLen, int times)
{
	int i, j;

	for (j = 0; j < times; j++)
	{
		for (i = bits - 1; i > -1; i--)
		{
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 28)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
				}
				if (i == 28)
				{
			       		digitalWrite(addr + 28, HIGH);
			       		digitalWrite(addr + 29, HIGH);
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
			       		digitalWrite(addr + i + 5, HIGH);
				}
				if (i >= 6 && i < 24)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
			       		digitalWrite(addr + i + 5, HIGH);
			       		digitalWrite(addr + i + 6, HIGH);
				}
				else
			       	{
					switch (i)
					{
						case 24:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
			       			digitalWrite(addr + 24, HIGH);
						break;
						case 25:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
						break;
						case 26:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
						break;
						case 27:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
						break;
						case 28:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
						break;
					}
				}
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i + 1, HIGH);
		       		digitalWrite(addr + i + 2, HIGH);
			}
			if (i == 1)
			{
				digitalWrite(addr + i + 1, HIGH);
			}
			digitalWrite(addr + i, HIGH);
			// r to l off
			delay(delayTime);
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 28)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
				}
				if (i == 28)
				{
			       		digitalWrite(addr + 28, LOW);
			       		digitalWrite(addr + 29, LOW);
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
			       		digitalWrite(addr + i + 5, LOW);
				}
				if (i >= 6 && i < 24)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
			       		digitalWrite(addr + i + 5, LOW);
			       		digitalWrite(addr + i + 6, LOW);
				}
				else
			       	{
					switch (i)
					{
						case 24:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
			       			digitalWrite(addr + 24, LOW);
						break;
						case 25:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
						break;
						case 26:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
						break;
						case 27:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
						break;
						case 28:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
						break;
					}
				}
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i + 1, LOW);
		       		digitalWrite(addr + i + 2, LOW);
			}
			if (i == 1)
			{
		       		digitalWrite(addr + i + 1, LOW);
			}
			digitalWrite(addr + i, LOW);
		}
	}
	for (i = 0; i < bits; i++)
	{
		digitalWrite(addr + i, LOW);
	}

}

void Snake(int bitStop, int snakeLen, int times)
{
	int i, j;

	for (j = 0; j < times; j++)
	{
		// start left to right first 32 bits
		for (i = 0; i < bitStop; i++)
		{
			if (i == 1)
			{
		       		digitalWrite(addr + i - 1, HIGH);
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i - 1, HIGH);
		       		digitalWrite(addr + i - 2, HIGH);
			}
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 30)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
				}
				else
				{
					if (i == 30)
					{
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
					}
					if (i == 31)
					{
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
					}
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
			       		digitalWrite(addr + i - 5, HIGH);
				}
				if (i >= 6 && i < 30)
				{
			       		digitalWrite(addr + i - 1, HIGH);
			       		digitalWrite(addr + i - 2, HIGH);
			       		digitalWrite(addr + i - 3, HIGH);
			       		digitalWrite(addr + i - 4, HIGH);
			       		digitalWrite(addr + i - 5, HIGH);
			       		digitalWrite(addr + i - 6, HIGH);
				}
				else
				{
					switch (i)
					{
						case 30:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
			       			digitalWrite(addr + 24, HIGH);
						break;
						case 31:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
						break;
						case 32:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
						break;
						case 33:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
						break;
						case 34:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
						break;
					}
				}
			}
		       	digitalWrite(addr + i, HIGH);
			delay(delayTime);
			// now turn off the LED
			if (i == 1)
			{
		       		digitalWrite(addr + i - 1, LOW);
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i - 1, LOW);
		       		digitalWrite(addr + i - 2, LOW);
			}
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 30)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
				}
				else
				{
					if (i == 30)
					{
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
					}
					if (i == 31)
					{
			       			digitalWrite(addr + 28, LOW);
					}
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
			       		digitalWrite(addr + i - 5, LOW);
				}
				if (i >= 6 && i < 30)
				{
			       		digitalWrite(addr + i - 1, LOW);
			       		digitalWrite(addr + i - 2, LOW);
			       		digitalWrite(addr + i - 3, LOW);
			       		digitalWrite(addr + i - 4, LOW);
			       		digitalWrite(addr + i - 5, LOW);
			       		digitalWrite(addr + i - 6, LOW);
				}
				else
				{
					switch (i)
					{
						case 30:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
			       			digitalWrite(addr + 24, LOW);
						break;
						case 31:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
						break;
						case 32:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
						break;
						case 33:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
						break;
						case 34:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
						break;
					}
				}
			}
		       	digitalWrite(addr + i, LOW);
		}
		delay(delayTime);
		//digitalWrite(addr + bits - 1, 0);
		// now go the other way
		delay(delayTime);
		for (i = bits - 1; i > -1; i--)
		{
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 28)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
				}
				if (i == 28)
				{
			       		digitalWrite(addr + 28, HIGH);
			       		digitalWrite(addr + 29, HIGH);
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
			       		digitalWrite(addr + i + 5, HIGH);
				}
				if (i >= 6 && i < 24)
				{
			       		digitalWrite(addr + i + 1, HIGH);
			       		digitalWrite(addr + i + 2, HIGH);
			       		digitalWrite(addr + i + 3, HIGH);
			       		digitalWrite(addr + i + 4, HIGH);
			       		digitalWrite(addr + i + 5, HIGH);
			       		digitalWrite(addr + i + 6, HIGH);
				}
				else
			       	{
					switch (i)
					{
						case 24:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
			       			digitalWrite(addr + 24, HIGH);
						break;
						case 25:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
			       			digitalWrite(addr + 25, HIGH);
						break;
						case 26:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
			       			digitalWrite(addr + 26, HIGH);
						break;
						case 27:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
			       			digitalWrite(addr + 27, HIGH);
						break;
						case 28:
			       			digitalWrite(addr + 29, HIGH);
			       			digitalWrite(addr + 28, HIGH);
						break;
					}
				}
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i + 1, HIGH);
		       		digitalWrite(addr + i + 2, HIGH);
			}
			if (i == 1)
			{
				digitalWrite(addr + i + 1, HIGH);
			}
			digitalWrite(addr + i, HIGH);
			delay(delayTime);
			if (snakeLen == 4)
			{
				if (i >= 3 && i < 28)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
				}
				if (i == 28)
				{
			       		digitalWrite(addr + 28, LOW);
			       		digitalWrite(addr + 29, LOW);
				}
			}
			else if (snakeLen == 6)
			{
				if (i == 3)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
				}
				if (i == 4)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
				}
				if (i == 5)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
			       		digitalWrite(addr + i + 5, LOW);
				}
				if (i >= 6 && i < 24)
				{
			       		digitalWrite(addr + i + 1, LOW);
			       		digitalWrite(addr + i + 2, LOW);
			       		digitalWrite(addr + i + 3, LOW);
			       		digitalWrite(addr + i + 4, LOW);
			       		digitalWrite(addr + i + 5, LOW);
			       		digitalWrite(addr + i + 6, LOW);
				}
				else
			       	{
					switch (i)
					{
						case 24:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
			       			digitalWrite(addr + 24, LOW);
						break;
						case 25:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
			       			digitalWrite(addr + 25, LOW);
						break;
						case 26:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
			       			digitalWrite(addr + 26, LOW);
						break;
						case 27:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
			       			digitalWrite(addr + 27, LOW);
						break;
						case 28:
			       			digitalWrite(addr + 29, LOW);
			       			digitalWrite(addr + 28, LOW);
						break;
					}
				}
			}
			if (i == 2)
			{
		       		digitalWrite(addr + i + 1, LOW);
		       		digitalWrite(addr + i + 2, LOW);
			}
			if (i == 1)
			{
		       		digitalWrite(addr + i + 1, LOW);
			}
			digitalWrite(addr + i, LOW);
		}
	}
	for (i = 0; i < bits; i++)
	{
		digitalWrite(addr + i, LOW);
	}
}

void swapBlink(int times)
{
	int i, j;

	for (j = 0; j < times; j++)
	{
		for (i = 0; i < bits; i++)
		{
			if (i % 2 != 0) digitalWrite(addr + i, HIGH);
			else digitalWrite(addr + i, LOW);
		}
		delay(500);
		for (i = 0; i < bits; i++)
		{
			if (i % 2 == 0) digitalWrite(addr + i, HIGH);
			else digitalWrite(addr + i, LOW);
		}
		delay(500);
	}
	for (i = 0; i < bits; i++)
	{
		digitalWrite(addr + i, LOW);
	}
}

void Blink(int times)
{
	int i, j;

	for (j = 0; j < times; j++)
	{
		for (i = 0; i < bits; i++) digitalWrite(addr + i, 1);
		delay(500);
		for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
		delay(500);
	}
	for (i = 0; i < bits; i++)
	{
		digitalWrite(addr + i, LOW);
	}
}

int main(int argc, char **argv)
{
	int pin;
	int err = 0;
	int i;
	int bitStop;
	int snakeLen = 4;
	int reading;

	/*
	if (argc > 1)
	{
		pin = atoi(argv[1]);
		if (pin < 0 || pin > 7) err = 1;
	}
	else
	       	err = 1;
	if (err == 1)
	{
		printf("usage: %s PIN\nwhere PIN is a number from 0 - 7\n", argv[0]);
		return 0;
	}
	*/
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	/*
	 * START OF INIT
	 */
	if ((fd = wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	if ((wiringPiI2CWrite(fd, 0xff)) < 0)
	{
	    fprintf(stderr, "write badly %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	//delay(500);
	delay(50);
	if ((wiringPiI2CWrite(fd, 0)) < 0)
	{
	    fprintf(stderr, "write badly %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	//delay(500);
	delay(50);
	err = wiringPiI2CRead(fd);
	/*
	 * END OF INIT
	 */
	pcf8574Setup(MY_8574, devid);
	sr595Setup(addr, bits, dataPin, clockPin, latchPin);
	for (i = 0; i < 8; i++)
	{
		lastButtonState[i] = HIGH;
		lastDebounceTime[i] = 0;
	}
	//lastPlayButtonState = lastPrevButtonState = lastNextButtonState = lastInfoButtonState = lastQuitButtonState = HIGH;
	//lastPlayDebounceTime = lastPrevDebounceTime = lastNextDebounceTime = lastInfoDebounceTime = lastQuitDebounceTime = 0;
	for (i = 0; i < bits; i++)
	       	digitalWrite(addr + i, 0);
	if (delayTime < 60)
		snakeLen = 6;
	if (snakeLen == 4)
		bitStop = bits + 2;
	else
		bitStop = bits + 5;
	while (1)
	{
		for (i = 0; i < 8; i++)
		{
			reading = digitalRead(MY_8574 + i);
			if (reading != lastButtonState[i])
				lastDebounceTime[i] = millis();
			if ((millis() - lastDebounceTime[i]) > debounceDelay)
			{
				if (reading != buttonState[i])
				{
					buttonState[i] = reading;
					if (buttonState[i] == HIGH)
					{
						// do something
						switch (i)
						{
							case 0:
								if (digitalRead(MY_8574 + i) == HIGH)
									Snake(bitStop, snakeLen, 5);
								break;
							case 1:
								if (digitalRead(MY_8574 + i) == HIGH)
									swapBlink(5);
								break;
							case 2:
								if (digitalRead(MY_8574 + i) == HIGH)
									Blink(5);
								break;
							case 3:
								if (digitalRead(MY_8574 + i) == HIGH)
									LtoR(bitStop, snakeLen, 5);
								break;
							case 4:
								if (digitalRead(MY_8574 + i) == HIGH)
									RtoL(bitStop, snakeLen, 5);
								break;
							case 5:
								if (digitalRead(MY_8574 + i) == HIGH)
									printf("5\n");
								break;
							case 6:
								if (digitalRead(MY_8574 + i) == HIGH)
									printf("6\n");
								break;
							case 7:
								if (digitalRead(MY_8574 + i) == HIGH)
									printf("7\n");
								break;
						}
					}
				}
			}
			lastButtonState[i] = reading;
			delay(100);
		}
		//printf("%d %s\n", i, (digitalRead(MY_8574 + i) == HIGH ? "HIGH" : "LOW"));
	}
	return EXIT_SUCCESS;
}
