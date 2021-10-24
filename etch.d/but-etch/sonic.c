#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>

#define TRUE 1

#define TRIG 23
#define ECHO 24


int getCM()
{
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
	int distance = travelTime / 58;
	return distance;
}

int main(int argc, char **argv)
{
	wiringPiSetup();
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);
	// TRIG pin must start LOW
	digitalWrite(TRIG, LOW);
	delay(30);
	printf("Distance: %dcm\n", getCM());
	return EXIT_SUCCESS;
}
