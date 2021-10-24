#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>

#include <wiringPi.h>

#define GPIO_PIN 5

// Anything under 80 is ok; but for this test I'll make the threshold 70
#define MAX_TEMP 70

int Temp()
{
	int fd = -1, ret;
	char devname[100] = "/sys/class/thermal/thermal_zone0/temp";
	char buffer[100];
	char errmsg[100];
	long value;
	int integer;

	if ((fd = open(devname, O_RDONLY)) < 0)
	{
		sprintf(errmsg, "Error opening device: %s", devname);
		perror(errmsg);
		exit(1);
	}
	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0)
	{
		perror("read error");
		exit(1);
	}
	value = atoi(buffer);
	integer = value / 1000;
	close(fd);
	return integer;
}

int main(int argc, char *argv[])
{
	int delay_n = 300;

	wiringPiSetup();
	while (1)
	{
		digitalWrite(GPIO_PIN, (Temp() > MAX_TEMP ? HIGH : LOW));
		delay(delay_n); // add a delay so we don't always read the file.
	}
	return 0;
}

