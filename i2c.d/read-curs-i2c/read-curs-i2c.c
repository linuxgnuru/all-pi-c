#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>

#define ADDRESS 0x04

static const char *devName = "/dev/i2c-1";

int main(int argc, char **argv)
{
	int file;
	int arg;
	int val;
	int i;

	/*
	 * TODO
	 * Try to see if user is root then bail if not.
	 */
	if ((file = open(devName, O_RDWR)) < 0)
	{
		fprintf(stderr, "I2C: Failed to access %d\n", devName);
		exit(1);
	}
	//printw("I2C Connecting");
	if (ioctl(file, I2C_SLAVE, ADDRESS) < 0)
	{
		fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
		exit(1);
	}
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	move(1, 0);
	printw("I2C: Acquiring bus to 0x%x", ADDRESS);
	mvaddstr(2, 0, "Data");
	//mvaddstr(2, 0, "LEDs:");
	refresh();
	while (1)
	{
		char buf[1];
		if (read(file, buf, 1) == 1)
		{
		 	int temp = (int)buf[0];
			move(3, 0);
			printw("%d", temp);
			refresh();
			usleep(10000);
		}
/*
		unsigned char cmd[16];
		//if (0 == sscanf(argv[arg], "%d", &val))
		//printf("Sending %d\n", val);
		for (i = 1; i < 7; i++)
		{
			cmd[0] = i;
			if (write(file, cmd, 1) == 1)
			{
				//
				// As we are not talking to direct hardware but a microntroller we
				// need to wait a short while so that it can respond.
				//
				// 1 ms seems to be enough but it depends on what workload it has
				usleep(10000);
				char buf[1];
				if (read(file, buf, 1) == 1)
				{
				 	int temp = (int)buf[0];
					//printf("Received %d\n", temp);
					if (i == 1)
					{
						switch (temp)
						{
							case 0: // 000
								move(2, 11); printw("0"); move(2, 21); printw("0"); move(2, 30); printw("0");
								break;
							case 1: // 001
								move(2, 11); printw("0"); move(2, 21); printw("0"); move(2, 30); printw("1");
								break;
							case 10: // 010
								move(2, 11); printw("0"); move(2, 21); printw("1"); move(2, 30); printw("0");
								break;
							case 11: // 011
								move(2, 11); printw("0"); move(2, 21); printw("1"); move(2, 30); printw("1");
								break;
							case 100:
								move(2, 11); printw("1"); move(2, 21); printw("0"); move(2, 30); printw("0");
								break;
							case 101:
								move(2, 11); printw("1"); move(2, 21); printw("0"); move(2, 30); printw("1");
								break;
							case 110:
								move(2, 11); printw("1"); move(2, 21); printw("1"); move(2, 30); printw("0");
								break;
							case 111:
								move(2, 11); printw("1"); move(2, 21); printw("1"); move(2, 30); printw("1");
								break;
							default: // Shouldn't get to this spot.
								move(2, 11); printw("E"); move(2, 21); printw("E"); move(2, 30); printw("E");
								break;
						}
					}
					else
					{
						move(i + 1, 11);
						printw("%d", temp);
					}
					refresh();
				}
			}
			// now wait else you could crash the arduino by sending requests too fast
			usleep(10000);
		}
*/
		usleep(10000);
		move(3, 0);
		printw("               ");
		refresh();
	}
	close(file);
	endwin();
	return EXIT_SUCCESS;
}