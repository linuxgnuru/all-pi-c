#include <stdio.h>
#include <string.h>

#include <wiringPi.h>

#define G_POUT 22

int main(int argc, char **argv)
{
	int gip = 29;
	int myD = 500;

	if (argc == 2)
		gip = atoi(argv[1]);
	/*
	else
	{
		printf("usage: %s [input pin]\n", argv[0]);
		return 1;
	}
	*/
	wiringPiSetup();
	pinMode(gip, INPUT);
	pullUpDnControl(gip, PUD_UP);
	pinMode(G_POUT, OUTPUT);
	for (;;)
	{
		digitalWrite(G_POUT, !digitalRead(gip));
		//delay(500);
	}
	return 0;
}
