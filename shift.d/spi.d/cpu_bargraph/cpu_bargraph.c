#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <unistd.h>
#include <sys/fcntl.h>

#define DO_LOOP
//#define DO_TOG

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

#define CE 0

// Anything under 80 is ok; but for this test I'll make the threshold 70
#define MAX_TEMP 70

int Temp()
{
	int fd = -1, ret;
	char devname[100] = "/sys/class/thermal/thermal_zone0/temp";
	char buffer[100];
	char errmsg[100];
	long value;
	int integer;

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
	close(fd);
	return integer;
}

unsigned char data[1] = { 0x0 };
unsigned char backup_data[1] = { 0x0 };

const int ledPins[] = { 2, 3 }; // for bar graph leds 9 and 10

void bitWrite(int n, int b) { if (n <= 7 && n >= 0) data[0] ^= (-b ^ data[0]) & (1 << n); }
void bitClear(int n) { if (n <= 7 && n >= 0) data[0] ^= (0 ^ data[0]) & (1 << n); }
void bitSet(int n) { if (n <= 7 && n >= 0) data[0] ^= (-1 ^ data[0]) & (1 << n); }

void onOff(_Bool b)
{
    int thisLed;
    int lednum;

    for (thisLed = 0; thisLed < 10; thisLed++)
    {
        if (thisLed < 8)
        {
            lednum = abs(thisLed - 7);
            bitWrite(lednum, b);
            backup_data[0] = data[0];
            wiringPiSPIDataRW(CE, data, 1);
            data[0] = backup_data[0];
        }
        else
            digitalWrite(ledPins[thisLed - 8], b);
    }
}

void doGraph(int num)
{
    int thisLed;
    int lednum;
    _Bool toggle;

    if (num < 0 || num > 10) return;
    if (num == 0) onOff(0);
    else if (num == 10) onOff(1);
    else
    {
        for (thisLed = 0; thisLed < 10; thisLed++)
        {
            lednum = thisLed;
            toggle = (thisLed < num);
            if (thisLed < 8)
            {
                lednum = abs(thisLed - 7);
                bitWrite(lednum, toggle);
                backup_data[0] = data[0];
                wiringPiSPIDataRW(CE, data, 1);
                data[0] = backup_data[0];
                //digitalWrite(ADDR + thisLed, toggle);
            }
            else
                digitalWrite(ledPins[thisLed - 8], toggle);
        }
    }
}

static double map(float x, float x0, float x1, float y0, float y1)
{
    float y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    double z = (double)y;
    return z;
}

static void die(int sig)
{
    data[0] = 0b00000000;
    wiringPiSPIDataRW(CE, data, 1);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(CE, 500000);
    pinMode(ledPins[0], OUTPUT);
    pinMode(ledPins[1], OUTPUT);
    digitalWrite(ledPins[0], LOW);
    digitalWrite(ledPins[1], LOW);
    while (1)
    {
        doGraph(map(Temp(), 0, 80, 0, 10));
        /*
        for (i = 0; i < 11; i++)
        {
            doGraph(i);
            delay(x);
        }
        for (i = 10; i > -1; i--)
        {
            doGraph(i);
            delay(x);
        }
        */
    }
    return EXIT_SUCCESS;
}

