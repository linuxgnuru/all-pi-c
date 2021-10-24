#include <stdio.h>
#include <errno.h>

#include <wiringPiI2C.h>
#include <wiringPi.h>

int main(int argc, char **argv)
{
	int devid = 0x20;
	int fd;
	int error_c = 0;
	int error_t;
	int i;
	int pin_num;
	int new_pin_num;

	if (argc > 1)
	{
		pin_num = atoi(argv[1]);
		if (pin_num < 0 || pin_num > 7)
		{
			error_c = 1;
			error_t = 0;
		}
		if (argc == 3) devid = atoi(argv[2]);
	}
	else
	{
		error_c = 1;
		error_t = 1;
	}
	if (error_c == 1)
	{
		printf("Error: ");
		if (error_t == 0)
			printf("Invalid pin num: %d\n", pin_num);
		else
			printf("Not enough arguements.\n");
		printf("Usage: %s NUM [PORT]\n", argv[0]);
		printf("Where NUM is in the range of 0 - 7\n");
		printf("And PORT is optional (defaults to 0x20)\n");
		return 1;
	}
	if ((fd=wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror (errno));
	    return 1;
	}
	new_pin_num = wiringPiI2CRead(fd);
	// Just turn on/off the bit that needs to be changed
	new_pin_num ^= (1<<pin_num);
	// attempt to turn on pin
	if ((wiringPiI2CWrite(fd, new_pin_num)) < 0)
	{
		fprintf (stderr, "write badly %s\n", strerror (errno));
		return 1;
	}
	return(0);
}
