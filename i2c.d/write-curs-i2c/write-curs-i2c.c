#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // unsure
#include <unistd.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h> // unsure
#include <libgen.h> // unsure
#include <ncurses.h>

#include <wiringPi.h>

#define ADDRESS 0x04

static const char *devName = "/dev/i2c-1";

int  leftButtonState, rightButtonState;
int  lastLeftButtonState,  lastRightButtonState;
long lastLeftDebounceTime, lastRightDebounceTime;
long debounceDelay = 50;
int last_rot = 100;

int main(int argc, char **argv)
{
	int reading;
	int file;
	int arg;
	int val;
	int i;
	int rotation_val = 89; // this number must remain within 0 and 179
	int first = 1;

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
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return 1;
	}
	lastLeftButtonState = lastRightButtonState = HIGH;
	lastLeftDebounceTime = lastRightDebounceTime = 0;
	pinMode(21, INPUT);
	pinMode(22, INPUT);
	pullUpDnControl(21, PUD_UP);
	pullUpDnControl(22, PUD_UP);
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
		reading = digitalRead(21);
		if (reading != lastLeftButtonState)
			lastLeftDebounceTime = millis(); // reset the debouncing timer
		if ((millis() - lastLeftDebounceTime) > debounceDelay)
		{
			if (reading != leftButtonState)
			{
				leftButtonState = reading;
				if (leftButtonState == LOW)
				{
					if (rotation_val + 16 <= 179)
						rotation_val += 16;
					else
						rotation_val = 160;
				}
			}
		}
		lastLeftButtonState = reading;
		/*
		 * right
		 */
		reading = digitalRead(22);
		if (reading != lastRightButtonState)
			lastRightDebounceTime = millis();
		if ((millis() - lastRightDebounceTime) > debounceDelay)
		{
			if (reading != rightButtonState)
			{
				rightButtonState = reading;
				if (rightButtonState == LOW)
				{
					if (rotation_val - 16 >= 0)
						rotation_val -= 16;
					else
						rotation_val = 16;
				}
			}
		}
		lastRightButtonState = reading;
		unsigned char cmd[16];
		cmd[0] = rotation_val;
		if (write(file, cmd, 1) == 1)
		{
			char buf[1];
			if (read(file, buf, 1) == 1)
			{
			 	int temp = (int)buf[0];
				if (first == 1)
				{
					move(3, 0);
					printw("%d", temp);
					refresh();
					first = 2;
				}
				if (temp == last_rot)
				{
					move(3, 0);
					printw("%d", temp);
					refresh();
				}
				else
					last_rot = temp;
				usleep(10000);
			}
		}
/*
		//printf("Sending %d\n", val);
		//if (0 == sscanf(argv[arg], "%d", &val))
		//cmd[0] = val;
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
