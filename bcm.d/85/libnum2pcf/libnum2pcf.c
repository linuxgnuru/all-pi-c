#include <stdio.h>
#include <errno.h>

#include <wiringPiI2C.h>
#include <wiringPi.h>

int num2pcf(int devid, int user_pin)
{
	int fd;
	int cur_pin_data;

	if ((fd=wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror(errno));
	    return 1;
	}
	cur_pin_data = wiringPiI2CRead(fd);
	// Just turn on/off the bit that needs to be changed
	cur_pin_data ^= (1<<user_pin);
	// attempt to turn on pin
	if ((wiringPiI2CWrite(fd, cur_pin_data)) < 0)
	{
		fprintf (stderr, "write badly %s\n", strerror(errno));
		return 1;
	}
	return 0;
}
