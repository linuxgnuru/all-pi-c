#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8591 100

const int addr = 0x48;
const int delay_n = 200;

int main(int argc, char **argv)
{
    int raw = 0;
    int key;
    float Vin = 3.3;
    float Vout = 0;
    float R1 = 10;
    float R2 = 0;
    float buffer = 0;

	if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return 1;
    }
	pcf8591Setup(MY_8591, addr);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("Resistor Meter / Tester");
    mvaddstr(1, 0, "Voltage: ");
    mvaddstr(2, 0, "R2: ");
    refresh();
	while (1)
	{
        raw = analogRead(MY_8591);
        Vout = (3.3 / 256) * raw;
        move(1, 9);
        printw("%0.2f", Vout);
        buffer = (Vin / Vout) - 1;
        R2 = R1 / buffer;
        move(2, 5);
        printw("%0.3f", R2);
        refresh();
        // the read is always one step behind the selected input
        //printw("%0.3d", analogRead(MY_8591 + i));
        delay(delay_n);
		key = getch();
		if (key > -1) break;
	}
	endwin();
	return(0);
}
