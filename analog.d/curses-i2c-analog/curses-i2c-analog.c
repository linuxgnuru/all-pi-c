#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <ncurses.h>

int rev(int val)
{
	return abs(val - 255);
}

int main(int argc, char **argv)
{
	int fd;
	int r;
	unsigned char value[4];
	unsigned char command[2];
	useconds_t delay = 20000;
	char *dev = "/dev/i2c-1";
	int addr = 0x49;
	int i, j, k, key, new_val, val_mod, cnt, nb, flag;
	int newR;
	int n;
	int num_sensors = 4;

	if (argc > 1)
	{
		num_sensors = atoi(argv[1]);
		if (num_sensors < 1 || num_sensors > 4)
		{
			printf("Error: number must be between 1 and 4\n");
			return(-1);
		}
	}
	flag = 0;
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(2, 0, "Analog output");
	refresh();
	fd = open(dev, O_RDWR);
	if (fd < 0)
	{
		perror("Opening i2c device node\n");
		return 1;
	}
	r = ioctl(fd, I2C_SLAVE, addr);
	if (r < 0)
	{
		perror("Selecting i2c device\n");
	}
	command[1] = 0;
	while (1)
	{
		for (i = 0; i < num_sensors; i++)
		{
			command[0] = 0x40 | ((i + 1) & 0x03); // output enable | read input i
			r = write(fd, &command, 2);
			usleep(delay);
			// the read is always one step behind the selected input
			r = read(fd, &value[i], 1);
			if (r != 1)
				perror("reading i2c device\n");
			usleep(delay);
			//n = value[i];
			n = rev(value[i]);
			move(3 + i, 0);
			printw("Input: %d Data: %0.3d", i, n);
			refresh();
			key = getch();
			if (key == 43)
				command[1]++;
			else if (key == 45)
				command[1]--;
			else if (key > -1)
				break;
		}
	}
	endwin();
	close(fd);
	printf("%d\n", key);
	return(0);
}
