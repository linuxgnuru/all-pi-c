/*
   0123456
 A .-
 B -...
 C -.-.
 D -..
 E .
 F ..-.
 G --.
 H ....
 I ..
 J .---
 K -.-
 L .-..
 M --
 N -.
 O ---
 P .--.
 Q --.-
 R .-.
 S ...
 T -
 U ..-
 V ...-
 W .--
 X -..-
 Y -.--
 Z --..
 1 .----
 2 ..---
 3 ...--
 4 ....-
 5 .....
 6 -....
 7 --...
 8 ---..
 9 ----.
 0 -----

*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/i2c-dev.h>
#include <wiringPi.h>

// morse code constants?



const int gpioPin0 = 17;
const int gpioPin1 = 18;
const int gpioPin2 = 27;
const int gpioPin3 = 22;
const int gpioPin4 = 23;
const int gpioPin5 = 24;
const int gpioPin6 = 25;
const int gpioPin7 = 4;


int main(int argc, char **argv)
{
	useconds_t delay = 2000;
	int i, j;

	wiringPiSetupGpio();
	pinMode(gpioPin0, OUTPUT); // GPIO 0
	pinMode(gpioPin1, OUTPUT); // GPIO 1
	pinMode(gpioPin2, OUTPUT); // GPIO 2
	pinMode(gpioPin3, OUTPUT); // GPIO 3
	pinMode(gpioPin4, OUTPUT); // GPIO 4
	pinMode(gpioPin5, OUTPUT); // GPIO 5
	pinMode(gpioPin6, OUTPUT); // GPIO 6
	pinMode(gpioPin7, OUTPUT); // GPIO 7
	for (;;)
	{
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
