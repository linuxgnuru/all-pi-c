#include <stdio.h>
#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <string.h>
//#include <sys/fcntl.h>
//#include <sys/ioctl.h>
#include <errno.h>

#include <wiringPi.h>

int main(int argc, char **argv)
{
	unsigned char value[4];
	useconds_t delay = 2000;
	//char *dev = "/dev/i2c-1";
	char dev[40];
	int r, fd;
	int addr = 0x48;
	int lcdFD;
	int new_val;
	int i, j;

    strcpy(dev, "/dev/i2c-1");
	// try to connect to display
	if (wiringPiSetup() < 0)
	{
	    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
	    return 1;
	}
	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	fd = open(dev, O_RDWR);
	if (fd < 0)
	{
		perror("Opening i2c device node\n");
		return 1;
	}
	r = ioctl(fd, I2C_SLAVE, addr);
	if (r < 0) perror("Selecting i2c device\n");
	j = 0;
	for (;;)
	{
		// the read is always one step behind the selected input
		r = read(fd, &value[0], 1);
		if (r != 1) perror("reading i2c device\n");
		new_val = value[0];
		new_val /= 16;
		usleep(delay);
		lcdPosition(lcdFD, 0, 0);
		for (i = 0; i < 16; i++)
		{
			if (i < new_val)
				lcdPutchar(lcdFD, '*');
			else
				lcdPutchar(lcdFD, ' ');
		}
		/*
		lcdPosition(lcdFD, 0, 1);
		lcdPrintf(lcdFD, "Value: %d", new_val);
		*/
	}
	close(fd);
	return(0);
}
