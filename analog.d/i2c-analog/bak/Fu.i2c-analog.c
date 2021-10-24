#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <ncurses.h>

int rev(int val) { return abs(val - 255); }

int main(int argc, char **argv)
{
	int r;
	int fd;
	unsigned char value[4];
	useconds_t delay = 2000;
	char *dev = "/dev/i2c-1";
	int addr = 0x48;
	int i, j, k, key, new_val, val_mod, ctr, nb, flag;

	flag = 0;
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(5, 0, "Brightness");
	mvaddstr(5, 11, "[");
	mvaddstr(5, 67, "]");

	/*
	 * 10
	 */
	ctr = 0;
	for (i = 0; i < 64; i++)
	{
		val_mod = i % 10;
		move(3, i + 12);
		if (val_mod == 0)
		{
			if (ctr != 0)
			{
				nb = ctr % 10;
				printw("%d", nb);
			}
			ctr++;
		}
		else if (i % 10 == 0)
		{
			addch('1');
		}
	}
	/*
	 * 0-9
	 */
	for (i = 0; i < 64; i++)
	{
		val_mod = i % 10;
		move(4, i + 12);
		if (val_mod == 0 || val_mod == 5)
		{
			printw("%d", val_mod);
		}
	}
	refresh();
	fd = open(dev, O_RDWR);
	if (fd < 0)
	{
		perror("Opening i2c device node\n");
		return 1;
	}
	r = ioctl(fd, I2C_SLAVE, addr);
	if (r < 0) perror("Selecting i2c device\n");
	while (1)
	{
		usleep(delay);
		r = read(fd, &value[0], 1);
		if (r != 1) perror("reading i2c device\n");
		new_val = rev(value[0]); // Reverse the output; i.e. 255 becomes 0
		new_val /= 4;
		usleep(delay);
		move(5, 12);
		for (i = 0; i < 64; i++)
		{
			//if (i < new_val) { printw("%d", new_val); }
			if (i < new_val) addch('*');
			else addch(' ');
		}
		refresh();
		key = getch();
		if (key > -1) break;
	}
	endwin();
	close(fd);
	//printf("%d\n", key);
	return(0);
}
