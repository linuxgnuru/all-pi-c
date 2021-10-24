#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for uid_t
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

#define DISPLAY_BRIGHTNESS  25

const int dataPin  = 23; // blue (pin 14)
const int latchPin = 22; // green (pin 12)
const int clockPin = 21; // yellow (pin 11)
const int addr = 100;
const int bits = 8;

const int COLUMN_1=0;
const int COLUMN_2=1;
const int COLUMN_3=2;
//int SEGMENT_4=15;

const int SEGMENT_P = 0;
const int SEGMENT_A = 7;
const int SEGMENT_B = 6;
const int SEGMENT_C = 5;
const int SEGMENT_D = 4;
const int SEGMENT_E = 3;
const int SEGMENT_F = 2;
const int SEGMENT_G = 1;

/*
   int SEGMENT_A=3;
   int SEGMENT_B=5;
   int SEGMENT_C=18;
   int SEGMENT_D=19;
   int SEGMENT_E=23;
   int SEGMENT_F=24;
   int SEGMENT_G=25;
 */

void print_number(int num);
void display_number(int num);
static void die(int sig);

int main(int argc, char **argv)
{
    int counter = 0;

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    printf("7 Segment Multiplexing using Raspberry Pi\n") ;
    if (getuid() != 0) //wiringPi requires root privileges
    {
        printf("Error:wiringPi must be run as root.\n");
        return EXIT_FAILURE;
    }
    if (wiringPiSetup() == -1)
    {
        printf("Error:wiringPi setup failed!\n");
        return EXIT_FAILURE;
    }
    sr595Setup(addr, bits, dataPin, clockPin, latchPin);
    pinMode(COLUMN_1, OUTPUT);
    pinMode(COLUMN_2, OUTPUT);
    pinMode(COLUMN_3, OUTPUT);
    for (;;)
    {
        //printf("counter: %d\n", counter);
        display_number(counter++);
        delay(1000);
        if (counter > 999)
            counter = 0;
    }
    return EXIT_SUCCESS;
}

void print_number(int num)
{
    /*
       pinMode(SEGMENT_A,OUTPUT);
       pinMode(SEGMENT_B,OUTPUT);
       pinMode(SEGMENT_C,OUTPUT);
       pinMode(SEGMENT_D,OUTPUT);
       pinMode(SEGMENT_E,OUTPUT);
       pinMode(SEGMENT_F,OUTPUT);
       pinMode(SEGMENT_G,OUTPUT);
     */
    switch (num)
    {
        case 0:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, LOW);
            digitalWrite(addr + SEGMENT_E, LOW);
            digitalWrite(addr + SEGMENT_F, LOW);
            digitalWrite(addr + SEGMENT_G, HIGH);
            break;

        case 1:
            digitalWrite(addr + SEGMENT_A, HIGH);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, HIGH);
            digitalWrite(addr + SEGMENT_E, HIGH);
            digitalWrite(addr + SEGMENT_F, HIGH);
            digitalWrite(addr + SEGMENT_G, HIGH);
            break;

        case 2:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, HIGH);
            digitalWrite(addr + SEGMENT_D, LOW);
            digitalWrite(addr + SEGMENT_E, LOW);
            digitalWrite(addr + SEGMENT_F, HIGH);
            digitalWrite(addr + SEGMENT_G, LOW);
            break;

        case 3:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, LOW);
            digitalWrite(addr + SEGMENT_E, HIGH);
            digitalWrite(addr + SEGMENT_F, HIGH);
            digitalWrite(addr + SEGMENT_G, LOW);
            break;

        case 4:
            digitalWrite(addr + SEGMENT_A, HIGH);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, HIGH);
            digitalWrite(addr + SEGMENT_E, HIGH);
            digitalWrite(addr + SEGMENT_F, LOW);
            digitalWrite(addr + SEGMENT_G, LOW);
            break;

        case 5:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, HIGH);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, LOW);
            digitalWrite(addr + SEGMENT_E, HIGH);
            digitalWrite(addr + SEGMENT_F, LOW);
            digitalWrite(addr + SEGMENT_G, LOW);
            break;

        case 6:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, HIGH);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, LOW);
            digitalWrite(addr + SEGMENT_E, LOW);
            digitalWrite(addr + SEGMENT_F, LOW);
            digitalWrite(addr + SEGMENT_G, LOW);
            break;

        case 7:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, HIGH);
            digitalWrite(addr + SEGMENT_E, HIGH);
            digitalWrite(addr + SEGMENT_F, HIGH);
            digitalWrite(addr + SEGMENT_G, HIGH);
            break;

        case 8:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, LOW);
            digitalWrite(addr + SEGMENT_E, LOW);
            digitalWrite(addr + SEGMENT_F, LOW);
            digitalWrite(addr + SEGMENT_G, LOW);
            break;

        case 9:
            digitalWrite(addr + SEGMENT_A, LOW);
            digitalWrite(addr + SEGMENT_B, LOW);
            digitalWrite(addr + SEGMENT_C, LOW);
            digitalWrite(addr + SEGMENT_D, LOW);
            digitalWrite(addr + SEGMENT_E, HIGH);
            digitalWrite(addr + SEGMENT_F, LOW);
            digitalWrite(addr + SEGMENT_G, LOW);
            break;

        case 10:
            digitalWrite(addr + SEGMENT_A, HIGH);
            digitalWrite(addr + SEGMENT_B, HIGH);
            digitalWrite(addr + SEGMENT_C, HIGH);
            digitalWrite(addr + SEGMENT_D, HIGH);
            digitalWrite(addr + SEGMENT_E, HIGH);
            digitalWrite(addr + SEGMENT_F, HIGH);
            digitalWrite(addr + SEGMENT_G, HIGH);
            break;
    }
}

static void die(int sig)
{
    int i;
    for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

void display_number(int num)
{
    long start;
    int i;

    start = millis();
    for (i = 3; i > 0; i--)
    {
        switch (i)
        {
            case 1: digitalWrite(COLUMN_1, LOW); break;
            case 2: digitalWrite(COLUMN_2, LOW); break;
            case 3: digitalWrite(COLUMN_3, LOW); break;
        }
        print_number(num % 10);
        num /= 10;
        delayMicroseconds(DISPLAY_BRIGHTNESS);
        print_number(10);
        digitalWrite(COLUMN_1, HIGH);
        digitalWrite(COLUMN_2, HIGH);
        digitalWrite(COLUMN_3, HIGH);
    }
    while ((millis() - start) < 10)
        ;
}

