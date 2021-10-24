#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
// for signal catching
#include <errno.h>
#include <signal.h>

#include <ncurses.h>
#include <time.h>
#include <math.h>
#include <alsa/asoundlib.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "rotaryencoder.h"

#define CE 1

#define FALSE 0
#define TRUE 1

// for bar graph leds 9 and 10
const int extraLedPins[] = { 29, 28 };

const int rot_enc_A = 23;
const int rot_enc_B = 27;
const int rot_enc_S = 24;

int  buttonState; // current reading from the input pin
int  lastButtonState = LOW; // previous reading from the input pin
long lastDebounceTime = 0; // last time the output pin was toggled
const long debounceDelay = 50; // debounce time; increase if the output flickers

static char card[64] = "hw:0";
//static char card[64] = "default";
snd_mixer_t *handle = NULL;
snd_mixer_elem_t *elem = NULL;

unsigned char data[1] = { 0x0 };

unsigned char backup_data[1] = { 0x0 };

unsigned int lastMillis = 0;

void bitWrite(int n, int b) { if (n <= 7 && n >= 0) data[0] ^= (-b ^ data[0]) & (1 << n); }

void bitClear(int n) { if (n <= 7 && n >= 0) data[0] ^= (0 ^ data[0]) & (1 << n); }

void bitSet(int n) { if (n <= 7 && n >= 0) data[0] ^= (-1 ^ data[0]) & (1 << n); }

void Off()
{
    data[0] = 0x0;
    backup_data[0] = data[0];
    wiringPiSPIDataRW(CE, data, 1);
    data[0] = backup_data[0];
    digitalWrite(extraLedPins[0], LOW);
    digitalWrite(extraLedPins[1], LOW);
}

void On()
{
    data[0] = 0b11111111;
    backup_data[0] = data[0];
    wiringPiSPIDataRW(CE, data, 1);
    data[0] = backup_data[0];
    digitalWrite(extraLedPins[0], HIGH);
    digitalWrite(extraLedPins[1], HIGH);
}

double map(float x, float x0, float x1, float y0, float y1)
{
    float y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    double z = (double)y;
    return z;
}

void doGraph(int num)
{
    int thisLed, lednum;
    _Bool toggle;

    if (num < 0 || num > 10) return;
    if (num == 0) Off();
    else if (num == 10) On();
    else
    {
        for (thisLed = 0; thisLed < 10; thisLed++)
        {
            lednum = thisLed;
            toggle = (thisLed < num);
            if (thisLed < 8)
            {
                lednum = 7 - lednum;
                bitWrite(lednum, toggle);
                backup_data[0] = data[0];
                wiringPiSPIDataRW(CE, data, 1);
                data[0] = backup_data[0];
            }
            else
            {
                lednum -= 8;
                digitalWrite(extraLedPins[lednum], toggle);
            }
        }
    }
}

void die(int sig)
{
    Off();
    if (sig == SIGINT)
    {
        printf("SIGINT recieved\n");
        endwin();
        exit(1);
    }
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    else (void)fprintf(stderr, "Exiting due to %s\n", strsignal(sig));
    endwin();
}

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

    if (volume < 0.017170) volume = 0.017170;
    else if (volume > 1.0) volume = 1.0;
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

int main(int argc, char **argv)
{
    //int i;
    unsigned long currentMillis;
    int myVol = 0;
    _Bool stopFlag = FALSE;
//    int reading;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    pinMode(rot_enc_S, INPUT);
    pullUpDnControl(rot_enc_S, PUD_UP);
    pinMode(extraLedPins[0], OUTPUT);
    pinMode(extraLedPins[1], OUTPUT);
    wiringPiSPISetup(CE, 500000);
    //piHiPri(99);
    struct encoder *vol_selector = setupEncoder(rot_enc_A, rot_enc_B);
    if (vol_selector == NULL) exit(1);
    int oldvalue = vol_selector->value;
    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    //snd_mixer_selem_id_set_name(sid, "PCM");
    snd_mixer_selem_id_set_name(sid, "Speaker");
    if (snd_mixer_open(&handle, 0) < 0)
    {
        printf("Error openning mixer\n");
        exit(1);
    }
    if (snd_mixer_attach(handle, card) < 0)
    {
        printf("Error attaching mixer\n");
        snd_mixer_close(handle);
        exit(1);
    }
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0)
    {
        printf("Error registering mixer\n");
        snd_mixer_close(handle);
        exit(1);
    }
    if (snd_mixer_load(handle) < 0)
    {
        printf("Error loading mixer\n");
        snd_mixer_close(handle);
        exit(1);
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem)
    {
        printf("Error finding simple control\n");
        snd_mixer_close(handle);
        exit(1);
    }
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("Volume control");
    refresh();
    // set the leds to the current volume level
    for (int chn = 0; chn <= SND_MIXER_SCHN_LAST; chn++)
    {
        double vol = get_normalized_volume(elem);
        myVol = map(vol, 0, 1, 0, 10);
        doGraph(myVol);
        move(0, 4);
        printw("vol: %d", myVol);
        refresh();
    }

    Off();
    while (stopFlag == FALSE)
    {
        /*
        reading = digitalRead(rot_enc_S);
        if (reading != lastButtonState)
            lastDebounceTime = millis();
        if ((millis() - lastDebounceTime) > debounceDelay)
        {
            if (reading != buttonState)
            {
                buttonState = reading;
                if (buttonState == LOW)
                    stopFlag = TRUE;
            }
        }
        lastButtonState = reading;
*/
        currentMillis = millis();
        // if 8 seconds has passed; turn off the LEDs
        if (currentMillis - lastMillis >= 8000)
        {
            lastMillis = currentMillis;
            wiringPiSPIDataRW(CE, 0x0, 1);
            digitalWrite(extraLedPins[0], LOW);
            digitalWrite(extraLedPins[1], LOW);
        }
        if (oldvalue != vol_selector->value)
        {
            int change = vol_selector->value - oldvalue;
            move(2, 0);
            printw("Volume: %d\n", myVol);
            move(3, 0);
            printw("vol_selector->value: %d\n", vol_selector->value);
            move(4, 0);
            printw("change: %d\n", change);
            move(5, 0);
            printw("oldvalue: %d\n", oldvalue);
            //if (change == -1) myVol -= 100;
            //if (change == 1) myVol += 100;
            myVol = map( + (change * 0.00065105), 0, 1, 0, 10);
            move(6, 0);
            printw("myVol: %d\n", myVol);
            refresh();
            if (myVol < 0) myVol = 0;
            if (myVol > 10) myVol = 10;
            doGraph(myVol);
            //printf("oldvalue: %d\nchange: %d\n", oldvalue, change);
            for (int chn = 0; chn <= SND_MIXER_SCHN_LAST; chn++)
            {
                double vol = get_normalized_volume(elem);
                set_normalized_volume(elem, vol + (change * 0.00065105));
                myVol = map(vol + (change * 0.00065105), 0, 1, 0, 10);
                //if (myVol < 0) myVol = 0;
                //if (myVol > 10) myVol = 10;
                doGraph(myVol);
            }
            oldvalue = vol_selector->value;
        }
        //delay(25);
    }
    Off();
    endwin();
    /*
    if (argc == 1 || argc > 2)
    {
        printf("usage %s [num (0-8)]\n", argv[0]);
        return EXIT_FAILURE;
    }
    i = atoi(argv[1]);
    doGraph(i);
    */
    return EXIT_SUCCESS;
}

