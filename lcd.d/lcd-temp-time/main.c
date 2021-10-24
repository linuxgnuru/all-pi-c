/*
             define from  wiringPi.h                     define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
                8/SDA | | 5V               ->                  SDA | | 5V
                9/SCL | | GND              ->                  SCL | | GND
                    7 | | 14/TX            ->                  IO7 | | TX
                  GND | | 15/RX            ->                  GND | | RX
                    0 | | 18               ->                  IO0 | | IO1
                    2 | | GND              ->                  IO2 | | GND
                    3 | | 23               ->                  IO3 | | IO4
                  VCC | | 24               ->                  VCC | | IO5
              MOSI/12 | | GND              ->                 MOSI | | GND
              MISO/13 | | 25               ->                 MISO | | IO6
               SCK/14 | | 8/CE0            ->                  SCK | | CE0
                  GND | | 9/CE19           ->                  GND | | CE1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/fcntl.h>


#include<wiringPi.h>
#include<lcd.h>

const int RS = 3;	//
const int EN = 14;	//
const int D0 = 4;	//
const int D1 = 12;	//
const int D2 = 13;	//
const int D3 = 6;	//

/**************************************************************************
  OPEN2440 ds18b20 1-wires driver test
  device node is "/sys/devices/w1 bus master/28-IDNUM/w1_slave"
  IDNUM is read from ds18b20's rom, the value is the only one

 ***************************************************************************/

int Temp(char int_dec[20], int c_f_flag)
{
	int fd = -1, ret;
	char *tmp1, tmp2[10], ch='t';
	char devname_head[50] = "/sys/devices/w1_bus_master1/28-00000";
	char devname_end[10] = "/w1_slave";
	char dev_name[100];
	long value;
	int integer, decimal;
	char buffer[100];
	int i, j;
	int f, g;

	//int sleep_time;
	//sleep_time = (30 * 1000) * 1000; // 1000 gives miliseconds so need to multiply another 1000 to get seconds
	strcpy(dev_name, devname_head);
	strcat(dev_name, "51091c5");
	strcat(dev_name, devname_end);
	if ((fd = open(dev_name, O_RDONLY)) < 0)
	{
		perror("temperature - open error");
		exit(1);
	}
	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0)
	{
		perror("temperature - read error");
		exit(1);
	}
	tmp1 = strchr(buffer, ch);
	sscanf(tmp1, "t=%s", tmp2);
	value = atoi(tmp2);
	integer = value / 1000;
	decimal = value % 1000;
	if (c_f_flag == 1)
	{
		f = ((integer * 9) / 5) + 32;
		integer = f;
		g = ((decimal * 9) / 5) + 32;
		decimal = g;
	}
	sprintf(int_dec, "%d.%04d", integer, decimal);
	//sprintf(int_dec, "%d.%d C", integer, decimal);
	close(fd);
	//printf("temperature is %d.%d\n", integer, decimal);
	return 0;
}

int main(int argc, char **argv)
{
	int integer, decimal;
	int err_no;
	char temp_c[20];
	char buf[32];
	int i, j;
	int lcd;
	int lcdFD;
	int c_f_flag;

	struct tm *t;
	time_t tim;

	//struct timespec ts;
	//ts.tv_sec=30;
	//ts.tv_nsec = 1000000L;

	c_f_flag = ((argc > 1) && (strcasecmp(argv[1], "F") == 0) ? 1 : 0);
	if (wiringPiSetup() < 0)
	{
	    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
	    return 1;
	}
	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
//	lcdPosition(lcdFD, 0,0);
//	lcdPrintf(lcdFD,"Wiggins Temp:");
//	lcdPrintf(lcdFD,"Waveshare");
	while (1)
	{
		err_no = Temp(temp_c, c_f_flag);
		/*
		// Clear the top display (remove any F)
		for (j = 0; j < 16; j++)
		{
			lcdPosition(lcdFD, j,0);
 			lcdPrintf(lcdFD, " ");
		}
		*/
		if (c_f_flag == 1)
			strcat(temp_c, " F");
		else
			strcat(temp_c, " C");
		lcdPosition(lcdFD, 0,0);
		lcdPrintf(lcdFD, temp_c);
		tim = time (NULL);
		t = localtime (&tim);
		sprintf (buf, "%02d:%02d:%02d %02d/%02d",
			t->tm_hour, t->tm_min, t->tm_sec, t->tm_mday, t->tm_mon + 1);

		//sprintf (buf, "%02d:%02d:%02d %02d/%02d/%02d", t->tm_hour, t->tm_min, t->tm_sec, t->tm_mday, t->tm_mon + 1, t->tm_year-100);
	//	sprintf (buf, "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year+1900);
		lcdPosition (lcdFD, 0,1);
		lcdPuts     (lcdFD, buf);
//		nanosleep(&ts, (struct timespec *)NULL); // maybe pause for 30 seconds?
	}
//	lcdPrintf(lcdFD, "Pi");
/*
	lcdPosition(lcdFD, 0,0);
	lcdPrintf(lcdFD,"             ");
	lcdPosition(lcdFD, 0,1);
	lcdPrintf(lcdFD,"             ");
	*/
	return 0;
}
