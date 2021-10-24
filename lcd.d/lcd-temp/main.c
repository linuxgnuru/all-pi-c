 
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


//int main(int argc, char *argv[])
int Temp(char int_dec[8])
{
	int fd = -1, ret;
	char *tmp1, tmp2[10], ch='t';
	char devname_head[50] = "/sys/devices/w1_bus_master1/28-00000";
	char devname_end[10] = "/w1_slave";
	char dev_name[100];
	long value;
	int integer, decimal;
	char buffer[100];
	int i,j;
	//int sleep_time;

	//sleep_time = (30 * 1000) * 1000; // 1000 gives miliseconds so need to multiply another 1000 to get seconds
	strcpy(dev_name, devname_head);
	strcat(dev_name, "51091c5");
	strcat(dev_name, devname_end);

	if ((fd = open(dev_name, O_RDONLY)) < 0)
	{
		perror("open error");
		exit(1);
	}

	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0)
	{
		perror("read error");
		exit(1);
	}

	tmp1 = strchr(buffer, ch);
	sscanf(tmp1, "t=%s", tmp2);

	value = atoi(tmp2);
	integer = value / 1000;
	decimal = value % 1000;
	sprintf(int_dec, "%d.%d C", integer, decimal);
	close(fd);

	//printf("temperature is %d.%d\n", integer, decimal);

	return 0;
}


int main(int argc, char **argv)
{
	int integer, decimal;
	int err_no;
	char temp_c[8];
	struct timespec ts;

	if (argc > 1)
	{
	}

	ts.tv_sec=30;
	ts.tv_nsec = 1000000L;
	if (wiringPiSetup() < 0)
	{
	    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
	    return 1 ;
	}

	int lcdFD;

	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	lcdPosition(lcdFD, 0,0);
	lcdPrintf(lcdFD,"Wiggins Temp:");
//	lcdPrintf(lcdFD,"Waveshare");

	while (1)
	{
		err_no = Temp(temp_c);
		lcdPosition(lcdFD, 0,1);
		lcdPrintf(lcdFD, temp_c);
		nanosleep(&ts, (struct timespec *)NULL); // maybe pause for 30 seconds?
	}
//	lcdPrintf(lcdFD, "Pi");
	lcdPosition(lcdFD, 0,0);
	lcdPrintf(lcdFD,"             ");
	lcdPosition(lcdFD, 0,1);
	lcdPrintf(lcdFD,"             ");
	return 0;
}

