#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

//#define USE_N

#ifdef USE_N
#include <ncurses.h>
#endif

#include <wiringPi.h>
#include <sr595.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

#define ADDR 100
#define BITS 8

#define dataPin   25 // blue (pin 14)
#define latchPin  26 // green (pin 12)
#define clockPin  27 // yellow (pin 11)

const int ledPins[] = { 23, 24 }; // for bar graph leds 9 and 10

// Rotary Encoder
#define encoderPinB  21
#define encoderPinA  22
#define encoderButton  28

const long debounceDelay = 50;

//_Bool ledState[10];

void onOff(_Bool b)
{
    int thisLed;

    for (thisLed = 0; thisLed < 10; thisLed++)
    {
        //ledState[thisLed] = b;
        if (thisLed < 8) digitalWrite(ADDR + thisLed, b);
        else digitalWrite(ledPins[thisLed - 8], b);
    }
}

void doGraph(int num)
{
    int thisLed;
    _Bool toggle;

    if (num < 0 || num > 10)
        return;
    if (num == 0)
        onOff(0);
    else if (num == 10)
        onOff(1);
    else
    {
        for (thisLed = 0; thisLed < 10; thisLed++)
        {
            toggle = (thisLed < num);
            //ledState[thisLed] = toggle;
            if (thisLed < 8) digitalWrite(ADDR + thisLed, toggle);
            else digitalWrite(ledPins[thisLed - 8], toggle);
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
    _Bool reading;
    _Bool quitFlag = FALSE;
    _Bool flipflopflag;
    _Bool lastStatus;
    _Bool currentStatus;
    long lastDebounceTime = 0;
    int buttonState = HIGH;
    int lastButtonState = HIGH;
    //int i;
    int number = 0;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
        exit(1);
    }
    sr595Setup(ADDR, BITS, dataPin, clockPin, latchPin);
    pinMode(ledPins[0], OUTPUT);
    pinMode(ledPins[1], OUTPUT);
    pinMode(encoderButton, INPUT);
    pullUpDnControl(encoderButton, PUD_UP);
    pinMode(encoderPinB, INPUT);
    pinMode(encoderPinA, INPUT);
#ifdef USE_N
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("hello world");
    refresh();
#endif
    //for (i = 0; i < 10; i++) ledState[i] = 0;
    onOff(0);
    while (quitFlag == FALSE)
    {
        reading = digitalRead(encoderButton);
        if (reading != lastButtonState)
            lastDebounceTime = millis();
        if ((millis() - lastDebounceTime) > debounceDelay)
        {
            if (reading != buttonState)
            {
                buttonState = reading;
                if (buttonState == LOW)
                    quitFlag = TRUE;
            }
        }
        lastButtonState = reading;

        lastStatus = digitalRead(encoderPinB);
        while (!digitalRead(encoderPinA))
        {
            currentStatus = digitalRead(encoderPinB);
            flipflopflag = TRUE;
        }
        if (flipflopflag == TRUE)
        {
            flipflopflag = FALSE;
            if (!lastStatus && currentStatus) number++;
            if (lastStatus && !currentStatus) number--;
            //if ((lastStatus == 0) && (currentStatus == 1)) number++;
            //if ((lastStatus == 1) && (currentStatus == 0)) number--;
            if (number < 0) number = 0;
            if (number > 10) number = 10;
            doGraph(number);
#ifdef USE_N
            move(1, 0);
            printw("number: %2d", number);
            refresh();
#endif
        }
    }
    onOff(0);
#ifdef USE_N
    endwin();
#endif
    return EXIT_SUCCESS;
}

