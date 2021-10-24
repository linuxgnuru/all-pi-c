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

int rev(int val)
{ 
	return abs(val - 255);
}

int main(int argc, char **argv)
{
	unsigned char value[4];
	useconds_t delay = 2000;
	char *dev = "/dev/i2c-1";
	int r, fd;
	int addr = 0x48;
	int new_val;
	int i, j;

	// try to connect to display
	/*
	if (wiringPiSetup() < 0)
	{
	    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
	    return 1;
	}
	*/
	wiringPiSetupGpio();
	pinMode(gpioPin0, OUTPUT); // GPIO 0
	pinMode(gpioPin1, OUTPUT); // GPIO 1
	pinMode(gpioPin2, OUTPUT); // GPIO 2
	pinMode(gpioPin3, OUTPUT); // GPIO 3
	pinMode(gpioPin4, OUTPUT); // GPIO 4
	pinMode(gpioPin5, OUTPUT); // GPIO 5
	pinMode(gpioPin6, OUTPUT); // GPIO 6
	pinMode(gpioPin7, OUTPUT); // GPIO 7
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
		new_val = rev(value[0]);
		new_val /= 32;
		usleep(delay);
		for (i = 0; i < 8; i++)
		{
			switch (i)
			{
				case 0:
					if (i < new_val) digitalWrite(gpioPin0, HIGH);
					else digitalWrite(gpioPin0, LOW);
					break;
				case 1:
					if (i < new_val) digitalWrite(gpioPin1, HIGH);
					else digitalWrite(gpioPin1, LOW);
					break;
				case 2:
					if (i < new_val) digitalWrite(gpioPin2, HIGH);
					else digitalWrite(gpioPin2, LOW);
					break;
				case 3:
					if (i < new_val) digitalWrite(gpioPin3, HIGH);
					else digitalWrite(gpioPin3, LOW);
					break;
				case 4:
					if (i < new_val) digitalWrite(gpioPin4, HIGH);
					else digitalWrite(gpioPin4, LOW);
					break;
				case 5:
					if (i < new_val) digitalWrite(gpioPin5, HIGH);
					else digitalWrite(gpioPin5, LOW);
					break;
				case 6:
					if (i < new_val) digitalWrite(gpioPin6, HIGH);
					else digitalWrite(gpioPin6, LOW);
					break;
				case 7:
					if (i < new_val) digitalWrite(gpioPin7, HIGH);
					else digitalWrite(gpioPin7, LOW);
					break;
			}
			usleep(delay);
		}
		/*
		lcdPosition(lcdFD, 0, 1);
		lcdPrintf(lcdFD, "Value: %d", new_val);
		*/
	}
	close(fd);
	return(0);
}
