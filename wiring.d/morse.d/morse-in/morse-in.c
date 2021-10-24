#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <wiringPi.h>

const int gpioPin0 = 17;
const int gpioPin1 = 18;
const int gpioPin2 = 27;
const int gpioPin3 = 22;
const int gpioPin4 = 23;
const int gpioPin5 = 24;
const int gpioPin6 = 25;
const int gpioPin7 = 4;
const int gpioPin8 = 2;

int main(int argc, char **argv)
{
	useconds_t delay = 2000;
	useconds_t dot_delay = 500000;
	useconds_t dash_delay = 100000;
	int i, j;

	wiringPiSetupGpio();
	pinMode(gpioPin8, INPUT); // GPIO 4
	pinMode(gpioPin5, OUTPUT); // GPIO 5
	for (;;)
	{
		if (digitalRead(gpioPin8) == 0)
		{
			digitalWrite(gpioPin5, HIGH);
			//usleep(delay);
		}
		else
		{
			digitalWrite(gpioPin5, LOW);
		}
	}
	return(0);
}
