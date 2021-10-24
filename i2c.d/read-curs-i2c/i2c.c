#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define ADDRESS 0x04

static const char *devName = "/dev/i2c-1";

int usage(const char *progName)
{
	fprintf(stderr, "Usage: %s [commands to send to Arduino]\n", progName);
	return EXIT_FAILURE;
}

int main(int argc, char **argv)
{
	int file;
	int arg;
	int val;
	int integer;
	int decimal;

	if (argc == 1)
		return usage(argv[0]);
	printf("I2C Connecting\n");
	if ((file = open(devName, O_RDWR)) < 0)
	{
		fprintf(stderr, "I2C: Failed to access %d\n", devName);
		exit(1);
	}
	printf("I2C: Acquiring buss to 0x%x\n", ADDRESS);
	if (ioctl(file, I2C_SLAVE, ADDRESS) < 0)
	{
		fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
		exit(1);
	}
	for (arg = 1; arg < argc; arg++)
	{
		unsigned char cmd[16];
		if (0 == sscanf(argv[arg], "%d", &val))
		{
			fprintf(stderr, "Invalid parameter %d \"%s\"\n", arg, argv[arg]);
			exit(1);
		}
		printf("Sending %d\n", val);
		cmd[0] = val;
		if (write(file, cmd, 1) == 1)
		{
			/*
			 * As we are not talking to direct hardware but a microntroller we
			 * need to wait a short while so that it can respond.
			 *
			 * 1 ms seems to be enough but it depends on what workload it has
			 */
			usleep(10000);
			char buf[1];
			if (read(file, buf, 1) == 1)
			{
			 	int temp = (int)buf[0];
				if (cmd[0] == 4)
				{
					printf("raw: %d\n", temp);
					integer = temp / 1000;
					decimal = temp % 1000;
					printf("Received %d.%d\n", integer, decimal);
				}
				else
				{
					integer = decimal = temp;
					printf("Received %d\n", temp);
				}
			}
		}
		// now wait else you could crash the arduino by sending requests too fast
		usleep(10000);
	}
	close(file);
	return EXIT_SUCCESS;
}
