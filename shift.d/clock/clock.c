#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <wiringPi.h>
#include <sr595.h>
//#include <wiringShift.h>

int hourDataPin = 3;
int hourLatchPin = 4;
int hourClockPin = 5;
int hourAddr = 100;

int minDataPin = 0;
int minLatchPin = 1;
int minClockPin = 2;
int minAddr = 200;

const int ampmPin = 6;

int main(int argc, char **argv)
{
	struct tm *t;
	time_t tim;
	int last_hour = 14;
	int c_hour;
	int c_min;
	int prev_hour;
	int bits = 12;

	wiringPiSetup();
	sr595Setup(hourAddr, bits, hourDataPin, hourClockPin, hourLatchPin);
	sr595Setup(minAddr, bits, minDataPin, minClockPin, minLatchPin);
	pinMode(ampmPin, OUTPUT);
	while (1)
	{
		tim = time(NULL);
		t = localtime(&tim);
		c_hour = t->tm_hour;
		c_min = t->tm_min;
    		if (last_hour == 14) last_hour = c_hour;
		// am / pm
		digitalWrite(ampmPin, (c_hour >= 12 ? HIGH : LOW));
		// convert 24 hour to 12 hour
		c_hour = (c_hour > 12 || c_hour == 0 ? abs(c_hour - 12) : c_hour);
		// turn off the previous hour
		if (c_hour != last_hour)
		{
			prev_hour = c_hour - 1;
			if (prev_hour == 0) prev_hour = 12;
			digitalWrite(hourAddr + (prev_hour - 1), LOW);
			last_hour = c_hour;
		}
		// turn on current hour
		digitalWrite(hourAddr + (c_hour - 1), HIGH);
		// MINUTES
		// 0
		if ((c_min == 59) || (c_min >= 0 && c_min < 4)) { digitalWrite(minAddr + 10, LOW); digitalWrite(minAddr + 11, HIGH); }
		// 5
		if (c_min > 3 && c_min < 8) { digitalWrite(minAddr + 11, LOW); digitalWrite(minAddr + 0, HIGH); }
		// 10
		if (c_min > 9 && c_min < 13) { digitalWrite(minAddr + 0, LOW); digitalWrite(minAddr + 1, HIGH); }
		// 15
		if (c_min > 14 && c_min < 17) { digitalWrite(minAddr + 1, LOW); digitalWrite(minAddr + 2, HIGH); }
		// 20
		if (c_min > 18 && c_min < 23) { digitalWrite(minAddr + 2, LOW); digitalWrite(minAddr + 3, HIGH); }
		// 25
		if (c_min > 24 && c_min < 28) { digitalWrite(minAddr + 3, LOW); digitalWrite(minAddr + 4, HIGH); }
		// 30
		if (c_min > 29 && c_min < 33) { digitalWrite(minAddr + 4, LOW); digitalWrite(minAddr + 5, HIGH); }
		// 35
		if (c_min > 34 && c_min < 37) { digitalWrite(minAddr + 5, LOW); digitalWrite(minAddr + 6, HIGH); }
		// 40
		if (c_min > 38 && c_min < 43) { digitalWrite(minAddr + 6, LOW); digitalWrite(minAddr + 7, HIGH); }
		// 45
		if (c_min > 44 && c_min < 47) { digitalWrite(minAddr + 7, LOW); digitalWrite(minAddr + 8, HIGH); }
		// 50
		if (c_min > 48 && c_min < 53) { digitalWrite(minAddr + 8, LOW); digitalWrite(minAddr + 9, HIGH); }
		// 55
		if (c_min > 54 && c_min < 58) { digitalWrite(minAddr + 9, LOW); digitalWrite(minAddr + 10, HIGH); }
		delay(1000);
	}
	return 0;
}
