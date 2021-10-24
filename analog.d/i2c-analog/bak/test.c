#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <ncurses.h>

int main(int argc, char **argv)
{
	int cnt, i, val_mod, nb;
	int r, key;
	unsigned char value[4];
	useconds_t delay = 2000;

	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);

	cnt = 0;
	for (i = 0; i < 128; i++)
	{
		val_mod = i % 10;
		move(3, i + 12);
		if (val_mod == 0)
		{
			if (cnt != 0)
			{
				nb = cnt % 10;
				printw("%d", nb);
			}
			cnt++;
		}
		else if (i % 100 == 0 || i % 10 == 0)
		{
			addch('1');
		}
		move(2, 0);
		printw("i 100: %d i 10: %d", i % 100, i % 10);
	}
	refresh();
	/*
	while (1)
	{
		key = getch();
		if (key > -1) break;
	}
	*/
	endwin();
	return 0;
}
