#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <wiringPi.h>
#include <pcf8591.h>


// Clears the screen
void Cls() { printf("\033[2J\033[H"); }

// Return an absolute value to reverse the numbers.
int rev(int val) { return abs(val - 255); }

int main(int argc, char **argv)
{
	int base;
	int i2c_add;

	base = 200;
	i2c_add = 0x4d; // I have a second board and changed this address; by default it is 0x48

	wiringPiSetup();
	pcf8591Setup(base, i2c_add);
	Cls();
	for (;;)
	{
		printf("%4d %4d %4d %4d\n", rev(analogRead(200)), rev(analogRead(201)), rev(analogRead(202)), rev(analogRead(203));
		usleep(800000);
		Cls(); // NOTE: try to add some ncurses instead of simply clearing the screen.

	}
	return 0;
}
