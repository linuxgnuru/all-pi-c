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
	unsigned char command[2];
	unsigned char value[4];
	useconds_t delay = 2000;
	char *dev = "/dev/i2c-1";
	int addr = 0x4d;
	int i, j, k, key, new_val, fu, bar, nb, flag;
	int newR;

	flag = 0;
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(5, 0, "Brightness");

	/*
	 * 100
	 */
	bar = 0;
	for (i = 0; i < 128; i++)
	{
		fu = i % 100;
		move(2, i + 12);
		if (fu == 0 || flag == 1)
		{
			if (flag != 1)
			{
				if (bar != 0)
				{
					printw("%d", bar);
					flag = 1;
				}
				bar++;
			}
			else
			{
				j = i % 10;
				if (j == 0 || j == 5)
					addch('1');
			}
		}
	}
	/*
	 * 10
	 */
	bar = 0;
	for (i = 0; i < 128; i++)
	{
		fu = i % 10;
		move(3, i + 12);
		if (fu == 0)
		{
			if (bar != 0)
			{
				nb = bar % 10;
				printw("%d", nb);
			}
			bar++;
		}
		else if (i % 100 == 0 || i % 10 == 0)
			addch('1');
	}
	for (i = 0; i < 128; i++)
	{
		fu = i % 10;
		move(4, i + 12);
		if (fu == 0 || fu == 5)
			printw("%d", fu);
	}
	refresh();
	fd = open(dev, O_RDWR);
	if (fd < 0)
	{
		perror("Opening i2c device node\n");
		return 1;
	}
	r = ioctl(fd, I2C_SLAVE, addr);
	if (r < 0) { perror("Selecting i2c device\n"); }
	command[1] = 0;
	/*
	command[0] = 1 & 0x03; // read input i
	usleep(delay);
	r = read(fd, &value[0], 1);
	printf("%4d\n", value[0]);
	*/
	mvaddstr(5, 11, "[");
	mvaddstr(5, 140, "]");
	while (1)
	{
		for (k = 0; k < 2; k++)
		{
			//command[0] = 0x40 | (1 & 0x03); // output enable | read input i
			command[0] = 1 & 0x03; // read input i
			//r = write(fd, &command, 2);
			usleep(delay);
			// the read is always one step behind the selected input
			r = read(fd, &value[k], 1);
			if (r != 1) { perror("reading i2c device\n"); }
			new_val = rev(value[k]);
			//value[0] /= 4;
			new_val /= 2;
			usleep(delay);
			move(, 12);
			for (i = 0; i < 128; i++)
			{
				if (i < new_val) { addch('*'); }
				//if (j < new_val) { printw("%d", new_val); }
				else { addch(' '); }
			}
		}
		refresh();
		key = getch();
		if (key == 43) { command[1]++; }
		else if (key == 45) { command[1]--; }
		else if (key > -1) { break; }
	}
	endwin();
	close(fd);
	printf("%d\n", key);
	return(0);
}
