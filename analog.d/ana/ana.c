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
	i2c_add = 0x4d;

	wiringPiSetup();
	pcf8591Setup(base, i2c_add);
	Cls();
	for (;;)
	{
		//printf("%4d %4d %4d %4d\n", rev(analogRead(200)), rev(analogRead(201)), rev(analogRead(202)), rev(analogRead(203));
		switch (argc)
		{
			case 1:
				printf("%4d\n", rev(analogRead(200)));
				break;
			case 2:
				printf("[1] %4d [2] %4d\n", rev(analogRead(200)), rev(analogRead(201)));
				break;
			case 3:
				printf("[1] %4d [2] %4d [3] %4d\n", rev(analogRead(200)), rev(analogRead(201)), rev(analogRead(202)));
				break;
			case 4:
				printf("[1] %4d [2] %4d [3] %4d [4] %4d\n", rev(analogRead(200)), rev(analogRead(201)), rev(analogRead(202)), rev(analogRead(203)));
				break;
		}
		usleep(800000);
		Cls();

	}
	return 0;
}
