#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <wiringPi.h>
#include <lcd.h>

#define MAXSIZE 103

static const char *spaces = "                    ";

static int lcdHandle;

const int RS = 3;	//
const int EN = 14;	//
const int D0 = 4;	//
const int D1 = 12;	//
const int D2 = 13;	//
const int D3 = 6;	//

int usage(const char *progName)
{
	fprintf(stderr, "Usage: %s [message]\n", progName);
	return EXIT_FAILURE;
}

void scrollMessage(int line, int width, char mymsg[MAXSIZE])
{
	char buf[32];
	static int position = 0;
	static int timer = 0;

	if (millis() < timer)
		return;
	timer = millis() + 200;
	strncpy(buf, &mymsg[position], width);
	buf[width] = 0;
	lcdPosition(lcdHandle, 0, line);
	lcdPuts(lcdHandle, buf);
	if (++position == (strlen(mymsg) - width))
		position = 0;
}

int main(int argc, char **argv)
{
	int cols = 16;
	char mymsg[MAXSIZE];

	if (argc != 2)
		return usage(argv[0]);
	else
	{
		strcpy(mymsg, spaces);
		strncat(mymsg, argv[1], strlen(argv[1]) + 1);
		strcat(mymsg, spaces);
	}
	mymsg[strlen(mymsg) + 1] = 0;
	if (wiringPiSetup() < 0)
	{
		fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
		return 1;
	}
	lcdHandle = lcdInit(2, cols, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	if (lcdHandle < 0)
	{
		fprintf(stderr, "%s: lcdInit failed\n", argv[0]);
		return -1;
	}
	//printf("Raspberry Pi LCD test\n");
	//printf("=====================\n");
	for (;;)
		scrollMessage(0, cols, mymsg);
	return 0;
}
