#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include <wiringPi.h>
#include <sr595.h>
//#include <wiringShift.h>

const int dataPin = 7;
const int latchPin = 21;
const int clockPin = 22;

const int mx = 24;

// catch signals and clean up GPIO
static void die(int sig)
{
  if (sig != 0 && sig != 2)
      (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
  if (sig == 2)
      (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
  exit(0);
}

int main(int argc, char **argv)
{
	int i, bit;

  // note: we're assuming BSD-style reliable signals here
  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
  (void)signal(SIGTERM, die);
  if (wiringPiSetup() == -1)
  {
    fprintf(stdout, "oops: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
	sr595Setup(100, mx, dataPin, clockPin, latchPin);
	//printf("Raspberry Pi Shift Register\n");
	while (1)
	{
        //printf("up\n");
		for (i = 0; i < mx; i++)
		{
			digitalWrite(100 + i, 1);
			delay(50);
		}
		for (i = mx - 1; i > -1; i--)
		{
			digitalWrite(100 + i, 0);
			delay(50);
		}
        //printf("down\n");
		//delay(50);
		for (i = mx - 1; i > -1; i--)
		{
			digitalWrite(100 + i, 1);
			delay(50);
		}
		for (i = 0; i < mx; i++)
		{
			digitalWrite(100 + i, 0);
			delay(50);
		}
	}
/*
	for (i = 0; i < 1024; ++i)
	{
		for (bit = 0; bit < mx; ++bit)
			digitalWrite(100 + bit, i & (i << bit));
		delay(200);
	}
	for (bit = 0; bit < mx; ++bit)
		digitalWrite(100 + bit, i & (i << bit));
	while (1)
	{
		for (i = 0; i < 1024; ++i)
		{
			for (bit = 0; bit < mx; ++bit)
				digitalWrite(100 + bit, i & (i << bit));
			delay(5);
		}
	}
*/
	return 0;
}
