#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <lcd.h>

#define TRUE 1

const int RS = 3;
const int EN = 14;
const int D0 = 4;
const int D1 = 12;
const int D2 = 13;
const int D3 = 6;

int getCM(int c_i)
{
  int distance;
	// Send trig pulse
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIG, LOW);
	// Wait for echo start
	while (digitalRead(ECHO) == LOW);
	// Wait for echo end
	long startTime = micros();
	while (digitalRead(ECHO) == HIGH);
	long travelTime = micros() - startTime;
	// Get distance in cm
	 distance = travelTime / (c_i == 1 ? 58 : 148);
	return distance;
}

int main(int argc, char **argv)
{
	char buf[32];
	int lcdFD;
	int i;

	wiringPiSetup();
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);
	// TRIG pin must start LOW
	digitalWrite(TRIG, LOW);
	delay(30);
	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	lcdPosition(lcdFD, 0,0);
	while (1)
	{
		//printf("Distance: %dcm\n", getCM(2));
		//sprintf(buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
		lcdPosition(lcdFD, 8, 0);
		for (i = 0; i < 8; i++) lcdPuts(lcdFD, " ");
		sprintf(buf, "Inches: %d", getCM(2));
		lcdPosition(lcdFD, 0, 0);
		lcdPuts(lcdFD, buf);
		sprintf(buf, "Centim: %d", getCM(1));
		lcdPosition(lcdFD, 8, 1);
		for (i = 0; i < 8; i++) lcdPuts(lcdFD, " ");
		lcdPosition(lcdFD, 0, 1);
		lcdPuts(lcdFD, buf);
		delay(250);
	}
	return EXIT_SUCCESS;
}
