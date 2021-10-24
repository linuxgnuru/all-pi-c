#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#define USE_N

#ifdef USE_N
#include <ncurses.h>
#endif

#include <wiringPi.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

//typedef enum { LEFT, RIGHT } lr_enum;

const int ledPins[10] = { 1, 4, 5, 6, 21, 22, 23, 24, 25, 27 };

#define LED_NUM 10

unsigned long lm = 0;

#define MX_DL 100

void onOff(_Bool b)
{
    int thisLed;

    for (thisLed = 0; thisLed < LED_NUM; thisLed++)
    {
        digitalWrite(ledPins[thisLed], b);
        //if (thisLed < 8) digitalWrite(ADDR + thisLed, b);
        //else digitalWrite(ledPins[thisLed - 8], b);
    }
}

void doGraph(int num)
{
    int thisLed;
    _Bool toggle;

    if (num < 0 || num > LED_NUM) return;
    if (num == 0) onOff(0);
    else if (num == LED_NUM) onOff(1);
    else
    {
        for (thisLed = 0; thisLed < LED_NUM; thisLed++)
        {
            toggle = (thisLed < num);
            digitalWrite(ledPins[thisLed], toggle);
            //if (thisLed < 8) digitalWrite(ADDR + thisLed, toggle);
            //else digitalWrite(ledPins[thisLed - 8], toggle);
        }
    }
}

static void die(int sig)
{
    onOff(0);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
#ifdef USE_N
    endwin();
#endif
    exit(0);
}

int main(int argc, char **argv)
{
    unsigned long cm;
    _Bool quitFlag = FALSE;
    int number = 0;
    int i;
    _Bool rev = 1;
#ifdef USE_N
    int key;
#endif

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
        exit(1);
    }
    for (i = 0; i < LED_NUM; i++)
        pinMode(ledPins[i], OUTPUT);
#ifdef USE_N
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("hello world");
    refresh();
#endif
    onOff(0);
    number = 0;
    while (quitFlag == FALSE)
    {
        cm = millis();
        if (cm - lm >= MX_DL)
        {
            lm = cm;
            doGraph(number);
#ifdef USE_N
            move(1, 0);
            printw("number: %2d", number);
            refresh();
            key = getch();
            if (key > -1) quitFlag = TRUE;
#endif
            if (rev)
                number++;
            else
                number--;
            if (number > LED_NUM)
            {
                rev = 0;
                number = LED_NUM;
            }
            if (number < 0)
            {
                rev = 1;
                number = 0;
            }
        }
    }
    onOff(0);
#ifdef USE_N
    endwin();
#endif
    return EXIT_SUCCESS;
}

