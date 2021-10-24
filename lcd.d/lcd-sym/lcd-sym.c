#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>

#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <lcd.h>

const int RS = 3;	//
const int EN = 14;	// 
const int D0 = 4;	// 
const int D1 = 12;	// 
const int D2 = 13;	// 
const int D3 = 6;	// 

// Musical note
static unsigned char musicNote[8] = 
{
	0b01111,
	0b01001,
	0b01001,
	0b11001,
	0b11011,
	0b00011,
	0b00000,
	0b00000,
};

static unsigned char mario[8] = 
{
	0b00111,
	0b01010,
	0b00100,
	0b01111,
	0b10100,
	0b01010,
	0b10001,
	0b10001,
};

static unsigned char grid[8] = 
{
	0b11111,
	0b10101,
	0b11111,
	0b10101,
	0b11111,
	0b10101,
	0b11111,
	0b10101,
};

static unsigned char v_lines[8] = 
{
	0b10101,
	0b10101,
	0b10101,
	0b10101,
	0b10101,
	0b10101,
	0b10101,
	0b10101,
};

static unsigned char h_lines[8] = 
{
	0b11111,
	0b00000,
	0b11111,
	0b00000,
	0b11111,
	0b00000,
	0b11111,
	0b00000,
};

// Global lcd handle:
static int lcdHandle;

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

int main (int argc, char **argv)
{
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	lcdHandle = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	if (lcdHandle < 0)
	{
		fprintf(stderr, "%s: lcdInit failed\n", argv[0]);
		return -1;
	}
	/*
	strobe(lcdHandle);
	*/

	lcdCharDef(lcdHandle, 2, musicNote);
	lcdPosition(lcdHandle, 0, 0);
	lcdPutchar(lcdHandle, 2);

	lcdCharDef(lcdHandle, 3, mario);
	lcdPosition(lcdHandle, 1, 0);
	lcdPutchar(lcdHandle, 3);

	lcdCharDef(lcdHandle, 4, grid);
	lcdPosition(lcdHandle, 2, 0);
	lcdPutchar(lcdHandle, 4);

	lcdCharDef(lcdHandle, 5, v_lines);
	lcdPosition(lcdHandle, 3, 0);
	lcdPutchar(lcdHandle, 5);

	lcdCharDef(lcdHandle, 6, h_lines);
	lcdPosition(lcdHandle, 4, 0);
	lcdPutchar(lcdHandle, 6);
/*
	lcdPosition(lcdHandle, 1, 0);
	lcdPuts(lcdHandle, cur_song.title);
  	lcdClear(lcdHandle);
*/
	return 0;
}
