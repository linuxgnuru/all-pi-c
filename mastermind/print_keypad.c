#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <wiringPi.h>

const char MATRIX[4][4] = {
			{ '1','2','3','A' },
			{ '4','5','6','B' },
			{ '7','8','9','C' },
			{ '*','0','#','D' }
	       	};

const int ROW[] = { 0, 1, 2, 3 };
const int COL[] = { 4, 5, 6, 7 };

static void die(int sig)
{
	if (sig != 0)
		(void)fprintf(stderr, "caught signal %d\n", sig);
	exit(0);
}

void usage(char *arg)
{
	printf("%s: (no args)\n"
		"Pin out: (from left to right on 4x4 keypad)\n"
		"GPIO\tBCM\n"
		"0\t17\n1\t18\n2\t27\n3\t22\n"
		"4\t23\n5\t24\n6\t25\n7\t4\n"
		"WARNING: The first 4 are all using the internal pull-up resistors\n"
		"         so keep this in mind or use your own resistors to be safe.\n"
		, arg);
	exit(0);
}

int main(int argc, char **argv)
{
	int i, j;

	if (argc > 1)
	{
		usage(argv[0]);
	}
	wiringPiSetup();
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	for (i = 0; i < 4; i++)
		pinMode(COL[i], OUTPUT);
	for (i = 0; i < 4; i++)
	{
		pinMode(ROW[i], INPUT);
		pullUpDnControl(ROW[i], PUD_UP);
	}
	while (1)
	{
		for (i = 0; i < 4; i++)
		{
			digitalWrite(COL[i], LOW);
			for (j = 0; j < 4; j++)
			{
				if (digitalRead(ROW[j]) == LOW)
				{
					printf("%c\n", MATRIX[j][i]);
					while (digitalRead(ROW[j]) == LOW)
						delay(20);
				}
			}
			digitalWrite(COL[i], HIGH);
		}
	}
	return 0;
}
