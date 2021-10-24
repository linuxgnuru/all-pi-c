#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
//#include <pcf8591.h>
#include <sr595.h>

//#define MY_8591 200

#define NUM_CHIPS 3
#define BIT_NUM 24
#define MAX_DELAY 250

const int dataPin = 7; // blue
const int latchPin = 21; // green
const int clockPin = 22; // yellow

const int addr = 100;

unsigned int myDelay = 500;

int bits[BIT_NUM];

// catch signals and clean up GPIO
static void die(int sig)
{
    int i;
    for (i = 0; i < NUM_CHIPS * 8; i++)
        digitalWrite(addr + i, 0);
    if (sig != 0 && sig != 2)
        (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2)
        (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

double map(double x, double x0, double x1, double y0, double y1)
{
    double y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    return y;
}

void toggle(int my_addr, int bit)
{
    if (bit < 0 || bit > 1)
        return;
    digitalWrite(my_addr, bit);
    bits[my_addr - addr] = bit;
}

int main(int argc, char **argv)
{
    int i;

    for (i = 0; i < BIT_NUM; i++)
        bits[i] = 0;
    /*
       if (argc > 1)
       {
       if (strcmp(argv[1], "-d") == 0)
       showText = 0;
       }
     */
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    //pcf8591Setup(MY_8591, 0x48);
    //sr595Setup(addr, bits, dataPin, clockPin, latchPin);
    sr595Setup(100, BIT_NUM, dataPin, clockPin, latchPin);
    toggle(101, HIGH);
    printf("%d\n", bits[101 - addr]);
    //digitalWrite(101, HIGH);

    for (;;)
    {
        for (i = 0; i < BIT_NUM; i++)
        {
            digitalWrite(100 + i, 1);
            delay(50);
        }
        for (i = BIT_NUM - 1; i > -1; i--)
        {
            digitalWrite(100 + i, 0);
            delay(50);
        }
        for (i = BIT_NUM - 1; i > -1; i--)
        {
            digitalWrite(100 + i, 1);
            delay(50);
        }
        for (i = 0; i < BIT_NUM; i++)
        {
            digitalWrite(100 + i, 0);
            delay(50);
        }
    }

    //  for (;;) {
    //    myDelay = map(analogRead(MY_8591), 0, 255, 0, MAX_DELAY);
    //myDelay = 250;
    //for (i = 0; i < bits; i++)
#if 0
    for (i = bits - 1; i > -1; i--)
    {
        printf("[U] i: %d\taddr: %d\n", i, addr + i);
        digitalWrite(addr + i, HIGH);
        delay(myDelay);
    }
#endif
#if 0
    for (i = bits - 1; i > -1; i--)
    {
        printf("[D] i: %d\taddr: %d\n", i, addr + i);
        digitalWrite(addr + i, LOW);
        delay(myDelay);
    }
#endif
#if 0
    for (i = 0; i < bits; i++)
    {
        printf("[D] i: %d\taddr: %d\n", i, addr + i);
        digitalWrite(addr + i, LOW);
        delay(myDelay);
    }
#endif
    //  }
    return EXIT_SUCCESS;
}

