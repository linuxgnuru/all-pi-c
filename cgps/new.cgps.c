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

static latFlag = 1;
static lonFlag = 1;
static altFlag = 1;

#define CGPS_QUIT	0	/* voluntary yterminastion */
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

int update_gps_panel(struct gps_data_t *gpsdata)
{
    int i, j, n;
    int newstate;
    //char scr[128], *s;
    char lat[12];//, *s;
    char lon[12];//, *s;
    char alt[16];
    bool usedflags[MAXCHANNELS];
    int bad = 0;
    char ns;
    char ew;
    // must build bit vector of which statellites are used from list
    for (i = 0; i < MAXCHANNELS; i++)
    {
		usedflags[i] = false;
		for (j = 0; j < gpsdata->satellites_used; j++)
		    if (gpsdata->used[j] == gpsdata->PRN[i])
				usedflags[i] = true;
    }
    /*
    // Print time/date.
    if (isnan(gpsdata->fix.time) == 0)
		(void)unix_to_iso8601(gpsdata->fix.time, scr, sizeof(scr));
    else bad++;
    if (bad == 0) printf("%s\n", scr);
    */
    // Fill in the latitude.
    if (gpsdata->fix.mode >= MODE_2D && isnan(gpsdata->fix.latitude) == 0)
    {
        ns = (gpsdata->fix.latitude < 0) ? 'S' : 'N';
		(void)snprintf(lat, sizeof(lat), "%s", deg_to_str(deg_type, fabs(gpsdata->fix.latitude)));
    }
    else bad++;
    //if (bad == 0) printf("%s %c\n", scr, ns_ew);
    // Fill in the longitude.
    if (gpsdata->fix.mode >= MODE_2D && isnan(gpsdata->fix.longitude) == 0)
    {
		ew = (gpsdata->fix.longitude < 0) ? 'W' : 'E';
		(void)snprintf(lon, sizeof(lon), "%s", deg_to_str(deg_type, fabs(gpsdata->fix.longitude)));
    }
    else bad++;
    //if (bad == 0) printf("%s %c\n", scr, ns_ew);
    // Fill in the altitude.
    if (gpsdata->fix.mode >= MODE_3D && isnan(gpsdata->fix.altitude) == 0)
    {
		(void)snprintf(alt, sizeof(alt), "%.1f %s", gpsdata->fix.altitude * altfactor, altunits);
    }
    else bad++;
    //if (bad == 0) printf("%s\n", scr);
    if (bad == 0)
    {
	if (latFlag == 1)
		printf("%s %c\n", trim(lat), ns);
	if (lonFlag == 1)
		printf("%s %c\n", trim(lon), ew);
	//printf("%s %c %s %c\n", lat, ns, lon, ew);
	if (altFlag == 1)
		printf("%s\n", alt);
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
	//char fu[] = "  hello world";

    if (argc == 2)
    {
	    if (strcmp(argv[1], "lat") == 0)
		    latFlag = 1;
	    else
		    latFlag = 0;
	    if (strcmp(argv[1], "lon") == 0)
		    lonFlag = 1;
	    else
		    lonFlag = 0;
	    if (strcmp(argv[1], "alt") == 0)
		    altFlag = 1;
	    else
		    altFlag = 0;
    }
    if (argc == 3)
    {
	    if (strcmp(argv[1], "lat") == 0 || strcmp(argv[2], "lat") == 0)
		    latFlag = 1;
	    else
		    latFlag = 0;
	    if (strcmp(argv[1], "lon") == 0 || strcmp(argv[2], "lon") == 0)
		    lonFlag = 1;
	    else
		    lonFlag = 0;
	    if (strcmp(argv[1], "alt") == 0 || strcmp(argv[2], "alt") == 0)
		    altFlag = 1;
	    else
		    altFlag = 0;
    }
    if (argc == 4)
    {
	    if (strcmp(argv[1], "lat") == 0 || strcmp(argv[2], "lat") == 0 || strcmp(argv[3], "lat") == 0)
		    latFlag = 1;
	    else
		    latFlag = 0;
	    if (strcmp(argv[1], "lon") == 0 || strcmp(argv[2], "lon") == 0 || strcmp(argv[3], "lon") == 0)
		    lonFlag = 1;
	    else
		    lonFlag = 0;
	    if (strcmp(argv[1], "alt") == 0 || strcmp(argv[2], "alt") == 0 || strcmp(argv[3], "alt") == 0)
		    altFlag = 1;
	    else
		    altFlag = 0;
    }
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
	//printf("[%s] [%s]\n", fu, trim(fu));
    /* heart of the client */
    while (contFlag == 1)
    {
		if (!gps_waiting(&gpsdata, 5000000))
   		{
		    die(GPS_TIMEOUT);
		}
   		else
   		{
		    errno = 0;
		    if (gps_read(&gpsdata) == -1)
		    {
				fprintf(stderr, "cgps: socket error 4\n");
				die(errno == 0 ? GPS_GONE : GPS_ERROR);
		    }
		    else
		    {
			    i = update_gps_panel(&gpsdata);
		    }
		}
		if (i == 0)
			contFlag = 0;
    }
}
