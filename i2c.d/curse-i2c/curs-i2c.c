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

int usage(const char *progName)
{
	fprintf(stderr, "Usage: %s [commands to send to Arduino]\n", progName);
	return EXIT_FAILURE;
}

int main(int argc, char **argv)
{
	int file;
	int arg;
	int val;
	int i;

	//if (argc == 1) return usage(argv[0]);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("I2C Connecting");
	if ((file = open(devName, O_RDWR)) < 0)
	{
		fprintf(stderr, "I2C: Failed to access %d\n", devName);
		exit(1);
	}
	move(1, 0);
	printw("I2C: Acquiring bus to 0x%x", ADDRESS);
	if (ioctl(file, I2C_SLAVE, ADDRESS) < 0)
	{
		fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
		exit(1);
	}
	//              01234
	mvaddstr(2, 0, "LEDs:");
	//                        1         2         3
	//              0123456789012345678901234567890
	//              LEDs: Red: 1 Yellow: 0 Green: 1
	//                     2, 11     2, 21    2, 30
	mvaddstr(2, 6, "Red: ");
	mvaddstr(2, 13, "Yellow: ");
	mvaddstr(2, 23, "Green: ");
	/*
	mvaddstr(2,  6, "Ana0"); mvaddstr(2, 11, "Ana1");
	mvaddstr(2, 16, "Ana2"); mvaddstr(2, 21, "Ana3");
	mvaddstr(2, 26, "Ana4");
	*/
	//              012345678901
	mvaddstr(3, 0, "   Temp A:"); // 3, 11
	mvaddstr(4, 0, "      LDR:"); // 4, 11
	mvaddstr(5, 0, "     Temp:"); // 3, 11
	mvaddstr(6, 0, "50K pot A:"); // 5, 11
	mvaddstr(7, 0, "50K pot B:"); // 6, 11
	refresh();
	while (1)
	{
		unsigned char cmd[16];
		//if (0 == sscanf(argv[arg], "%d", &val))
		//printf("Sending %d\n", val);
		for (i = 1; i < 7; i++)
		{
			cmd[0] = i;
			if (write(file, cmd, 1) == 1)
			{
				/*
				 * As we are not talking to direct hardware but a microntroller we
				 * need to wait a short while so that it can respond.
				 *
				 * 1 ms seems to be enough but it depends on what workload it has
				 */
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
								move(2, 11); printw("0");
								move(2, 21); printw("0");
								move(2, 30); printw("0");
								break;
							case 1: // 001
								move(2, 11); printw("0");
								move(2, 21); printw("0");
								move(2, 30); printw("1");
								break;
							case 10: // 010
								move(2, 11); printw("0");
								move(2, 21); printw("1");
								move(2, 30); printw("0");
								break;
							case 11: // 011
								move(2, 11); printw("0");
								move(2, 21); printw("1");
								move(2, 30); printw("1");
								break;
							case 100:
								move(2, 11); printw("1");
								move(2, 21); printw("0");
								move(2, 30); printw("0");
								break;
							case 101:
								move(2, 11); printw("1");
								move(2, 21); printw("0");
								move(2, 30); printw("1");
								break;
							case 110:
								move(2, 11); printw("1");
								move(2, 21); printw("1");
								move(2, 30); printw("0");
								break;
							case 111:
								move(2, 11); printw("1");
								move(2, 21); printw("1");
								move(2, 30); printw("1");
								break;
							default: // Shouldn't get to this spot.
								move(2, 11); printw("E");
								move(2, 21); printw("E");
								move(2, 30); printw("E");
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
		usleep(10000);
	}
	close(file);
	endwin();
	return EXIT_SUCCESS;
}
