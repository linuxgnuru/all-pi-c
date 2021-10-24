#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <wiringPi.h>

#define MY_8574 100

const static int hours[] = { 5, 6, 7, 21, 22, 23, 24, 25, 26, 27, 28, 29 };

int main(int argc, char **argv)
{
	struct tm *t;
	time_t tim;
	int last_hour = 14;
	int i;
	int c_hour;
	int c_min;

	wiringPiSetup();
	pcf8574Setup(MY_8574, 0x20);
	//digitalWrite(MY_8574 + i, 0);
	for (i = 0; i < 8; i++)
	{
		pinMode(i, OUTPUT);
		digitalWrite(MY_8574 + i, 1);
		digitalWrite(i, LOW);
	}
	for (i = 21; i < 30; i++)
	{
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	while (1)
	{
		tim = time(NULL);
		t = localtime(&tim);
		//printf("%02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
		c_hour = t->tm_hour;
		c_min = t->tm_min;
    		if (last_hour == 14)
			last_hour = c_hour;
		// AM or PM
		// TODO
    		//digitalWrite(13, (c_hour > 12 ? HIGH : LOW));

		//
		// HOURS
		//
		// convert from 24 hour to normal hour.
		c_hour = (c_hour > 12 || c_hour == 0 ? abs(c_hour - 12) : c_hour);
		// turn off the previous hour.
		//printf("c_hour: [%d]\n", c_hour);
		if (c_hour != last_hour)
		{
			digitalWrite(hours[last_hour], LOW);
			last_hour = c_hour;
		}
		digitalWrite(hours[c_hour - 1], HIGH);
		//printf("hours: [%d]\n", hours[abs(c_hour - 12) + 1]);
		//printf("hours abs: [%d]\n", abs(c_hour - 12) + 1);
		//
		// MINUTES
		//
		// turn off everything
		// 0
		if ((c_min == 59) || (c_min >= 0 && c_min < 4))
		{
			digitalWrite(4, LOW);
			digitalWrite(MY_8574 + 0, 0);
		}
		// 5
		if (c_min > 3 && c_min < 8)
		{
			digitalWrite(MY_8574 + 0, 1);
			digitalWrite(MY_8574 + 5, 0);
		}
		// 10
		if (c_min > 9 && c_min < 13)
		{
			digitalWrite(MY_8574 + 5, 1);
			digitalWrite(MY_8574 + 6, 0);
		}
		// 15
		if (c_min > 14 && c_min < 17)
		{
			digitalWrite(MY_8574 + 6, 1);
			digitalWrite(0, HIGH);
		}
		// 20
		if (c_min > 18 && c_min < 23)
		{
			digitalWrite(0, LOW);
			digitalWrite(MY_8574 + 4, 0);
		}
		// 25
		if (c_min > 24 && c_min < 28)
		{
			digitalWrite(MY_8574 + 4, 1);
			digitalWrite(MY_8574 + 3, 0);
		}
		// 30
		if (c_min > 29 && c_min < 33)
		{
			digitalWrite(MY_8574 + 3, 1);
			digitalWrite(1, HIGH);
		}
		// 35
		if (c_min > 34 && c_min < 37)
		{
			digitalWrite(1, LOW);
			digitalWrite(2, HIGH);
		}
		// 40
		if (c_min > 38 && c_min < 43)
		{
			digitalWrite(2, LOW);
			digitalWrite(3, HIGH);
		}
		// 45
		if (c_min > 44 && c_min < 47)
		{
			digitalWrite(3, LOW);
			digitalWrite(MY_8574 + 2, 0);
		}
		// 50
		if (c_min > 48 && c_min < 53)
		{
			digitalWrite(MY_8574 + 2, 1);
			digitalWrite(MY_8574 + 1, 0);
		}
		// 55
		if (c_min > 54 && c_min < 58)
		{
			digitalWrite(MY_8574 + 1, 1);
			digitalWrite(4, HIGH);
		}
		delay(1000);
	}
	return 0;
}


