#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include<wiringPi.h>
#include<lcd.h>

int main(void)
{
	int lcdFD;
	if (wiringPiSetup() < 0)
	{
	    fprintf(stderr, "Unable to open serial device: %s\n", strerror (errno));
	    return 1;
	}
	lcdFD = lcdInit(2, 16, 4, 3, 14, 4, 12, 13, 6, 4, 12, 13, 6);
	lcdClear(lcdFD);
	return 0;
}
