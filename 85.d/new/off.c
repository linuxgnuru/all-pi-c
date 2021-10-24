#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>

#define MY_8574 100

int main(int argc, char **argv)
{
	int i;

	wiringPiSetup();
	pcf8574Setup(MY_8574, 0x20);
	for (i = 0; i < 8; i++)
	{
		//printf("%d\n", digitalRead(MY_8574 + i));
		digitalWrite(MY_8574 + i, 1);
	}
	return 0;
}
