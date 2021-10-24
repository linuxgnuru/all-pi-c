/*
             define from  wiringPi.h                     define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
                8/SDA | | 5V               ->                  SDA | | 5V
                9/SCL | | GND              ->                  SCL | | GND
                    7 | | 14/TX            ->                  IO7 | | TX
                  GND | | 15/RX            ->                  GND | | RX
                    0 | | 18               ->                  IO0 | | IO1
                    2 | | GND              ->                  IO2 | | GND
                    3 | | 23               ->                  IO3 | | IO4
                  VCC | | 24               ->                  VCC | | IO5
              MOSI/12 | | GND              ->                 MOSI | | GND
              MISO/13 | | 25               ->                 MISO | | IO6
               SCK/14 | | 8/CE0            ->                  SCK | | CE0
                  GND | | 9/CE19           ->                  GND | | CE1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include<wiringPi.h>
#include<lcd.h>

const int RS = 3;	//
const int EN = 14;	//
const int D0 = 4;	//
const int D1 = 12;	//
const int D2 = 13;	//
const int D3 = 6;	//

int main(int argc, char *argv[])
{
	char msg[17];
	int i, j, k;
	int lcdFD;

	// try to connect to display
	if (wiringPiSetup() < 0)
	{
	    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
	    return 1;
	}
	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	// clear LCD
	lcdClear(lcdFD);
	lcdCursor(lcdFD, 1);
	lcdCursorBlink(lcdFD, 1);
	for (i = 9; i >= 0; i--)
	{
		for (j = 0; j < 8; j++)
		{
			lcdPosition(lcdFD, j,0);
 			lcdPrintf(lcdFD, "%d", i);
			lcdPosition(lcdFD, j-1, 0);
 			lcdPrintf(lcdFD, " ");
			usleep(3000);
		}
		if (i != 9)
		{
			for (j = 0; j < 8; j++)
			{
				lcdPosition(lcdFD, j,1);
				lcdPrintf(lcdFD, "%d", i + 1);
				lcdPosition(lcdFD, j-1,1);
	 			lcdPrintf(lcdFD, " ");
				usleep(3000);
			}
		}
		sleep(1);
	}
	lcdPosition(lcdFD, 0,0);
	lcdPrintf(lcdFD, "BOOOOOOOOOOOOOOO");
	lcdPosition(lcdFD, 0,1);
	lcdPrintf(lcdFD, "OOOOOOOOOOOOOOM!");
	return 0;
}
