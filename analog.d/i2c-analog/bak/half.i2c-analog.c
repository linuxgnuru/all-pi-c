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
	int addr = 0x48;
	int j;
	int key;
	int new_val;
	int fu;
	int bar;
	int nb;
	int flag;
	int k;
	int wiggins;

	flag = 0;
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(5, 0, "Brightness");
	/*
	for (j = 0; j < 64; j++)
	{
		fu = j % 10;
		move(6, j + 12);
		if (fu == 0)
			printw("%d", fu);
	}
	*/
	bar = flag = 0;
	k = 1;
	for (j = 0; j < 64; j++)
	{
		fu = j % 10;
		move(3, j + 12);
		if (fu == 0)
		{
			if (bar != 0)
			{
				nb = bar % 10;
				printw("%d", nb);
			}
			bar++;
		}
		/*
		else
		{
			wiggins = j % 10;
			if (wiggins == 0 || wiggins == 5)
			{
				printw("%d", k);
			}
			k++;
		}
		*/
	}
	for (j = 0; j < 64; j++)
	{
		fu = j % 10;
		move(4, j + 12);
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
	mvaddstr(5, 66, "]");
	while (1)
	{
		//command[0] = 0x40 | (1 & 0x03); // output enable | read input i
		command[0] = 1 & 0x03; // read input i
		//r = write(fd, &command, 2);
		usleep(delay);
		// the read is always one step behind the selected input
		r = read(fd, &value[0], 1);
		new_val = rev(value[0]);
		if (r != 1) { perror("reading i2c device\n"); }
		usleep(delay);
		//value[0] /= 4;
		new_val /= 4;
		move(5, 12);
		for (j = 0; j < 64; j++)
		{
			if (j < new_val) { addch('*'); }
			//if (j < new_val) { printw("%d", new_val); }
			else { addch(' '); }
		}
		refresh();
		//key = getch();
		//if (key == 43) { command[1]++; }
		//else if (key == 45) { command[1]--; }
		//else if (key > -1) { break; }
	}
	endwin();
	close(fd);
//	printf("%d\n", key);
	return(0);
}
