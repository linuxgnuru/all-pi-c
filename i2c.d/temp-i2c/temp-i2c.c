#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <wiringPi.h>

#define ADDRESS 0x04

int tmp_reading;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

static const char *devName = "/dev/i2c-1";

static const int ledsPins[] = { 0, 1, 2, 3, 4, 5, 6 };

static const int leds[10][7] = {
				{ 0, 1, 2, 7, 4, 5, 6 }, // 0
				{ 7, 7, 2, 7, 7, 5, 7 }, // 1
				{ 0, 7, 2, 3, 4, 7, 6 }, // 2
				{ 0, 7, 2, 3, 7, 5, 6 }, // 3
				{ 7, 1, 2, 3, 7, 5, 0 }, // 4
				{ 0, 1, 7, 3, 7, 5, 6 }, // 5
				{ 0, 1, 7, 3, 4, 5, 6 }, // 6
				{ 0, 7, 2, 7, 7, 5, 0 }, // 7
				{ 0, 1, 2, 3, 4, 5, 6 }, // 8
				{ 0, 1, 2, 3, 7, 5, 6 }  // 9
				};


int usage(const char *progName)
{
	fprintf(stderr, "Usage: %s [commands to send to Arduino]\n", progName);
	return EXIT_FAILURE;
}

int main(int argc, char **argv)
{
	int file;
	int arg;
	int val;
	int i;
	int j;
	int temp;
	int btnCtr;
	int buttonState;

	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	pinMode(7, INPUT);
	pullUpDnControl(7, PUD_UP);
	pinMode(28, OUTPUT);
	digitalWrite(28, LOW);
	for (i = 0; i < 7; i++)
		pinMode(i, OUTPUT);
	//if (argc == 1) return usage(argv[0]);
	if ((file = open(devName, O_RDWR)) < 0)
	{
		fprintf(stderr, "I2C: Failed to access %d\n", devName);
		exit(1);
	}
	printf("I2C: Acquiring bus to 0x%x\n", ADDRESS);
	if (ioctl(file, I2C_SLAVE, ADDRESS) < 0)
	{
		fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
		exit(1);
	}
	while (1)
	{
		unsigned char cmd[16];
		//if (0 == sscanf(argv[arg], "%d", &val))
		//printf("Sending %d\n", val);
		cmd[0] = 1;
		if (write(file, cmd, 1) == 1)
		{
			/*
			 * As we are not talking to direct hardware but a microntroller we
			 * need to wait a short while so that it can respond.
			 *
			 * 1 ms seems to be enough but it depends on what workload it has
			 */
			usleep(10000);
			char buf[1];
			if (read(file, buf, 1) == 1)
			{
			 	temp = (int)buf[0];
				if (temp != 11 && temp != 42)
				{
					for (i = 0; i < 7; i++)
					{
						if (leds[temp][i] != 7) digitalWrite(leds[temp][i], HIGH);
						else digitalWrite(ledsPins[i], LOW);
					}
				}
				// Turn off all LEDs
				if (temp == 11) for (i = 0; i < 7; i++) digitalWrite(i, LOW);
				// Turn on all LEDs
				if (temp == 42)
				{
					for (i = 0; i < 7; i++)
					{
						digitalWrite(i, HIGH);
						delay(100);
					}
				}
				if (temp == 70)
					digitalWrite(28, !digitalRead(28));
			}
			// now wait else you could crash the arduino by sending requests too fast
			usleep(10000);
		}
  		tmp_reading = digitalRead(7);
		if (tmp_reading != lastButtonState)
		lastDebounceTime = millis();
		if ((millis() - lastDebounceTime) > debounceDelay)
		{
			if (tmp_reading != buttonState)
			{
				buttonState = tmp_reading;
				if (buttonState == HIGH)
				{
					//printf("button pressed\n");
					cmd[0] = 2;
					write(file, cmd, 1);
					digitalWrite(28, !digitalRead(28));
					// if (write(file, cmd, 1) == 1) { }
				}
			}
		}
		lastButtonState = tmp_reading;
	}
	close(file);
	return EXIT_SUCCESS;
}
