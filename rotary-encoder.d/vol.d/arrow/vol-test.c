#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>

#include <wiringPi.h>
#include <lcd.h>

#include "rotaryencoder.h"

#define exp10(x) (exp((x) * log(10)))

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

const int BS = 4;	// Bits (4 or 8)
const int CO = 16;	// Number of columns
const int RO = 2;	// Number of rows
const int RS = 3;
const int EN = 14;
const int D0 = 4;
const int D1 = 12;
const int D2 = 13;
const int D3 = 6;

static int lcdFD;

static unsigned char blank[8] = 
{
	0b00000,
	0b00000,
	0b00010,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
};

static unsigned char head[8] = 
{
	0b10000,
	0b11100,
	0b11110,
	0b11111,
	0b11111,
	0b11100,
	0b11000,
	0b10000,
};

static unsigned char body[8] = 
{
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
};

int aPin = 16; // RxD (15 BCM)
int bPin = 15; // TxD (14 BCM)
int sPin =  8; // SDA ( 2 BCM)

int  buttonState; // current reading from the input pin
int  lastButtonState = LOW; // previous reading from the input pin
long lastDebounceTime = 0; // last time the output pin was toggled
const long debounceDelay = 50; // debounce time; increase if the output flickers

static char card[64] = "hw:0";
snd_mixer_t *handle = NULL;
snd_mixer_elem_t *elem = NULL;

void sig_handler(int signo)
{
    int i;
    if (signo == SIGINT)
    {
        printf("SIGINT recieved\n");
        if (handle != NULL)
            snd_mixer_close(handle);
        exit(1);
    }
}

double map(float x, float x0, float x1, float y0, float y1)
{
	float y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    double z = (double)y;
	return z;
}

/*
   The following code is borrowed from the MPD project.
   Info can be found here: http://theatticlight.net/posts/My-Embedded-Music-Player-and-Sound-Server
 */
// get the volume as a floating point number 0..1
double get_normalized_volume(snd_mixer_elem_t *elem)
{
    long max, min, value;
    int err;

    err = snd_mixer_selem_get_playback_dB_range(elem, &min, &max);
    if (err < 0)
    {
        printf("Error getting volume\n");
        return 0;
    }
    err = snd_mixer_selem_get_playback_dB(elem, 0, &value);
    if (err < 0)
    {
        printf("Error getting volume\n");
        return 0;
    }
    // Perceived 'loudness' does not scale linearly with the actual decible level
    // it scales logarithmically
    return exp10((value - max) / 6000.0);
}

// set the volume from a floating point number 0..1
void set_normalized_volume(snd_mixer_elem_t *elem, double volume)
{
    long min, max, value;
    int err;

    if (volume < 0.017170)
        volume = 0.017170;
    else if (volume > 1.0)
        volume = 1.0;
    err = snd_mixer_selem_get_playback_dB_range(elem, &min, &max);
    if (err < 0)
    {
        printf("Error setting volume\n");
        return;
    }
    // Perceived 'loudness' does not scale linearly with the actual decible level
    // it scales logarithmically
    value = lrint(6000.0 * log10(volume)) + max;
    snd_mixer_selem_set_playback_dB(elem, 0, value, 0);
}

void print_vol_bar(snd_mixer_elem_t *elem)
{
    int volbar_length = rint(get_normalized_volume(elem) * (double)CO-1);
    char volbar[CO];

    int idx = 0;
    int i;
    volbar[idx++] = '-';
    for (; idx < volbar_length; idx++)
        volbar[idx] = '=';
    volbar[volbar_length - 1] = '>';
    for (; idx < CO - 1; idx++)
        volbar[idx] = ' ';
    volbar[CO - 1] = '+';
    volbar[CO] = '\0';
	lcdCharDef(lcdFD, 2, head);
	lcdCharDef(lcdFD, 3, body);
	lcdCharDef(lcdFD, 4, blank);
 //   lcdPosition(lcdFD, 0, 1);
//    lcdPuts(lcdFD, "-               +");
    lcdPosition(lcdFD, 0, 1);
    lcdPuts(lcdFD, volbar);
}
#if 0
    for (i = 0; i < CO; i++)
    {
        if (volbar[i] == '=' && volbar[i + 1] == ' ')
        {
            lcdPosition(lcdFD, i + 1, 1);
            lcdPutchar(lcdFD, 2);
        }
//        else if (volbar[i] == '=') lcdPutchar(lcdFD, 3);
    }
}
#endif

void mute_unmute()
{
    int ival;
    snd_mixer_selem_get_playback_switch(elem, 0, &ival);
    if (ival == 1)
    {
        lcdPosition(lcdFD, 0, 0);
        lcdPrintf(lcdFD, " --  MUTED --  ");
    }
    else
    {
        lcdPosition(lcdFD, 0, 0);
        lcdPrintf(lcdFD, "               ");
    }
    snd_mixer_selem_set_playback_switch(elem, 0, !ival);
}

int main()
{
    int stopFlag = FALSE;
    int reading;
    int i;
    int myVol;

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\n Can't catch SIGINT. \n");
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
	lcdFD = lcdInit(RO, CO, BS, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
    pinMode(sPin, INPUT);
	pullUpDnControl(sPin, PUD_UP);
    piHiPri(99);
    struct encoder *vol_selector = setupencoder(aPin, bPin);
    if (vol_selector == NULL)
        exit(1);
    int oldvalue = vol_selector->value;
    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "PCM");
    if (snd_mixer_open(&handle, 0) < 0)
    {
        printf("Error openning mixer");
        exit(1);
    }
    if (snd_mixer_attach(handle, card) < 0)
    {
        printf("Error attaching mixer");
        snd_mixer_close(handle);
        exit(1);
    }
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0)
    {
        printf("Error registering mixer");
        snd_mixer_close(handle);
        exit(1);
    }
    if (snd_mixer_load(handle) < 0)
    {
        printf("Error loading mixer");
        snd_mixer_close(handle);
        exit(1);
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem)
    {
        printf("Error finding simple control");
        snd_mixer_close(handle);
        exit(1);
    }
    lcdPosition(lcdFD, 0, 1);
    while (stopFlag == FALSE)
    {
        reading = digitalRead(sPin);
        if (reading != lastButtonState)
          lastDebounceTime = millis();
        if ((millis() - lastDebounceTime) > debounceDelay)
        {
          if (reading != buttonState)
          {
              buttonState = reading;
              if (buttonState == LOW)
                  mute_unmute();
//                  stopFlag = TRUE;
          }
        }
        lastButtonState = reading;
        if (oldvalue != vol_selector->value)
        {
            int change = vol_selector->value - oldvalue;
            int chn = 0;
            for (; chn <= SND_MIXER_SCHN_LAST; chn++)
            {
                double vol = get_normalized_volume(elem);
                //printf("Changing volume: %f\n", vol + (change * 0.00065105));
                set_normalized_volume(elem, vol + (change * 0.00065105));
/*
                myVol = map(vol + (change * 0.00065105), 0, 1, 0, 16);
                lcdPosition(lcdFD, 0, 0);
                lcdPrintf(lcdFD, "myVol: %d", myVol);
*/
            }
            oldvalue = vol_selector->value;
        }
        print_vol_bar(elem);
//        set_mute_indicator(mute_indicator);
        delay(25);
    }
//    if (sid != NULL) snd_mixer_selem_id_free(sid);
    if (handle != NULL) snd_mixer_close(handle);
    return 0;
}


