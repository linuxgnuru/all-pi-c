#include <stdio.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>

int devid = 0x20;
int fd;

int rev(int val) { return abs(val - 255); }

int main(int argc, char **argv)
{
	int max = 0x200;
	int x, i, j, k, l, m;
	int swap, tmp_n;
	int pscale;
	int nums[9];
	int numst[9];
	int indiv[9];

	j = max;
	for (i = 8; i >= 0; i--)
	{
		j /= 2;
		k = j - 1;
		nums[i] = k;
		//printf("k: %d\n", k);
	}
	for (i = 0; i <= 8; i++)
	{
		x = abs(i - 8);
		//printf("x: %d\n", x);
		numst[i] = nums[x];
	}
	/*
	for (i = 0; i <= 8; i++)
	{
		printf("i: %d\tnumst: %d\tnums: %d\n", i, numst[i], nums[i]);
	}
	*/
	l = 255;
	k = 1;
	for (i = 0; i <= 8; i++)
	{
		m = l - k;
		indiv[i] = m;
		k *= 2;
	}
	for (i = 0; i <= 8; i++)
	{
		printf("i: %d\tnumst: %d\tnums: %d\tindiv: %d\n", i, numst[i], nums[i], indiv[i]);
	}
	/*
	if ((fd=wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror (errno));
	    return 1;
	}
	x = 1;
	for (i = 0; i <= 8; i++)
	{
		tmp_n = nums[i];
		//printf("tmp_n: %d\n", tmp_n);
		if ((wiringPiI2CWrite(fd,tmp_n)) < 0)
		{
		    fprintf (stderr, "write badly %s\n", strerror (errno));
		    return 1;
		}
		delay(500);
	}
	*/
	return(0);
}
