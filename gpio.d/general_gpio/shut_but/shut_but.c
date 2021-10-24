#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include <wiringPi.h>
const int okPin = 21;

int main(int argc, char **argv)
{
	int i, j;

	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	pinMode(okPin, OUTPUT);
	digitalWrite(okPin, LOW);
	pinMode(26, INPUT);
	while (1)
	{
		if (digitalRead(26) == LOW)
		{
			digitalWrite(okPin, HIGH);
			system("shutdown -h now");
		}
		else
			digitalWrite(okPin, LOW);
	}
	return EXIT_SUCCESS;
}
