#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include <wiringPi.h>
#include <sr595.h>

#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
} while (0)

const int leds[17][8] = {
			{  0, 30,  2,  3,  4,  5,  6, 30 }, // 0
			{ 30, 30, 30, 30,  4, 30,  6, 30 }, // 1
			{ 30,  1,  2,  3, 30,  5,  6, 30 }, // 2
			{ 30,  1, 30,  3,  4,  5,  6, 30 }, // 3
			{  0,  1, 30, 30,  4, 30,  6, 30 }, // 4
			{  0,  1, 30,  3,  4,  5, 30, 30 }, // 5
			{  0,  1,  2,  3,  4,  5, 30, 30 }, // 6
			{ 30, 30, 30, 30,  4,  5,  6, 30 }, // 7
			{  0,  1,  2,  3,  4,  5,  6, 30 }, // 8
			{  0,  1, 30,  3,  4,  5,  6, 30 }, // 9
			{  0,  1,  2, 30,  4,  5,  6, 40 }, // A.
			{  0,  1,  2,  3,  4,  5,  6, 40 }, // B. (8)
			{  0, 30,  2,  3, 30,  5, 30, 40 }, // C.
			{  0, 30,  2,  3,  4,  5,  6, 40 }, // D. (0) 
			{  0,  1,  2, 30,  4, 30,  6, 40 }, // #.
			{ 30,  1, 30,  3, 30,  5, 30, 40 }, // *.
			{  0,  1,  2,  3, 30,  5, 30, 40 }  // E. (error)
		};

const char MATRIX[4][4] = {
			{ '1','2','3','A' },
			{ '4','5','6','B' },
			{ '7','8','9','C' },
			{ '*','0','#','D' }
	       	};

// Pins for keypad
const int ROW[] = { 0, 1, 2, 3 };
const int COL[] = { 4, 5, 6, 7 };

// Pins for LEDs
const int okPin = 21;
const int noPin = 22;

// Pins for shift register
const int dataPin  = 23; // blue (pin 14)
const int latchPin = 24; // green (pin 12)
const int clockPin = 25; // yellow (pin 11)

// base address to use (i.e. 100 = pin 0, 102 = pin 2, etc)
const int addr = 100;

// Password
const char pass[5] = { '1', '7', '0', '1', 'D' };

// device
int fd;

const int butPin = 29;

