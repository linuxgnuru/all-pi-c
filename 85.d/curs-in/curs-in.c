#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define MY_8574 100

int devid = 0x20;   //you cam use "i2cdetect" to find this devid
int fd;	// 

//void pcf_toggle(int pin) { digitalWrite(MY_8574 + pin, !digitalRead(MY_8574 + pin)); }

// catch signals and clean up GPIO
static void die(int sig)
{
	int i;
	//for (i = 0; i < 8; i++) digitalWrite(MY_8574 + i, LOW);
	move(9, 0);
	refresh();
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "\n\ncaught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "\n\nExiting due to Ctrl + C\n");
	endwin();
	exit(0);
}

int main(int argc, char **argv)
{
	int pin;
	int err = 0;
	int i;

	/*
	if (argc > 1)
	{
		pin = atoi(argv[1]);
		if (pin < 0 || pin > 7) err = 1;
	}
	else err = 1;
	if (err == 1)
	{
		printf("usage: %s PIN\nwhere PIN is a number from 0 - 7\n", argv[0]);
		return 0;
	}
	*/
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	/*
	 * START OF INIT
	 */
	if ((fd = wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	if ((wiringPiI2CWrite(fd, 0xff)) < 0)
	{
	    fprintf(stderr, "write badly %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	//delay(500);
	delay(50);
	if ((wiringPiI2CWrite(fd, 0)) < 0)
	{
	    fprintf(stderr, "write badly %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	//delay(500);
	delay(50);
	err = wiringPiI2CRead(fd);
	/*
	 * END OF INIT
	 */
	pcf8574Setup(MY_8574, devid);
	//for (i = 0; i < 8; i++) pinMode(i, INPUT);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8574");
	mvaddstr(1, 0, "+---+---+---+---+---+---+---+---+");
	mvaddstr(2, 0, "| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |");
	mvaddstr(3, 0, "+---+---+---+---+---+---+---+---+");
	mvaddstr(4, 0, "|   |   |   |   |   |   |   |   |");
	mvaddstr(5, 0, "+---+---+---+---+---+---+---+---+");
	refresh();
	pin = 0;
	while (1)
	{
		move(4, 0);
		printw("| %d | %d | %d | %d | %d | %d | %d | %d |",
				(digitalRead(MY_8574 + pin++) == HIGH ? 1 : 0),
				(digitalRead(MY_8574 + pin++) == HIGH ? 1 : 0),
				(digitalRead(MY_8574 + pin++) == HIGH ? 1 : 0),
				(digitalRead(MY_8574 + pin++) == HIGH ? 1 : 0),
				(digitalRead(MY_8574 + pin++) == HIGH ? 1 : 0),
				(digitalRead(MY_8574 + pin++) == HIGH ? 1 : 0),
				(digitalRead(MY_8574 + pin++) == HIGH ? 1 : 0),
				(digitalRead(MY_8574 + pin  ) == HIGH ? 1 : 0));
		refresh();
		pin = 0;
		delay(100);
	}
	//printf("%s\n", ( digitalRead(MY_8574 + pin) == HIGH ? "HIGH" : "LOW"));
	return 0;
}

/*
	for (i = 0; i < 8; i++)
	{
		digitalWrite(MY_8574 + i, 1);
	}
	for (loop = 0; loop < 2; loop++)
	{
		for (i = 0; i < 8; i++)
		{
			swap_v = (swap == 0 ? i : abs(i-7));
			digitalWrite(MY_8574 + swap_v, !digitalRead(MY_8574 + swap_v));
			delay(delay_n);
		}
		swap = (swap == 0 ? 1 : 0);
	}
*/
