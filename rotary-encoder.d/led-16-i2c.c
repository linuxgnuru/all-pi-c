#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <wiringPi.h>

int main(int argc, char **argv)
{
	unsigned char value[4];
	useconds_t delay = 2000;
	char *dev = "/dev/i2c-1";
	int r, fd;
	int addr = 0x48;
	int new_val;
	int i, j, k, l, m;
	int tmp_n;
	int pscale;
	int gpio_leds[] = { 17, 18, 27, 22, 23, 24, 25, 4 };
	int nums[8];
	int rev_nums[8];
	int all_off = 255;
	// up
	// 127, 63, 31, 15, 7, 3, 1, 0
	// down
	// 0, 1, 3, 7, 15, 31, 63, 127

	j = 0x200;
	for (i = 7; i >= 0; i--)
	{
		j /= 2;
		k = j - 1;
		rev_nums[i] = k;
	}
	for (i = 0; i < 8; i++)
	{
		nums[i] = rev_nums[i];
	}
	wiringPiSetupGpio();
	// Set GPIO pins for output
	for (i = 0; i < 8; i++)
		pinMode(gpio_leds[i], OUTPUT);
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
		new_val = abs(value[0] - 255);
		new_val /= 16;
		usleep(delay);
		//printf("nv: %d\n", new_val);
		for (i = 0; i < 16; i++)
		{
			if (i < 8)
			{
				if (i < new_val) digitalWrite(gpio_leds[i], HIGH);
				else digitalWrite(gpio_leds[i], LOW);
			}
			else
			{
				pscale = abs(i - 16) - 1;
				if (i < new_val) tmp_n = nums[pscale];
				else tmp_n = rev_nums[pscale];
				//printf("tmp_n: %d\n", tmp_n);
				if ((wiringPiI2CWrite(fd,tmp_n)) < 0)
				{
				    fprintf (stderr, "write badly %s\n", strerror (errno));
				    return 1;
				}
			}
			usleep(delay);
		}
	}
	close(fd);
	return(0);
}
