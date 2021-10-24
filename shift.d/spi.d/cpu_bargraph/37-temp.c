/**************************************************************************
  OPEN2440 ds18b20 1-wires driver test
  device node is "/sys/devices/w1 bus master/28-IDNUM/w1_slave"
  IDNUM is read from ds18b20's rom, the value is the only one
  
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>

#define FAR 1
#define CEL 2

int main(int argc, char *argv[])
{
	int fd = -1, ret;
	char *buf, tmp[10], ch='t';
	char devname_head[50] = "/sys/devices/w1_bus_master1/28-00000";
	char devname_end[10] = "/w1_slave";
	char dev_name[100];
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
	strcpy(dev_name, devname_head);
	strcat(dev_name, "5e66e3b");
	strcat(dev_name, devname_end);
	if ((fd = open(dev_name, O_RDONLY)) < 0)
	{
		sprintf(errmsg, "Error opening device: %s", dev_name);
		perror(errmsg);
		exit(1);
	}
	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0)
	{
		perror("read error");
		exit(1);
	}
	buf = strchr(buffer, ch);
	sscanf(buf, "t=%s", tmp);
	value = atoi(tmp);
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
