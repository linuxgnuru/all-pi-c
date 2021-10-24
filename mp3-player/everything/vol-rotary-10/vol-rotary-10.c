#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <ncurses.h>
// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "rotaryencoder.h"

#define exp10(x) (exp((x) * log(10)))

#define FALSE 0
#define TRUE 1

#define CE 1

const int extraLedPins[] = { 29, 28 }; // for bar graph leds 9 and 10

const int rot_enc_A = 23;
const int rot_enc_B = 27;
const int rot_enc_S = 24;

int  buttonState; // current reading from the input pin
int  lastButtonState = LOW; // previous reading from the input pin
long lastDebounceTime = 0; // last time the output pin was toggled
const long debounceDelay = 50; // debounce time; increase if the output flickers

static char card[64] = "hw:0";
snd_mixer_t *handle = NULL;
snd_mixer_elem_t *elem = NULL;

unsigned char data[1] = { 0x0 };
unsigned char backup_data[1] = { 0x0 };

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

void doGraph(int num)
{
    int thisLed;
    int lednum;
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
        if (handle != NULL) snd_mixer_close(handle);
        endwin();
        exit(1);
    }
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    endwin();
}

double map(float x, float x0, float x1, float y0, float y1)
{
    float y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    double z = (double)y;
    return z;
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

#if 0
void print_vol_bar(snd_mixer_elem_t *elem, struct lcd lcd)
{
    int volbar_length = rint(get_normalized_volume(elem) * (double)LCD_WIDTH-1);
    char volbar[LCD_WIDTH];

    int idx = 0;
    volbar[idx++] = '-';
    for (; idx < volbar_length; idx++)
        volbar[idx] = '=';
    for (; idx < LCD_WIDTH - 1; idx++)
        volbar[idx] = '_';
    volbar[LCD_WIDTH - 1] = '+';
    volbar[LCD_WIDTH] = '\0';
    lcd_string(lcd, volbar, 2);
}

void mute_unmute_callback(int state)
{
    if (state == 1)
    {
        int ival;
        printf("Toggling Mute\n");
        snd_mixer_selem_get_playback_switch(elem, 0, &ival);
        snd_mixer_selem_set_playback_switch(elem, 0, !ival);
    }
}

void set_mute_indicator(struct led mute_indicator)
{
    int ival;

    snd_mixer_selem_get_playback_switch(elem, 0, &ival);
    if (ival == 1)
        digitalWrite(29, HIGH);
    else
        digitalWrite(29, LOW);
}
#endif

int main()
{
    _Bool stopFlag = FALSE;
    int reading;
    int i;
    int myVol;

    (void)signal(SIGINT, die);
    //if (signal(SIGINT, die) == SIG_ERR) printf("\n Can't catch SIGINT. \n");
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pinMode(extraLedPins[0], OUTPUT);
    pinMode(extraLedPins[1], OUTPUT);
    wiringPiSPISetup(CE, 500000);
    pinMode(rot_enc_S, INPUT);
    pullUpDnControl(rot_enc_S, PUD_UP);
    piHiPri(99);
    struct encoder *vol_selector = setupEncoder(rot_enc_A, rot_enc_B);
    if (vol_selector == NULL)
        exit(1);
    int oldvalue = vol_selector->value;
    //   setup_button(24, mute_unmute_callback);
    snd_mixer_selem_id_t *sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
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
    //              01234567890123456
    mvaddstr(3, 0, "[--------------]");
    refresh();
    Off();
    while (stopFlag == FALSE)
    {
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
        if (oldvalue != vol_selector->value)
        {
            int change = vol_selector->value - oldvalue;
            int chn = 0;
            for (; chn <= SND_MIXER_SCHN_LAST; chn++)
            {
                double vol = get_normalized_volume(elem);
                //printf("Changing volume: %f\n", vol + (change * 0.00065105));
                move(2, 0);
                printw("Changing volume: %f\n", vol + (change * 0.00065105));
                set_normalized_volume(elem, vol + (change * 0.00065105));
                myVol = map(vol + (change * 0.00065105), 0, 1, 0, 13);
                //move(4, 1); printw("myVol: %2d", myVol);
                if (myVol == 13)
                {
                    move(3, 1);
                    printw("==============");
                }
                else
                {
                    for (i = 0; i < myVol + 1; i++)
                    {
                        move(3, 1 + i);
                        printw("=");
                    }
                    // myVol + 1
                    for (i = myVol; i < 14; i++)
                    {
                        move(3, 1 + i);
                        printw("-");
                    }
                }
                myVol = map(vol + (change * 0.00065105), 0, 1, 0, 10);
                //if (myVol < 0) myVol = 0;
                //if (myVol > 10) myVol = 10;
                doGraph(myVol);

                //printw("myVol: %d\n", myVol);
                /*
                for (i = 0; i < myVol + 1; i++)
                    digitalWrite(MY_8574 + i, HIGH);
                for (i = myVol + 1; i < 8; i++)
                    digitalWrite(MY_8574 + i, LOW);
                    */
                refresh();
            }
            oldvalue = vol_selector->value;
        }
        //        print_vol_bar(elem,lcd);
        //        set_mute_indicator(mute_indicator);
        //delay(25);
    }
    Off();
    endwin();
    //for (i = 0; i < 8; i++) digitalWrite(MY_8574 + i, LOW);
    //    if (sid != NULL) snd_mixer_selem_id_free(sid);
    if (handle != NULL) snd_mixer_close(handle);
    //    system("shutdown -h now &");
    return 0;
}

