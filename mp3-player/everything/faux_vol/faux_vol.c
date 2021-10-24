#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CE 1

// for bar graph leds 9 and 10
const int extraLedPins[] = { 29, 28 };

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
        exit(1);
    }
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    else (void)fprintf(stderr, "Exiting due to %s\n", strsignal(sig));
}

int main(int argc, char **argv)
{
    int i;

    (void)signal(SIGINT, die);
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
    Off();
    if (argc == 1 || argc > 2)
    {
        printf("usage %s [num (0-8)]\n", argv[0]);
        return EXIT_FAILURE;
    }
    i = atoi(argv[1]);
    doGraph(i);
    return EXIT_SUCCESS;
}

