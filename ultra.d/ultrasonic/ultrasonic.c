#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wiringPi.h>

#define IN 1
#define CM 2

#define TRIG 23
#define ECHO 24

int getDist(int ic)
{
	digitalWrite(TRIG, HIGH); // Send trig pulse
	delayMicroseconds(20);
	digitalWrite(TRIG, LOW);
	while (digitalRead(ECHO) == LOW); // Wait for echo start
	long startTime = micros(); // Wait for echo end
	while (digitalRead(ECHO) == HIGH);
	long travelTime = micros() - startTime;
       	// Get distance 58 = cm, 37 = inches
	return travelTime / (ic == CM ? 58 : 37);
}

int main(int argc, char **argv)
{
	int ic = CM;

	if (argc > 1)
	{
		if (strcmp(argv[1], "-i") == 0)
			ic = IN;
	}
	wiringPiSetup();
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);
	digitalWrite(TRIG, LOW); // TRIG pin must start LOW
	delay(30);
	printf("Distance: %d%s\n", getDist(ic), (ic == IN ? "in" : "cm"));
	return EXIT_SUCCESS;
}
