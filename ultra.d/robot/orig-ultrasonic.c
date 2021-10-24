#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>

#define TRIG 23
#define ECHO 24

int getDist()
{
	digitalWrite(TRIG, HIGH); // Send trig pulse
	delayMicroseconds(20);
	digitalWrite(TRIG, LOW);
	while (digitalRead(ECHO) == LOW); // Wait for echo start
	long startTime = micros(); // Wait for echo end
	while (digitalRead(ECHO) == HIGH);
	long travelTime = micros() - startTime;
       	// Get distance 58 = cm, 37 = inches
       	int distance = travelTime / 58;
	return distance;
}

int main(int argc, char **argv)
{
	wiringPiSetup();
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);
	digitalWrite(TRIG, LOW); // TRIG pin must start LOW
	delay(30);
	printf("Distance: %dcm\n", getDist());
	return EXIT_SUCCESS;
}
