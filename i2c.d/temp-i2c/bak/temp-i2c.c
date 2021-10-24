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
	int temp;
	int btnCtr;
	int buttonState;

	if (wiringPiSetup () == -1)
  	{
	    fprintf(stdout, "oops: %s\n", strerror(errno));
	    return 1;
	}
	pinMode(29, INPUT);
	pullUpDnControl(29, PUD_UP);
	for (i = 0; i < 8; i++)
		pinMode(i, OUTPUT);
	pinMode(21, OUTPUT);
	pinMode(22, OUTPUT);
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
				switch (temp)
				{
					case 0:
						digitalWrite(22, HIGH);
						digitalWrite(21, LOW);
						for (i = 0; i < 8; i++) digitalWrite(i, LOW);
						break;
					case 1:
						digitalWrite(22, LOW);
						digitalWrite(21, HIGH);
						for (i = 0; i < 8; i++) digitalWrite(i, LOW);
						break;
					case 2:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						digitalWrite(7, HIGH);
						for (i = 0; i < 7; i++) digitalWrite(i, LOW);
						break;
					case 3:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						for (i = 0; i < 6; i++) digitalWrite(i, LOW);
						digitalWrite(6, HIGH);
						digitalWrite(7, LOW);
						break;
					case 4:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						for (i = 0; i < 5; i++) digitalWrite(i, LOW);
						digitalWrite(5, HIGH);
						digitalWrite(6, LOW);
						digitalWrite(7, LOW);
						break;
					case 5:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						for (i = 0; i < 4; i++) digitalWrite(i, LOW);
						digitalWrite(4, HIGH);
						for (i = 5; i < 8; i++) digitalWrite(i, LOW);
						break;
					case 6:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						for (i = 0; i < 3; i++) digitalWrite(i, LOW);
						digitalWrite(3, HIGH);
						for (i = 4; i < 8; i++) digitalWrite(i, LOW);
						break;
					case 7:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						for (i = 0; i < 2; i++) digitalWrite(i, LOW);
						digitalWrite(2, HIGH);
						for (i = 3; i < 8; i++) digitalWrite(i, LOW);
						break;
					case 8:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						for (i = 0; i < 1; i++) digitalWrite(i, LOW);
						digitalWrite(1, HIGH);
						for (i = 2; i < 8; i++) digitalWrite(i, LOW);
						break;
					case 9:
						digitalWrite(22, LOW);
						digitalWrite(21, LOW);
						for (i = 1; i < 8; i++) digitalWrite(i, LOW);
						digitalWrite(0, HIGH);
						break;
					case 11:
						for (i = 0; i < 8; i++) digitalWrite(i, LOW);
						digitalWrite(21, LOW);
						digitalWrite(22, LOW);
						break;
					case 42:
						for (i = 0; i < 8; i++)
						{
							digitalWrite(i, HIGH);
							delay(100);
						}
						digitalWrite(21, HIGH);
						delay(100);
						digitalWrite(22, HIGH);
						delay(100);
					default: break;
				}
				/*
				if (temp < 8) digitalWrite(temp, HIGH);
				else
				{
					if (temp == 8) digitalWrite(21, HIGH);
					if (temp == 9) digitalWrite(22, HIGH);
				}
				for (i = 0; i < 8; i++)
				{
					if (i != temp) digitalWrite(i, LOW);
				}
				if (temp != 8) digitalWrite(21, LOW);
				if (temp != 9) digitalWrite(22, LOW);
				//printf("Received %d\n", temp);
				if (temp == 11)
				{
					for (i = 0; i < 8; i++) digitalWrite(i, LOW);
					digitalWrite(21, LOW);
					digitalWrite(22, LOW);
				}
				if (temp == 42)
				{
					for (i = 0; i < 8; i++) digitalWrite(i, HIGH);
					digitalWrite(21, HIGH);
					digitalWrite(22, HIGH);
				}
				//printf("temp: %d\n", temp);
				*/
			}
			// now wait else you could crash the arduino by sending requests too fast
			usleep(10000);
		}
		/*
		btnCtr = 0;
		buttonState = digitalRead(29);
		if (buttonState == LOW)
		{
			for (i = 0; i < 3; i++)
			{
				buttonState = digitalRead(29);
				if (buttonState == LOW)
					btnCtr++;
				delay(50);
			}
		}
		if (btnCtr >= 3)
		{
			printf("button pressed\n");
			cmd[0] = 2;
			write(file, cmd, 1);
		}
		*/
  		tmp_reading = digitalRead(29);
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
					// if (write(file, cmd, 1) == 1) { }
				}
			}
		}
		lastButtonState = tmp_reading;
	}
	close(file);
	return EXIT_SUCCESS;
}
