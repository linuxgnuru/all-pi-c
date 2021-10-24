#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#ifndef S_SPLINT_S
#include <unistd.h>
#endif /* S_SPLINT_S */

#include <wiringPi.h>
#include <lcd.h>

#include "gps.h"
#include "gpsdclient.h"
#include "revision.h"

#define LATLON 1
#define ALTTIM 2

#define timeButtonPin 0 // GPIO 0, BCM 17
#define whatButtonPin 2 // GPIO 2, BCM 27
#define quitButtonPin 7 // GPIO 7, BCM  4

#define BTN_DELAY 30

int timeButtonState, whatButtonState, quitButtonState;
int timeLastButtonState, whatLastButtonState, quitLastButtonState;
long timeLastDebTime, whatLastDebTime, quitLastDebTime;
long debounceDelay = 50;

static struct gps_data_t gpsdata;
static time_t status_timer;	/* Time of last state change. */
static int state = 0;		/* or MODE_NO_FIX=1, MODE_2D=2, MODE_3D=3 */
static float altfactor = METERS_TO_FEET;
static float speedfactor = MPS_TO_MPH;
static char *altunits = "ft";
static struct fixsource_t source;
static char *speedunits = "mph";
int lcdFD;

#define CGPS_QUIT	0	/* voluntary yterminastion */
#define GPS_GONE	-1	/* GPS device went away */
#define GPS_ERROR	-2	/* low-level failure in GPS read */
#define GPS_TIMEOUT	-3	/* low-level failure in GPS waiting */

static enum deg_str_type deg_type = deg_dd;

const int RS = 3;	//
const int EN = 14;	//
const int D0 = 4;	//
const int D1 = 12;	//
const int D2 = 13;	//
const int D3 = 6;	//

// Function to call when we're all done.  Does a bit of clean-up.
static void die(int sig)
{
	(void)gps_close(&gpsdata);
	switch (sig)
	{
		case CGPS_QUIT: break;
		case GPS_GONE: (void)fprintf(stderr, "cgps: GPS hung up.\n"); break;
		case GPS_ERROR: (void)fprintf(stderr, "cgps: GPS read returned error\n"); break;
		case GPS_TIMEOUT: (void)fprintf(stderr, "cgps: GPS timeout\n"); break;
		default: (void)fprintf(stderr, "cgps: caught signal %d\n", sig); break;
	}
	exit(0);
}

char *trim(char *str)
{
	char *end;
	while (isspace(*str)) str++;
	if (*str == 0) return str;
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) end--;
	*(end+1) = 0;
	return str;
}

int update_gps_panel(struct gps_data_t *gpsdata, int disp)
{
	int i, j, n;
	int newstate;
	char lat[8];
	char lon[8];
	char alt[16];
	char buf[32];
	char tim[16];
	bool usedflags[MAXCHANNELS];
	int bad = 0;
	int noAlt = 0;
	char ns;
	char ew;
	for (i = 0; i < MAXCHANNELS; i++)
	{
		usedflags[i] = false;
		for (j = 0; j < gpsdata->satellites_used; j++)
			if (gpsdata->used[j] == gpsdata->PRN[i]) usedflags[i] = true;
	}
	// time
    if (isnan(gpsdata->fix.time) == 0)
	{
		(void)unix_to_iso8601(gpsdata->fix.time, tim, sizeof(tim));
	}
	else bad++;
	if (gpsdata->fix.mode >= MODE_2D && isnan(gpsdata->fix.latitude) == 0)
	{
		ns = (gpsdata->fix.latitude < 0) ? 'S' : 'N';
		(void)snprintf(lat, sizeof(lat), "%s", deg_to_str(deg_type, fabs(gpsdata->fix.latitude)));
	}
	else bad++;
	if (gpsdata->fix.mode >= MODE_2D && isnan(gpsdata->fix.longitude) == 0)
	{
		ew = (gpsdata->fix.longitude < 0) ? 'W' : 'E';
		(void)snprintf(lon, sizeof(lon), "%s", deg_to_str(deg_type, fabs(gpsdata->fix.longitude)));
	}
	else bad++;
	if (gpsdata->fix.mode >= MODE_3D && isnan(gpsdata->fix.altitude) == 0)
	{
		(void)snprintf(alt, sizeof(alt), "%.1f %s", gpsdata->fix.altitude * altfactor, altunits);
		noAlt = 0;
	}
	else noAlt = 1;
	if (bad == 0)
	{
		if (disp == LATLON)
		{
			sprintf(buf, "%s %c", trim(lat), ns);
			lcdPosition(lcdFD, 0, 0);
			lcdPuts(lcdFD, buf);
			sprintf(buf, "%s %c", trim(lon), ew);
			lcdPosition(lcdFD, 0, 1);
			lcdPuts(lcdFD, buf);
		}
		else
		{
			sprintf(buf, "%s", tim);
			lcdPosition(lcdFD, 0, 0);
			lcdPuts(lcdFD, buf);
			if (noAlt == 0)
				sprintf(buf, "%s", alt);
			else
				sprintf(buf, "No altitude");
			lcdPosition(lcdFD, 0, 1);
			lcdPuts(lcdFD, buf);
		}
	}
	return bad;
}

