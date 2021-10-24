 
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
#include <string.h> 
#include <errno.h>

#include<wiringPi.h>
#include<wiringPiI2C.h>
//#include <pcf8574.h>

int devid = 0x20;   //you cam use "i2cdetect" to find this devid
/*
int pinBase = 65;
*/
int fd;	// 

int main()
{
/*
	int ret;

	pcf8574Setup(pinBase, devid);
	while (1)
	{
		ret = myDigitalRead(fd, 
		printf("read from I2C: %d\n", ret);
	}
*/
	if ((fd=wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf (stderr, "Unable to open I2C device: %s\n", strerror (errno));
	    return 1;
	}
	if ((wiringPiI2CWrite(fd,0xff)) < 0)
	{
	    fprintf (stderr, "write badly %s\n", strerror (errno));
	    return 1;
	}
	delay(500);
	if ((wiringPiI2CWrite(fd,0)) < 0)
	{
	    fprintf (stderr, "write badly %s\n", strerror (errno));
	    return 1;
	}
	delay(500);
	printf("read from I2C: %d\n", wiringPiI2CRead(fd)); 
	return 0;
}

/*
#include <bcm2835.h>
#include <stdio.h>

#define KEY0 RPI_GPIO_P1_11
#define KEY1 RPI_GPIO_P1_12
#define KEY2 RPI_V2_GPIO_P1_13
#define KEY3 RPI_GPIO_P1_15
#define KEY4 RPI_GPIO_P1_16
#define KEY5 RPI_GPIO_P1_18
#define KEY6 RPI_GPIO_P1_22
#define KEY7 RPI_GPIO_P1_07
char KEY[]={KEY0,KEY1,KEY2,KEY3,KEY4,KEY5,KEY6,KEY7};
int main(int argc, char **argv)
{
    uint8_t value,i;
    if (!bcm2835_init())
        return 1;
    for(i=0;i<8;i++)
    {
    bcm2835_gpio_fsel(KEY[i], BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(KEY[i], BCM2835_GPIO_PUD_UP);
    }
    while (1)
    {
        for(i=0;i<8;i++)
	{
	if(bcm2835_gpio_lev(KEY[i])==0)        
            {
              printf("press the key: %d\n", i);
              delay(500);
            }
	}
	
    }
    bcm2835_close();
    return 0;
}
*/
