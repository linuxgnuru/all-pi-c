#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wiringPi.h>

#define LOCK_KEY 0

PI_THREAD (CheckButtonThread)
{
	piHiPri(50);
	if (digitalRead(3) == 0)
	{
		digitalWrite(21, HIGH);
	}
	else
	{
		digitalWrite(21, LOW);
	}
}

int main(int argc, char **argv)
{
	int i;
	int ret;

	wiringPiSetup();
	ret = piThreadCreate(CheckButtonThread);
	if (argc > 1)
	{
		printf("yay!\n");
	}
	else
	{
		printf("usage: %s fu\n", argv[0]);
		return 1;
	}
	i = 0;
	while (1)
	{
		if (i % 100 == 0)
			printf("i: %d\n", i);
		piLock(LOCK_KEY);
		i++;
		piUnlock(LOCK_KEY);
	}
	return 0;
}
