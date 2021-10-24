#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>

#include <wiringPi.h>

#define FAR 1
#define CEL 2

int main(int argc, char *argv[])
{
	int fd = -1, ret;
	char devname[100] = "/sys/class/thermal/thermal_zone0/temp";
	long value;
	char buffer[100];
	char errmsg[100];
	char c_f;
	int integer, decimal;
	int c_f_flag = FAR; // set scale to default to farenheigholhthtt
	int tmp_i;

	if (argc > 1)
		c_f_flag = (strcasecmp(argv[1], "F") == 0 ? FAR : CEL);
	c_f = (c_f_flag == FAR ? 'F' : 'C');
	wiringPiSetup();
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
	decimal = value % 1000;
	if (c_f_flag == FAR)
	{
		tmp_i = ((integer * 9) / 5) + 32;
		integer = tmp_i;
		tmp_i = ((decimal * 9) / 5) + 32;
		decimal = tmp_i;
	}
	close(fd);
	printf("Temperature: %d.%d %c\n", integer, decimal, c_f);
	return 0;
}
