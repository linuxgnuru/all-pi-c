#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <linux/input.h>
#include <linux/uinput.h>

#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
} while (0)

// catch signals and clean up GPIO
static void sDie(int sig)
{
	if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
	if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int i;
	int fd;
	struct uinput_user_dev uidev;
	struct input_event ev;
	int dx, dy;
	char buf[80];

	// note: we're assuming BSD-style reliable signals here
	(void)signal(SIGINT, sDie);
	(void)signal(SIGHUP, sDie);
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
	if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
	{
		sprintf(buf, "[%d, %s] error: ioctl", __LINE__, __FILE__);
		die(buf);
	}
	if (ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
	{
		sprintf(buf, "[%d, %s] error: ioctl", __LINE__, __FILE__);
		die(buf);
	}
	if (ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
	{
		sprintf(buf, "[%d, %s] error: ioctl", __LINE__, __FILE__);
		die(buf);
	}
	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1;
	uidev.id.product = 0x1;
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
	srand(time(NULL));
	while (1)
	{
		switch (rand() % 4)
		{
			case 0: dx = -10; dy =  -1; break;
			case 1: dx =  10; dy =   1; break;
			case 2: dx =  -1; dy =  10; break;
			case 3: dx =   1; dy = -10; break;
		}
		for (i = 0; i < 20; i++)
		{
			memset(&ev, 0, sizeof(struct input_event));
			ev.type = EV_REL;
			ev.code = REL_X;
			ev.value = dx;
			if (write(fd, &ev, sizeof(struct input_event)) < 0)
			{
				sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
				die(buf);
			}
			memset(&ev, 0, sizeof(struct input_event));
			ev.type = EV_REL;
			ev.code = REL_Y;
			ev.value = dy;
			if (write(fd, &ev, sizeof(struct input_event)) < 0)
			{
				sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
				die(buf);
			}
			memset(&ev, 0, sizeof(struct input_event));
			ev.type = EV_SYN;
			ev.code = 0;
			ev.value = 0;
			if (write(fd, &ev, sizeof(struct input_event)) < 0)
			{
				sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
				die(buf);
			}
			usleep(15000);
		}
		sleep(5);
	}
	sleep(2);
	if (ioctl(fd, UI_DEV_DESTROY) < 0)
	{
		sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__);
		die(buf);
	}
	close(fd);
	return EXIT_SUCCESS;
}
