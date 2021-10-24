#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define MY_8574 100

//void pcf_toggle(int pin) { digitalWrite(MY_8574 + pin, !digitalRead(MY_8574 + pin)); }
int devid = 0x20;   //you cam use "i2cdetect" to find this devid
int fd;	// 

// catch signals and clean up GPIO
static void die(int sig)
{
	int i;
	//for (i = 0; i < 8; i++) digitalWrite(MY_8574 + i, LOW);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "\n\ncaught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "\n\nExiting due to Ctrl + C\n");
	printf("exit_failure: %d\n", EXIT_FAILURE);
	exit(0);
}

int main(int argc, char **argv)
{
	int pin;
	int err = 0;
	int i;

	/*
	if (argc > 1)
	{
		pin = atoi(argv[1]);
		if (pin < 0 || pin > 7) err = 1;
	}
	else
	       	err = 1;
	if (err == 1)
	{
		printf("usage: %s PIN\nwhere PIN is a number from 0 - 7\n", argv[0]);
		return 0;
	}
	*/
	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	if (wiringPiSetup() == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	/*
	 * START OF INIT
	 */
	if ((fd = wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	if ((wiringPiI2CWrite(fd, 0xff)) < 0)
	{
	    fprintf(stderr, "write badly %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	//delay(500);
	delay(50);
	if ((wiringPiI2CWrite(fd, 0)) < 0)
	{
	    fprintf(stderr, "write badly %s\n", strerror(errno));
	    return EXIT_FAILURE;
	}
	//delay(500);
	delay(50);
	err = wiringPiI2CRead(fd);
	/*
	 * END OF INIT
	 */
	pcf8574Setup(MY_8574, devid);
	//for (i = 0; i < 8; i++) pinMode(i, INPUT);
	//pcf_toggle(pin);
	for (i = 0; i < 8; i++)
	{
		printf("%d %s\n", i, (digitalRead(MY_8574 + i) == HIGH ? "HIGH" : "LOW"));
	}
	return 0;
}
