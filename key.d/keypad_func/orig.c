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

int ROW[] = { 0, 1, 2, 3 };
int COL[] = { 4, 5, 6, 7 };

void init_keypad(int row[4], int col[4])
{
    int i;
	wiringPiSetup();
	for (i = 0; i < 4; i++)
    {
        COL[i] = col[i];
        ROW[i] = row[i];
		pinMode(COL[i], OUTPUT);
		pinMode(ROW[i], INPUT);
		pullUpDnControl(ROW[i], PUD_UP);
	}
}
char get_keypad()
{
    int i, j;
    char ret;

	for (i = 0; i < 4; i++)
	{
		digitalWrite(COL[i], LOW);
		for (j = 0; j < 4; j++)
		{
			if (digitalRead(ROW[j]) == LOW)
			{
				ret = MATRIX[j][i];
				while (digitalRead(ROW[j]) == LOW)
					delay(20);
			}
		}
		digitalWrite(COL[i], HIGH);
	}
    if (ret == 190)
        ret = 0;
    return ret;
}

int main(int argc, char **argv)
{
    int myRow[] = { 0, 1, 2, 3 };
    int myCol[] = { 4, 5, 6, 7 };
    char key;

    init_keypad(myRow, myCol);
	while (1)
	{
        key = get_keypad();
        if (key != 0)
        {
            printf("%c\n", key);
        }
	}
	return 0;
}
