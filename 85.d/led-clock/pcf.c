#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>

#define MY_8574 100

const static int hours[] = { 29, 5, 6, 7, 21, 22, 23, 24, 25, 26, 27, 28 };

int main(int argc, char **argv)
{
	int i;

	wiringPiSetup();
	pcf8574Setup(MY_8574, 0x20);
	//digitalWrite(MY_8574 + i, 0);
	for (i = 0; i < 7; i++) pinMode(i, OUTPUT);
	for (i = 21; i < 30; i++) pinMode(i, OUTPUT);
	digitalWrite(MY_8574, 0);
	return 0;
}


