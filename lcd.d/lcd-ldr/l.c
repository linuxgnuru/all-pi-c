/* 
             define from  wiringPi.h                     define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
                8/SDA | | 5V               ->                  SDA | | 5V
                9/SCL | | GND              ->                  SCL | | GND
                    7 | | 14/TX            ->                  IO7 | | TX
                  GND | | 15/RX            ->                  GND | | RX
                    0 | | 18               ->                  IO0 | | IO1
                    2 | | GND              ->                  IO2 | | GND
                    3 | | 23               ->                  IO3 | | IO4
                  VCC | | 24               ->                  VCC | | IO5
              MOSI/12 | | GND              ->                 MOSI | | GND
              MISO/13 | | 25               ->                 MISO | | IO6
               SCK/14 | | 8/CE0            ->                  SCK | | CE0
                  GND | | 9/CE19           ->                  GND | | CE1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <ncurses.h>

#include <wiringPi.h>

#define MY_8591 100

int rev(int value) { return (abs(value - 255)); }

int main(int argc, char **argv)
{
	int pin;
	int delay_n = 300;
	int addr = 0x48;
	int value;

	pin = (argc > 1 ? atoi(argv[1]) : 0);
	if (wiringPiSetup() < 0)
	{
	    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
	    return 1;
	}
	// setup analog input
	pcf8591Setup(MY_8591, addr);
	//value = rev(analogRead(MY_8591 + pin));
	value = analogRead(MY_8591 + pin);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("LDR value:");
//	mvaddstr(2, 0, "Analog output");
	refresh();
	while (1)
	{
		value = rev(analogRead(MY_8591 + pin));
		move(2, 5);
		printw("%02d", value);
		delay(delay_n);
	}
	endwin();
	return 0;
}

