#include <stdio.h>
#include <stdlib.h>

#include "keypad_func.h"

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
