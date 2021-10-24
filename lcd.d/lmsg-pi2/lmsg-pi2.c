#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <lcd.h>

const int RS = 14;	// SCK (SPI)
const int EN =  3;	// GPIO 3
const int D0 = 12;	// MOSI (SPI)
const int D1 =  6;	// GPIO 6
const int D2 =  5;	// GPIO 5
const int D3 =  4;	// GPIO 4

// catch signals and clean up GPIO
static void die(int sig)
{
	int i;
	for (i = 0; i < 8; i++)
	       	digitalWrite(i, LOW);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	char msg1[16];
	char msg2[16];
	int len;

	// first make sure there is something to type
	if (argc <= 1)
	{
	    //fprintf (stderr, "Usage: %s\n", strerror (errno)) ;
	    printf ("Usage:\n %s [ LINE1 ] ([ LINE2 ])\nMaximum 16 characters and LINE2 is optional\n", argv[0]);
	    return 1;
	}
	// test for string lengths
	len = strlen(argv[1]);
	if (len > 16)
	{
		printf("Length of message is too long.\n");
	    	return 1;
	}
	else if ((argc == 3) && (strlen(argv[2]) > 16))
	{
		printf("Length of second message is too long.\n");
	    	return 1;
	}
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	(void)signal(SIGTERM, die);
	// try to connect to display
	if (wiringPiSetup() < 0)
	{
	    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
	    return 1 ;
	}
	int lcdFD;
	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	strncpy(msg1, argv[1], 16);
	lcdPosition(lcdFD, 0,0);
	lcdPrintf(lcdFD, msg1);
	if (argc == 3)
	{
		strcpy(msg2, argv[2]);
		lcdPosition(lcdFD, 0, 1);
		lcdPrintf(lcdFD, msg2);
	}
}


