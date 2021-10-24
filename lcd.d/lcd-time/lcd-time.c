#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include <wiringPi.h>
#include <lcd.h>

const int RS = 3;	//
const int EN = 14;	//
const int D0 = 4;	//
const int D1 = 12;	//
const int D2 = 13;	//
const int D3 = 6;	//

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

int main(int argc, char **argv)
{
	char buf[32];
	int lcdFD;
	struct tm *t;
	time_t tim;

	if (wiringPiSetup() < 0)
	{
	    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
	    return 1;
	}
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	(void)signal(SIGTERM, die);
	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	lcdPosition(lcdFD, 0, 0);
	while (1)
	{
		tim = time(NULL);
		t = localtime(&tim);
		sprintf(buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
		lcdPosition(lcdFD, 4, 0);
		lcdPuts(lcdFD, buf);
		sprintf(buf, "%02d/%02d/%02d", t->tm_mday, t->tm_mon + 1, t->tm_year-100);
		lcdPosition(lcdFD, 4, 1);
		lcdPuts(lcdFD, buf);
	}
	return 0;
}
//sprintf(buf, "%02d:%02d:%02d %02d/%02d/%02d", t->tm_hour, t->tm_min, t->tm_sec, t->tm_mday, t->tm_mon + 1, t->tm_year-100);
//sprintf(buf, "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year+1900);
