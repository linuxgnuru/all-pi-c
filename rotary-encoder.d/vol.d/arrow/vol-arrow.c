#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

static unsigned char head[8] = 
{
	0b10000,
	0b11100,
	0b11110,
	0b11111,
	0b11111,
	0b11100,
	0b11000,
	0b10000,
};

static unsigned char body[8] = 
{
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
};

// Global lcd handle:
static int lcdHandle;

int main (int argc, char **argv)
{
	wiringPiSetup();
	lcdHandle = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	if (lcdHandle < 0)
	{
		fprintf(stderr, "%s: lcdInit failed\n", argv[0]);
		return -1;
	}
	/*
	strobe(lcdHandle);
	*/
#if 0
	lcdCharDef(lcdHandle, 2, musicNote);
	lcdPosition(lcdHandle, 0, 0);
	lcdPutchar(lcdHandle, 2);

	lcdCharDef(lcdHandle, 6, h_lines);
	lcdPosition(lcdHandle, 4, 0);
	lcdPutchar(lcdHandle, 6);
#endif
	return 0;
}
