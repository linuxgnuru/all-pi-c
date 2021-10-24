#include <stdio.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8574 100

void pcf_toggle(int pin)
{
	digitalWrite(MY_8574 + pin, !digitalRead(MY_8574 + pin));
}

int main(int argc, char **argv)
{
	int i;
	int delayn = 50;
	int gpio_in = 7;
	int gpio_out = 2;
	int pcf8574_pin = 0;
	int pcf8574_adr = 0x20;
	int pcf8574_base = MY_8574;

	if (argc == 5)
	{
		// argv[1] == input pin
		// argv[2] == gpio pin output
		// argv[3] == pcf8574 addr
		// argv[4] == pcf8574 basepin
		// argv[5] == pcf8574 pin output
		gpio_in = atoi(argv[1]);
		gpio_out = atoi(argv[2]);
		pcf8574_adr = atoi(argv[3]);
		pcf8574_base = atoi(argv[4]);
		pcf8574_pin = atoi(argv[5]);
	}
	wiringPiSetup();
	pcf8574Setup(pcf8574_base, pcf8574_adr);
	pinMode(gpio_out, OUTPUT);
	pinMode(gpio_in, INPUT);
	digitalWrite(gpio_out, LOW);

	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("Obstacle Avoidance");
	mvaddstr(2, 0, "using input pin GPIO 7 and output pin PCF8574 pin 0");
	mvaddstr(3, 0, "Status: ");
	refresh();
	for (i = 0; i < 7; i++)
		digitalWrite(pcf8574_base + i, 1);
	for (;;)
	{
		if (digitalRead(gpio_in) == LOW)
		{
			move(3, 8);
			printw("HIT");
			digitalWrite(gpio_out, HIGH);
			digitalWrite(pcf8574_base + pcf8574_pin, 0);
			//pcf_toggle(pin);
		}
		else
		{
			move(3, 8);
			printw("   ");
			digitalWrite(gpio_out, LOW);
			digitalWrite(pcf8574_base + pcf8574_pin, 1);
		}
		refresh();
		delay(delayn);
	}
	endwin();
	return(0);
}
