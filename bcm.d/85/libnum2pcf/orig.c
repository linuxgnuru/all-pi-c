#include <stdio.h>
#include <string.h> 
#include <errno.h>

#include<wiringPi.h>
#include<wiringPiI2C.h>

int devid = 0x20;
int fd;

int main(int argc, char **argv)
{
	int max = 0x200;
	int x, i, j, k;
	int nums[9];
	int swap, tmp_n;

	j = max;
	for (i = 0; i < 9; i++)
	{
		j /= 2;
		k = j - 1;
		nums[i] = k;
	}
	if ((fd=wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf (stderr, "Unable to open I2C device: %s\n", strerror (errno));
	    return 1;
	}
	j = 0;
	x = 1;
	swap = 1;
	while (x == 1)
	{
		for (i = 0; i < 9; i++)
		{
			tmp_n = (swap == 1 ? nums[i] : nums[abs(i-9)]);
			if ((wiringPiI2CWrite(fd,tmp_n)) < 0)
			{
			    fprintf (stderr, "write badly %s\n", strerror (errno));
			    return 1;
			}
			delay(500);
		}
		swap = (swap == 1 ? 0 : 1);
		if (j++ == 5) x = 2;
	}
	return 0;
}
