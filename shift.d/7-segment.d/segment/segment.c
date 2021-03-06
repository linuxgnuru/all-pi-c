#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

static const int dpPin = 2;

static const int leds[10][7] = {
    //     A   B   C   D   E   F   G
    {  4,  1,  3,  5,  6,  0, -1 }, // 0
    { -1,  1,  3, -1, -1, -1, -1 }, // 1
    {  4,  1, -1,  5,  6, -1,  7 }, // 2
    {  4,  1,  3,  5, -1, -1,  7 }, // 3
    { -1,  1,  3, -1, -1,  0,  7 }, // 4
    {  4, -1,  3,  5, -1,  0,  7 }, // 5
    {  4, -1,  3,  5,  6,  0,  7 }, // 6
    {  4,  1,  3, -1, -1, -1, -1 }, // 7
    {  4,  1,  3,  5,  6,  0,  7 }, // 8
    {  4,  1,  3,  5, -1,  0,  7 }  // 9
};

#define AR_

const int dataPin  = 3; // blue (pin 14)
const int latchPin = 2; // green (pin 12)
const int clockPin = 5; // yellow (pin 11)
const int addr = 100;
int bits = 32;

static void die(int sig)
{
    int i;
    for (i = 0; i < bits; i++)
        digitalWrite(addr + i, 0);
    if (sig != 0 && sig != 2)
        (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2)
        (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

void usage(char *name)
{
    printf("usage: %s [positive number between 0.000 and 9999]\n", name);
}

void printDigit(int digit, int pos, int dp)
{
    int i;
    int pos_ = addr + pos * 8;

    for (i = 0; i < 8; i++)
    {
        digitalWrite(pos_ + i, 0);
    }
    digitalWrite(pos_ + dpPin, dp);
    // if anything is invalid; just return
    if ((digit < 0 || digit > 9) || (pos < 0 || pos > 3) || (dp < 0 || dp > 1))
    {
        printf("ERROR: %d %d %d\n", digit, pos, dp);
        return;
    }
    for (i = 0; i < 7; i++)
    {
        if (leds[digit][i] != -1)
        {
            digitalWrite(pos_ + leds[digit][i], 1);
        }
    }
}

int main(int argc, char **argv)
{
    int dp_pos = -1;
    int badFlag = 0;
    int st_len = 0;
    int thou, hund, tens, ones;
#ifndef AR_
    int digit = 100;
#else
    double dig = 0.0;
    float tmp;
#endif
    //int dp = 0;
    //int pos;
    int i;
    int j;

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (argc > 1)
    {
        st_len = strlen(argv[1]);
        badFlag = (st_len > 5);
#ifndef AR_
        digit = atoi(argv[1]);
#else
        //dig = atof(argv[1]);
        sscanf(argv[1], "%lf", &dig);
        if (dig > 9999)
            badFlag = 1;
#endif
    }
    else
        badFlag = 1;
    if (badFlag)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (wiringPiSetup () == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return 1;
    }
    sr595Setup(addr, bits, dataPin, clockPin, latchPin);
#ifdef TIM_
    for (i = 0; i < 10000; i++)
    {
        thou = i / 1000;
        hund = (i / 100) % 10;
        tens = (i / 10) % 10;
        ones = i % 10;
        printDigit(thou, 3, 0);
        printDigit(hund, 2, 0);
        printDigit(tens, 1, 0);
        printDigit(ones, 0, 0);
        delay(5);
    }
#endif
#ifdef AR_
    i = j = 0;
    if (dig == (int)dig || dig > 1000) // no decimal point
    {
        thou = dig / 1000;
        hund = (int)(dig / 100) % 10;
        tens = (int)(dig / 10) % 10;
        ones = (int)dig % 10;
        dp_pos = 0;
        //printf("no dp\n");
    }
    else // decimal point
    {
        if (dig < 100 && dig > 9)
        {
            //printf("dig < 100 && dig > 9\n");
            dp_pos = 2;
            thou = (int)dig / 10;
            hund = (int)dig % 10;
            tmp = (dig - hund) * 10;
            tens = (int)tmp % 10;
            tmp = (dig * 10 - (thou * 100) - (hund * 10) - tens) * 10;
            ones = (int)tmp;
        }
        else if (dig < 10)
        {
            //printf("dig < 10\n");
            dp_pos = 3;
            thou = (int)dig % 10;
            tmp = (dig - thou) * 1000;
            hund = tmp / 100;
            tmp = (dig - thou) * 1000 - (hund * 100);
            tens = (int)tmp / 10;
            tmp = (dig - thou) * 1000 - (hund * 100) - (tens * 10);
            //printf("tmp: %f\n", tmp);
            ones = (int)tmp % 10;
        }
        else // dig > 99 && dig < 999
        {
            //printf("else (dig > 99 && dig < 999)\n");
            dp_pos = 1;
            thou = dig / 100;
            hund = (int)(dig / 10) % 10;
            tens = (int)dig % 10;
            tmp = dig * 10;
            ones = (int)tmp % 10;
        }
    }
    //printf("%f: %d %d %d %d\n", dig, thou, hund, tens, ones);
    printDigit(thou, 3, (dp_pos == 3));
    printDigit(hund, 2, (dp_pos == 2));
    printDigit(tens, 1, (dp_pos == 1));
    printDigit(ones, 0, (dp_pos == 0));
#endif
#ifdef AR_R
    i = j = 0;
    thou = digit / 1000;
    hund = (digit / 100) % 10;
    tens = (digit / 10) % 10;
    ones = digit % 10;
    printDigit(thou, 3, 0);
    printDigit(hund, 2, 0);
    printDigit(tens, 1, 0);
    printDigit(ones, 0, 0);
#endif
#ifdef DEMO
    for (i = 0; i < 4; i++)
    {
        //printf("pos: %d\n", i);
        for (j = 0; j < 10; j++)
        {
            //printDigit(-1, i, 0);
            printDigit(j, i, 0);
            //printf("digit: %d\n", j);
            delay(250);
        }
    }
#endif
    return EXIT_SUCCESS;
}

