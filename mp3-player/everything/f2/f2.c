#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include <time.h>
//#include <alsa/asoundlib.h>
// for signal catching
#include <errno.h>
//#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

//#include "rotaryencoder.h"

//#define exp10(x) (exp((x) * log(10)))

#define CE 1

const int extraLedPins[] = { 29, 28 }; // for bar graph leds 9 and 10

//const int rot_enc_A = 23;
//const int rot_enc_B = 27;

unsigned char data[1] = { 0x0 };
unsigned char backup_data[1] = { 0x0 };

//unsigned int lastMillis = 0;

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

/*
void die(int sig)
{
    Off();
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
}
*/

double map(float x, float x0, float x1, float y0, float y1)
{
    float y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    double z = (double)y;
    return z;
}

int main(int argc, char **argv)
{
    //unsigned long currentMillis;
    int myVol;

    /*
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    */
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pinMode(extraLedPins[0], OUTPUT);
    pinMode(extraLedPins[1], OUTPUT);
    wiringPiSPISetup(CE, 500000);
    //piHiPri(99);
    //struct encoder *vol_selector = setupEncoder(rot_enc_A, rot_enc_B);
    //if (vol_selector == NULL) exit(1);
    //int oldvalue = vol_selector->value;
        //double vol = get_normalized_volume(elem);
    double vol = atof(argv[1]);
    myVol = map(vol, 0, 1, 0, 10);
    doGraph(myVol);
    /*
    while (1)
    {
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
        delay(25);
    }
    */
    return 0;
}

