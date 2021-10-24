#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <ncurses.h>

#include <wiringPi.h>
#include <pcf8591.h>
#include <sr595.h>

#define MY_8591 200

#define BIT_NUM 24

const int dataPin = 7; // blue
const int latchPin = 21; // green
const int clockPin = 22; // yellow

const int addr = 100;

int bits[BIT_NUM];

unsigned int prevMillis = 0;

double map(double x, double x0, double x1, double y0, double y1)
{
    double y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    return y;
}

// catch signals and clean up GPIO
static void die(int sig)
{
    int i;
    for (i = 0; i < BIT_NUM; i++)
        digitalWrite(addr + i, LOW);
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
}

int main(int argc, char **argv)
{
    int i;
    int map_val;
    int raw_val;
    //unsigned int curMillis;

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
    pcf8591Setup(MY_8591, 0x48);
    sr595Setup(addr, BIT_NUM, dataPin, clockPin, latchPin);

    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);

    for (i = 0; i < BIT_NUM; i++)
        bits[i] = 0;
    // for (;;)
    i = 0;
    while (1)
    {
        raw_val = analogRead(MY_8591);
        map_val = map(raw_val, 0, 255, 0, 24);
        move(1, 0);
        printw("raw analog: %.3d", raw_val);
        move(2, 0);
        printw("map analog: %.3d", map_val);
        refresh();
        for (i = 0; i < 24; i++)
        {
            digitalWrite(addr + i, (i < map_val));
        }
    }
    return EXIT_SUCCESS;
}
