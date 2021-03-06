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

#include <wiringPi.h>
#include <lcd.h>

#define MY_8591 100

const int RS = 3;	// 
const int EN = 14;	// 
const int D0 = 4;	// 
const int D1 = 12;	// 
const int D2 = 13;	// 
const int D3 = 6;	// 

int rev(int value) { return (abs(value - 255)); }

int main(int argc, char **argv)
{
	int pin;
	int lcd_fd;
	int delay_n = 600;
	int addr = 0x48;
	char buf[32];
	int value;

	pin = (argc > 1 ? atoi(argv[1]) : 0);
	if (wiringPiSetup() < 0)
	{
	    fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
	    return 1;
	}
	// setup analog input
	pcf8591Setup(MY_8591, addr);
	// setup LCD
	lcd_fd = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	lcdHome(lcd_fd);
	lcdPrintf(lcd_fd,"LDR value:");
	while (1)
	{
		value = rev(analogRead(MY_8591 + pin));
		sprintf(buf, "%03d", value);
		lcdPosition(lcd_fd, 11, 0);
		lcdPrintf(lcd_fd, buf);
		delay(delay_n);
	}
	// clear the LCD display
	lcdClear(lcd_fd);
	return 0;
}

