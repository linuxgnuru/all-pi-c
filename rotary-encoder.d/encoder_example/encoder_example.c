/**********************************************************************
 * Filename    : rotaryEncoder.c
 * Description : make a rotaryEncoder work.
 * Author      : Robot
 * E-mail      : support@sunfounder.com
 * website     : www.sunfounder.com
 * Date        : 2014/08/27
 **********************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>

#include <wiringPi.h>

#define  RoAPin    27
#define  RoBPin    28
#define  RoSPin    7

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

static volatile int globalCounter = 0;

unsigned int flag;
unsigned char Last_RoB_Status;
unsigned char Current_RoB_Status;

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    endwin();
    exit(0);
}

void rotaryDeal(void)
{
    Last_RoB_Status = digitalRead(RoBPin);

    while (!digitalRead(RoAPin))
    {
        Current_RoB_Status = digitalRead(RoBPin);
        flag = TRUE;
    }
    if (flag == TRUE)
    {
        flag = FALSE;
        if ((Last_RoB_Status == 0) && (Current_RoB_Status == 1))
        {
            globalCounter++;
            move(1, 0);
            printw("globalCounter: %3d", globalCounter);
            refresh();
        }
        if ((Last_RoB_Status == 1) && (Current_RoB_Status == 0))
        {
            globalCounter--;
            move(1, 0);
            printw("globalCounter: %3d", globalCounter);
            refresh();
        }
    }
}

void rotaryClear(void)
{
    if (digitalRead(RoSPin) == LOW) // low because it is setup pullup
    {
        globalCounter = 0;
        move(1, 0);
        printw("globalCounter: %3d", globalCounter);
        refresh();
        delay(1000);
    }
}

int main(void)
{
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
    printw("hello world");
    refresh();
    if (wiringPiSetup() < 0)
    {
        fprintf(stderr, "Unable to setup wiringPi:%s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pinMode(RoAPin, INPUT);
    pinMode(RoBPin, INPUT);
    pinMode(RoSPin, INPUT);
    pullUpDnControl(RoSPin, PUD_UP);
    while (1)
    {
        rotaryDeal();
        rotaryClear();
    }
    endwin();
    return EXIT_SUCCESS;
}