int main(int argc, char **argv)
{
	int option;
	int c;
	int i;
	int contFlag = 1;
	unsigned int flags = WATCH_ENABLE;
	int reading;
	int disp = LATLON;

	timeLastButtonState = whatLastButtonState = quitLastButtonState = HIGH;
	timeLastDebTime = whatLastDebTime = quitLastDebTime = 0;
	if (wiringPiSetup() < 0)
	{
		fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
		return 1;
	}
	pinMode(timeButtonPin, INPUT);
	pullUpDnControl(timeButtonPin, PUD_UP);
	pinMode(whatButtonPin, INPUT);
	pullUpDnControl(whatButtonPin, PUD_UP);
	pinMode(quitButtonPin, INPUT);
	pullUpDnControl(quitButtonPin, PUD_UP);
	lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
	if (lcdFD < 0)
	{
		fprintf(stderr, "%s: lcdInit failed\n", argv[0]);
		return -1;
	}
	lcdPosition(lcdFD, 0,0);
	switch (gpsd_units())
	{
		case imperial: altfactor = METERS_TO_FEET; altunits = "ft"; speedfactor = MPS_TO_MPH; speedunits = "mph"; break;
		case nautical: altfactor = METERS_TO_FEET; altunits = "ft"; speedfactor = MPS_TO_KNOTS; speedunits = "knots"; break;
		case metric: altfactor = 1; altunits = "m"; speedfactor = MPS_TO_KPH; speedunits = "kph"; break;
		default:  break;
	}
	gpsd_source_spec(NULL, &source);
	if (gps_open(source.server, source.port, &gpsdata) != 0)
	{
		(void)fprintf(stderr, "cgps: no gpsd running or network error: %d, %s\n", errno, gps_errstr(errno));
		exit(2);
	}
	(void)signal(SIGINT, die);
	(void)signal(SIGHUP, die);
	status_timer = time(NULL);
	if (source.device != NULL) flags |= WATCH_DEVICE;
	(void)gps_stream(&gpsdata, flags, source.device);
	while (1)
	{
		reading = digitalRead(timeButtonPin);
		if (reading != timeLastButtonState)
			timeLastDebTime = millis(); // reset the debouncing timer
		if ((millis() - timeLastDebTime) > debounceDelay)
		{
			if (reading != timeButtonState)
			{
				timeButtonState = reading;
				if (timeButtonState == LOW)
				{
					disp = (disp == LATLON ? ALTTIM : LATLON);
					printf("disp: %d\n", disp);
				}
			}
		}
		timeLastButtonState = reading;
		reading = digitalRead(quitButtonPin);
		if (reading != quitLastButtonState)
			quitLastDebTime = millis(); // reset the debouncing timer
		if ((millis() - quitLastDebTime) > debounceDelay)
		{
			if (reading != quitButtonState)
			{
				quitButtonState = reading;
				if (quitButtonState == LOW)
				{
					system("shutdown -h now");
				}
			}
		}
		quitLastButtonState = reading;
		//while (contFlag == 1)
		//{
			if (!gps_waiting(&gpsdata, 5000000)) die(GPS_TIMEOUT);
			else
			{
				errno = 0;
				if (gps_read(&gpsdata) == -1)
				{
					fprintf(stderr, "cgps: socket error 4\n");
					die(errno == 0 ? GPS_GONE : GPS_ERROR);
				}
				else i = update_gps_panel(&gpsdata, disp);
			}
			//if (i == 0) contFlag = 0;
		//}
		//contFlag = 1;
	}
	return 0;
}
