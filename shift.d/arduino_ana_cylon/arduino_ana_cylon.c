#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <ncurses.h>

#include <wiringPi.h>
#include <pcf8591.h>
#include <sr595.h>

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))

//#define map(y, x, x0, x1, y0, y1) ((y) = (y0 + ((y1 - y0) * ((x -x0) / (x1 - x0)))))

#define MY_8591 200

#define NUM_CHIPS 3
#define BIT_NUM 24

#define MAX_DELAY 250
#define MIN_DELAY 5

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

const int dataPin = 7; // blue
const int latchPin = 21; // green
const int clockPin = 22; // yellow

const int addr = 100;

char data[NUM_CHIPS];

unsigned int myDelay = 500;
int showText = 1;

int bits[BIT_NUM];

unsigned int prevMillis = 0;

int sequence1Done = FALSE;
int sequence2Done = FALSE;

int ctr_a = 0;
int ctr_b = 0;

int goingRight = TRUE;

double map(double x, double x0, double x1, double y0, double y1)
{
    double y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    return y;
}

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
    endwin();
    exit(0);
}

void toggle(int input)
{
    digitalWrite(addr + input, !bits[input]);
    bits[input] = !bits[input];
    /*
       int chip, pin;
       int anaVal;

    // Input will be any number from 0 to (NUM_CHIPS * 8) - 1
    // pin == input
    anaVal = analogRead(MY_8591);
    myDelay = map(anaVal, 0, 255, 10, MAX_DELAY);
    //move(10, 0); printw("anaVal: %d", anaVal); refresh();

    if (input < 8)
    {
    chip = NUM_CHIPS - 1;
    pin = input;
    if (chip < 0 || pin < 0)
    {
    mvaddstr(9, 0, "- NEGATIVE -");
    mvaddstr(10, 0, "input < 8");
    move(11, 0);
    printw("chip: %d", chip);
    move(12, 0);
    printw("pin: %d", pin);
    move(13, 0);
    printw("input: %d", input);
    refresh();
    }
    }
    else if (input > 7 && input < 16)
    {
    chip = NUM_CHIPS - 2;
    pin = input - 8;
    if (chip < 0 || pin < 0)
    {
    mvaddstr(9, 10, "- NEGATIVE -");
    mvaddstr(10, 10, "input > 7 && input < 16");
    move(11, 10);
    printw("chip: %d", chip);
    move(12, 10);
    printw("pin: %d", pin);
    move(13, 10);
    printw("input: %d", input);
    refresh();
    }
    }
    else if (input > 15 && input < 24)
    {
    chip = NUM_CHIPS - 3;
    pin = input - 16;
    if (chip < 0 || pin < 0)
    {
    mvaddstr(9, 20, "- NEGATIVE -");
    mvaddstr(10, 20, "input > 15 && input < 24");
    move(11, 20);
    printw("chip: %d", chip);
    move(12, 20);
    printw("pin: %d", pin);
    move(13, 20);
    printw("input: %d", input);
    refresh();
    }
    }
    else if (input > 23 && input < 32)
    {
    chip = NUM_CHIPS - 4;
    pin = input - 24;
    if (chip < 0 || pin < 0)
    {
    mvaddstr(9, 30, "- NEGATIVE -");
    mvaddstr(10, 30, "input > 23 && input < 32");
    move(11, 30);
    printw("chip: %d", chip);
    move(12, 30);
    printw("pin: %d", pin);
    move(13, 30);
    printw("input: %d", input);
    refresh();
}
}
else // this else was because code was complaining
{
    chip = NUM_CHIPS;
    pin = input;
}
//t = bitRead(data[chip], pin);
if (!showText)
{
    move(0, 0); printw("     myDelay: %3d", myDelay);
    move(1, 0); printw("       input: %2d", input);
    move(2, 0); printw("addr + input: %3d", addr + input);
    move(3, 0); printw("chip: %d\tpin: %d", chip, pin);
    refresh();
}
// the following is the same as the bitWrite underneath
//data[chip] ^= (-(!((data[chip] >> pin) & 1)) ^ data[chip]) & (1 << pin);
//bitWrite(data[chip], pin, !bitRead(data[chip], pin));
//digitalWrite(addr + input, ((data[chip] >> pin) & 1));
//digitalWrite(addr + input, !bitRead(data[chip], pin));
digitalWrite(addr + input, !bits[input]);
bits[input] = !bits[input];
#if 0
digitalWrite(LATCH, LOW);
for (int i = 0; i < NUM_CHIPS; i++)
SPI.transfer(data[i]);
digitalWrite(LATCH, HIGH);
#endif
*/
}

int main(int argc, char **argv)
{
    int i;
    unsigned int curMillis;

    if (argc > 1)
    {
        if (strcmp(argv[1], "-d") == 0)
            showText = 0;
    }
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pcf8591Setup(MY_8591, 0x48);
    sr595Setup(addr, BIT_NUM, dataPin, clockPin, latchPin);

    for (i = 0; i < NUM_CHIPS; i++)
        data[i] = 0b00000000;
    for (i = 0; i < BIT_NUM; i++)
        bits[i] = 0;
    // setup curses (for debuging)
    // turn everything off
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    mvaddstr(1, 0, "Analog LED Chaser");
    refresh();

#if 0
    if (showText == -1)
    {
        //printw("Cur Led (A/B #)");
        printw("pulse %d", myDelay);
        //                  0123456789
        move(1, 0);
        //printw("1111111111222222222233");
        for (i = 0; i < bits; i++)
            printw("%d", i / 10);
        move(2, 0);
        //printw("01234567890123456789012345678901");
        for (i = 0; i < bits; i++)
            printw("%d", i % 10);
        refresh();
    }
#endif
    // for (;;)
    i = 0;
    while (1)
    {
        myDelay = map(analogRead(MY_8591), 0, 255, MIN_DELAY, MAX_DELAY);
        move(2, 0);
        printw("Current delay: %.3d", myDelay);
        refresh();
#if 0
        if (showText)
        {
            if (i >= bits)
                i = 0;
            move(3, i++);
            printw("*");
            refresh();
        }
#endif
        curMillis = millis();
        if (sequence1Done == FALSE)
        {
            // stop first "for" loop
            if (sequence2Done == TRUE)
            {
                sequence2Done = FALSE;
                // the following will always be true...
                if (++ctr_b > 1)
                    sequence1Done = TRUE;
            }
            else if ((curMillis - prevMillis >= myDelay) && sequence2Done == FALSE)
            {
                prevMillis += myDelay;
                if (goingRight)
                {
                    if (++ctr_a > (NUM_CHIPS * 8) - 1)
                    {
                        // stop second "for" loop
                        sequence2Done = TRUE;
                        ctr_a = 0;
                    }
                    if (sequence2Done == FALSE)
                        toggle(ctr_a);
                }
                else
                {
                    //if (--ctr_a < -2)
                    if (--ctr_a < -1)
                    {
                        // stop second "for" loop
                        sequence2Done = TRUE;
                        ctr_a = (NUM_CHIPS * 8);
                    }
                    if (sequence2Done == FALSE)
                        toggle(ctr_a);
                }
            }
        } // end if (sequence1done)
        else
        {
            if (goingRight == TRUE)
            {
                goingRight = FALSE;
                ctr_a = (NUM_CHIPS * 8);
            }
            else
            {
                goingRight = TRUE;
                ctr_a = 0;
            }
            sequence1Done = FALSE;
            ctr_b = 0;
        } // end else of if (sequence1done)
    } // end while (1)
    return EXIT_SUCCESS;
}

