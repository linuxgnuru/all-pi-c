#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>

#define MY_8591 100

int rev(int value) { return (abs(value - 255)); }

int main(int argc, char **argv)
{
	int err;
	int pin;
	int addr = 0x48;

	pin = (argc > 1 ? atoi(argv[1]) : 0);
	if (wiringPiSetup() < 0)
	{
		fprintf(stderr, "Unable to open device: %s\n", strerror(errno));
		return 1;
	}
	printf("pin: %d\n", pin);
	pcf8591Setup(MY_8591, addr);
	printf("LDR: %0.3d\n", rev(analogRead(MY_8591 + pin)));
	return 0;
}
