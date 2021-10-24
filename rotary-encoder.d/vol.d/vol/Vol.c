#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <ncurses.h>

#include <wiringPi.h>

#include "rotaryencoder.h"

#define exp10(x) (exp((x) * log(10)))

//struct encoder encoder;

static char card[64] = "hw:0";
snd_mixer_t *handle = NULL;
snd_mixer_elem_t *elem = NULL;

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("SIGINT recieved\n");
        if (handle != NULL)
            snd_mixer_close(handle);
        endwin();
        exit(1);
    }
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
    printf("Starting...\n");
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\n Can't catch SIGINT. \n");
    wiringPiSetup();
    pinMode(29, OUTPUT);
    piHiPri(99);
    struct encoder *vol_selector = setupencoder(22, 23);
    if (vol_selector == NULL)
        exit(1);
    int oldvalue = vol_selector->value;
 //   setup_button(24, mute_unmute_callback);
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
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("rotary encoder");
    refresh();
    while (1)
    {
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
                refresh();
                set_normalized_volume(elem, vol + (change * 0.00065105));
            }
            oldvalue = vol_selector->value;
        }
//        print_vol_bar(elem,lcd);
//        set_mute_indicator(mute_indicator);
        delay(25);
    }
    if (sid != NULL)
        snd_mixer_selem_id_free(sid);
    if (handle != NULL)
        snd_mixer_close(handle);
    endwin();
    return 0;
}
