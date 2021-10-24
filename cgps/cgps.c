#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#ifndef S_SPLINT_S
#include <unistd.h>
#endif /* S_SPLINT_S */

//#include "gpsd_config.h"
#include "gps.h"
#include "gpsdclient.h"
#include "revision.h"

static struct gps_data_t gpsdata;
static time_t status_timer;	/* Time of last state change. */
static int state = 0;		/* or MODE_NO_FIX=1, MODE_2D=2, MODE_3D=3 */
static float altfactor = METERS_TO_FEET;
static float speedfactor = MPS_TO_MPH;
static char *altunits = "ft";
static struct fixsource_t source;
static char *speedunits = "mph";

static bool raw_flag = false;
static bool silent_flag = false;
static bool magnetic_flag = false;
static int window_length;
static int display_sats;

#define CGPS_QUIT	0	/* voluntary termination */
#define GPS_GONE	-1	/* GPS device went away */
#define GPS_ERROR	-2	/* low-level failure in GPS read */
#define GPS_TIMEOUT	-3	/* low-level failure in GPS waiting */

static enum deg_str_type deg_type = deg_dd;

/* Function to call when we're all done.  Does a bit of clean-up. */
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

int update_gps_panel(struct gps_data_t *gpsdata)
{
    int i, j, n;
    int newstate;
    char scr[128], *s;
    bool usedflags[MAXCHANNELS];
    int bad = 0;
    /* must build bit vector of which statellites are used from list */
    for (i = 0; i < MAXCHANNELS; i++)
    {
	usedflags[i] = false;
	for (j = 0; j < gpsdata->satellites_used; j++)
	    if (gpsdata->used[j] == gpsdata->PRN[i])
		usedflags[i] = true;
    }
    /* Print time/date. */
    if (isnan(gpsdata->fix.time) == 0)
	(void)unix_to_iso8601(gpsdata->fix.time, scr, sizeof(scr));
    else
	   bad++;
    if (bad == 0)
	    printf("%s\n", scr);
    /* Fill in the latitude. */
    if (gpsdata->fix.mode >= MODE_2D && isnan(gpsdata->fix.latitude) == 0)
	(void)snprintf(scr, sizeof(scr), "%s %c", deg_to_str(deg_type, fabs(gpsdata->fix.latitude)), (gpsdata->fix.latitude < 0) ? 'S' : 'N');
    else
	   bad++;
    if (bad == 0)
    	printf("%s\n", scr);
    /* Fill in the longitude. */
    if (gpsdata->fix.mode >= MODE_2D && isnan(gpsdata->fix.longitude) == 0)
	(void)snprintf(scr, sizeof(scr), "%s %c", deg_to_str(deg_type, fabs(gpsdata->fix.longitude)), (gpsdata->fix.longitude < 0) ? 'W' : 'E');
    else
	    bad++;
    if (bad == 0)
    	printf("%s\n", scr);
    /* Fill in the altitude. */
    if (gpsdata->fix.mode >= MODE_3D && isnan(gpsdata->fix.altitude) == 0)
	(void)snprintf(scr, sizeof(scr), "%.1f %s", gpsdata->fix.altitude * altfactor, altunits);
    else
	    bad++;
    if (bad == 0)
	    printf("%s\n", scr);
    return bad;
}

int main(int argc, char *argv[])
{
    int option;
    int c;
    int i;
    int contFlag = 1;
    unsigned int flags = WATCH_ENABLE;

    switch (gpsd_units())
    {
	    case imperial: altfactor = METERS_TO_FEET; altunits = "ft"; speedfactor = MPS_TO_MPH; speedunits = "mph"; break;
	    case nautical: altfactor = METERS_TO_FEET; altunits = "ft"; speedfactor = MPS_TO_KNOTS; speedunits = "knots"; break;
	    case metric: altfactor = 1; altunits = "m"; speedfactor = MPS_TO_KPH; speedunits = "kph"; break;
	    default: /* leave the default alone */ break;
    }
    gpsd_source_spec(NULL, &source);
    /* Open the stream to gpsd. */
    if (gps_open(source.server, source.port, &gpsdata) != 0)
    {
	(void)fprintf(stderr, "cgps: no gpsd running or network error: %d, %s\n", errno, gps_errstr(errno));
	exit(2);
    }
    /* note: we're assuming BSD-style reliable signals here */
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    status_timer = time(NULL);
    if (source.device != NULL)
	flags |= WATCH_DEVICE;
    (void)gps_stream(&gpsdata, flags, source.device);
    /* heart of the client */
//    for (;;)
//    {
    while (contFlag == 1)
    {
	if (!gps_waiting(&gpsdata, 5000000))
	    die(GPS_TIMEOUT);
       	else
       	{
	    errno = 0;
	    if (gps_read(&gpsdata) == -1)
	    {
		fprintf(stderr, "cgps: socket error 4\n");
		die(errno == 0 ? GPS_GONE : GPS_ERROR);
	    }
	    else
		    i = update_gps_panel(&gpsdata);
	}
	if (i == 0)
		contFlag = 0;
    }
//    }
}