// catch signals and clean up GPIO
static void sDie(int sig)
{
	char buf[80];
	int i;

	digitalWrite(okPin, LOW);
	digitalWrite(noPin, LOW);
	for (i = 0; i < 8; i++)
	       	digitalWrite(addr + i, 0);
	for (i = 0; i < 4; i++)
	       	digitalWrite(COL[i], LOW);
	if (sig != 0 && sig != 2)
	       	(void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2)
	       	(void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	if (ioctl(fd, UI_DEV_DESTROY) < 0)
	{
		sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
		die(buf);
	}
	close(fd);
	exit(0);
}

void cls()
{
	int i;
	for (i = 0; i < 8; i++) digitalWrite(addr + i, 0);
       	//printf("\033[2J\033[H");
}

void pDigit(char *comp)
{
	int i, digit;
	// clear the display
	for (i = 0; i < 8; i++)
	       	digitalWrite(addr + i, LOW);
	if (strcmp(comp, "#") == 0 || strcmp(comp, "*") == 0 || strcasecmp(comp, "A") == 0 || strcasecmp(comp, "B") == 0 || strcasecmp(comp, "C") == 0 || strcasecmp(comp, "D") == 0 || strcasecmp(comp, "E") == 0)
	{
		if (strcmp(comp, "#") == 0) digit = 14;
		else if (strcmp(comp, "*") == 0) digit = 15;
		else if (strcasecmp(comp, "A") == 0) digit = 10;
		else if (strcasecmp(comp, "B") == 0) digit = 11;
		else if (strcasecmp(comp, "C") == 0) digit = 12;
		else if (strcasecmp(comp, "D") == 0) digit = 13;
		else if (strcasecmp(comp, "E") == 0) digit = 16;
	}
	else
	       	digit = atoi(comp);
	for (i = 0; i < 8; i++)
	{
		if (digit == 60) digitalWrite(addr + i, LOW);
		else
		{
	       		if (leds[digit][i] != 30) digitalWrite(addr + leds[digit][i], HIGH);
	       		if (leds[digit][7] == 40) digitalWrite(addr + 7, HIGH);
			else digitalWrite(addr + 7, LOW);
		}
	}
}

int main(int argc, char **argv)
{
	int i, j, k;
	struct uinput_user_dev uidev;
	struct input_event ev;
	int dx, dy;
	int pNum = 0;
	int loopFlag = 1;
	int numTries = 0;
	char buf[80];
	char new_m[2];

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, sDie);
	(void)signal(SIGHUP, sDie);
	if (wiringPiSetup() == -1)
	{
		fprintf(stderr, "oops: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	sr595Setup(addr, 8, dataPin, clockPin, latchPin);
	pinMode(okPin, OUTPUT);
	pinMode(noPin, OUTPUT);
	digitalWrite(okPin, LOW);
	digitalWrite(noPin, LOW);
	for (i = 0; i < 4; i++)
	       	pinMode(COL[i], OUTPUT);
	for (i = 0; i < 4; i++)
	{
		pinMode(ROW[i], INPUT);
		pullUpDnControl(ROW[i], PUD_UP);
	}
	// clear led
	for (i = 0; i < 8; i++)
	       	digitalWrite(addr + i, LOW);
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd < 0)
	{
		sprintf(buf, "[%d, %s] error: open", __LINE__, __FILE__);
		die(buf);
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
	{
		sprintf(buf, "[%d, %s] error: ioctl", __LINE__, __FILE__);
		die(buf);
	}
	//if (ioctl(fd, UI_SET_KEYBIT, KEY_O) < 0)
	if (ioctl(fd, UI_SET_KEYBIT, KEY_F2) < 0)
	{
		sprintf(buf, "[%d, %s] error: ioctl", __LINE__, __FILE__);
		die(buf);
	}
	/*
	if (ioctl(fd, UI_SET_KEYBIT, KEY_LEFTALT) < 0)
	{
		sprintf(buf, "[%d, %s] error: ioctl", __LINE__, __FILE__);
		die(buf);
	}
	*/
	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Wiggins-KeyPass");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1234;
	uidev.id.product = 0xfedc;
	uidev.id.version = 1;
	if (write(fd, &uidev, sizeof(uidev)) < 0)
	{
		sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
		die(buf);
	}
	if (ioctl(fd, UI_DEV_CREATE) < 0)
	{
		sprintf(buf, "[%d, %s] error: ioctl", __LINE__, __FILE__);
		die(buf);
	}
	sleep(2);
	//srand(time(NULL));
	cls();
	while (loopFlag)
	{
		for (i = 0; i < 4; i++)
		{
			digitalWrite(COL[i], LOW);
			for (j = 0; j < 4; j++)
			{
				if (digitalRead(ROW[j]) == LOW)
				{
					new_m[0] = MATRIX[j][i];
					new_m[1] = '\0';
					pDigit(new_m);
					if (MATRIX[j][i] == '#' || MATRIX[j][i] == '*')
					{
						digitalWrite(okPin, LOW);
						digitalWrite(noPin, LOW);
						for (k = 0; k < 8; k++)
							digitalWrite(addr + k, LOW);
					}
				       	if (MATRIX[j][i] == pass[pNum])
					       	pNum++;
					else
					{
						pNum = 0;
						digitalWrite(noPin, HIGH);
						new_m[0] = 'E';
						new_m[1] = '\0';
						pDigit(new_m);
						numTries++;
					}
					if (pNum >= 5 && digitalRead(noPin) != HIGH)
					       	loopFlag = 0;
					while (digitalRead(ROW[j]) == LOW)
					       	delay(20);
				}
			}
			digitalWrite(COL[i], HIGH);
		}
	}
	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_KEY;
	ev.code = KEY_F2;
	//ev.code = KEY_O;
	ev.value = 1;
	if (write(fd, &ev, sizeof(struct input_event)) < 0)
	{
		sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
		die(buf);
	}
	/*
	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_KEY;
	ev.code = KEY_LEFTALT;
	ev.value = 1;
	if (write(fd, &ev, sizeof(struct input_event)) < 0)
	{
		sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
		die(buf);
	}
	*/
	usleep(15000);
	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;
	if (write(fd, &ev, sizeof(struct input_event)) < 0)
	{
		sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
		die(buf);
	}
	//sleep(2);
	if (ioctl(fd, UI_DEV_DESTROY) < 0)
	{
		sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
		die(buf);
	}
	close(fd);
	//digitalWrite(okPin, HIGH);
	//delay(2000);
	digitalWrite(okPin, LOW);
	for (i = 0; i < 8; i++) digitalWrite(addr + i, LOW);
	return EXIT_SUCCESS;
}
