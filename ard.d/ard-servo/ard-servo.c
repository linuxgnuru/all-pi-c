#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define ADDRESS 0x04

static const char *devName = "/dev/i2c-1";

int main(int argc, char **argv)
{
	int file;
	int degree = 200;

	if (argc > 1) degree = atoi(argv[1]);
	if (degree < 0 || degree > 179)
	{
		printf("usage: %s [degree (0 - 179)]\n", argv[0]);
		return EXIT_FAILURE;
	}
	if ((file = open(devName, O_RDWR)) < 0)
	{
		fprintf(stderr, "I2C: Failed to access %d\n", devName);
		exit(1);
	}
	if (ioctl(file, I2C_SLAVE, ADDRESS) < 0)
	{
		fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
		exit(1);
	}
	unsigned char cmd[16];
	cmd[0] = degree;
	if (write(file, cmd, 1) == 1)
	{
		char buf[1];
		if (read(file, buf, 1) == 1)
		{
		 	int temp = (int)buf[0];
			printf("%d\n", temp);
		}
	}
	close(file);
	return EXIT_SUCCESS;
}
