#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <lcd.h>

#include "bounce.h"

const int RS = 3;	//
const int EN = 14;	// 
const int D0 = 4;	// 
const int D1 = 12;	// 
const int D2 = 13;	// 
const int D3 = 6;	// 

// Global lcd handle:
static int lcdHandle;

int main (int argc, char **argv)
{
	int i;

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
	for (i = 0; i < 8; i++)
	{
		switch (i)
		{
			case  0: lcdCharDef(lcdHandle, 2, b1); break;
			case  1: lcdCharDef(lcdHandle, 3, b2); break;
			case  2: lcdCharDef(lcdHandle, 4, b3); break;
			case  3: lcdCharDef(lcdHandle, 5, b4); break;
			case  4: lcdCharDef(lcdHandle, 6, b5); break;
			case  5: lcdCharDef(lcdHandle, 7, b6); break;
			case  6: lcdCharDef(lcdHandle, 8, b7); break;
			case  7: lcdCharDef(lcdHandle, 9, b8); break;
//			case  8: lcdCharDef(lcdHandle, 10, b9); break;
//			case  9: lcdCharDef(lcdHandle, 11, b10); break;
//			case 10: lcdCharDef(lcdHandle, 12, b11); break;
//			case 11: lcdCharDef(lcdHandle, 13, b12); break;
//			case 12: lcdCharDef(lcdHandle, 14, b13); break;
//			case 13: lcdCharDef(lcdHandle, 15, b14); break;
//			case 14: lcdCharDef(lcdHandle, , b15); break;
//			case 15: lcdCharDef(lcdHandle, (i + 2), b16); break;
//			default: lcdCharDef(lcdHandle, (i + 2), b1); break;
		}
		lcdPosition(lcdHandle, i, 0);
		lcdPutchar(lcdHandle, (i + 2));
	}
/*
	lcdPosition(lcdHandle, 1, 0);
	lcdPuts(lcdHandle, cur_song.title);
  	lcdClear(lcdHandle);
*/
	return 0;
}
