#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

/*
 * Button
 */
#define prevButtonPin 23 // BCM 13
#define playButtonPin 22 // BCM 6
#define nextButtonPin 2  // BCM 27
#define shufButtonPin 26 // BCM 12
#define infoButtonPin 21 // BCM 5
#define quitButtonPin 1  // BCM 18

#define BTN_DELAY 30

int playButtonState;
int prevButtonState;
int nextButtonState;
int infoButtonState;
int quitButtonState;
int shufButtonState;

int lastPlayButtonState;
int lastPrevButtonState;
int lastNextButtonState;
int lastInfoButtonState;
int lastQuitButtonState;
int lastShufButtonState;

long lastPlayDebounceTime;
long lastPrevDebounceTime;
long lastNextDebounceTime;
long lastInfoDebounceTime;
long lastQuitDebounceTime;
long lastShufDebounceTime;

long debounceDelay = 50;

const int numButtons = 6;

// FIXME this is only used during the setup;
// might get rid of to save memory
const int buttonPins[] = {
    playButtonPin,
    prevButtonPin,
    nextButtonPin,
    infoButtonPin,
    quitButtonPin,
    shufButtonPin
};

int printErr(char *msg, char *f, int l)
{
    fprintf(stderr, "[%s - %d]: %s\n", f, l, msg);
    return 0;
}

// For signal catching
static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int i;
    int reading;

    lastPlayButtonState = lastPrevButtonState = lastNextButtonState = HIGH;
    lastInfoButtonState = lastQuitButtonState = lastShufButtonState = HIGH;

    lastPlayDebounceTime = lastPrevDebounceTime = lastNextDebounceTime = 0;
    lastInfoDebounceTime = lastQuitDebounceTime = lastShufDebounceTime = 0;

    // NOTE: We're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s - %d]: %s\n", __FILE__, __LINE__, strerror(errno));
        return EXIT_FAILURE;
    }
    // Setup buttons
    for (i = 0; i < numButtons; i++)
    {
        pinMode(buttonPins[i], INPUT);
        pullUpDnControl(buttonPins[i], PUD_UP);
    }
    while (1)
    {
        /*
         * NOTE:
         * I got the following debouncing code from
         * http://www.arduino.cc/en/Tutorial/Debounce 
         *
         * Excerpt from source:
         *
         *  created 21 Nov 2006 by David A. Mellis
         * modified 30 Aug 2011 by Limor Fried
         * modified 28 Dec 2012 by Mike Walters
         * This example code is in the public domain.
         */
        /*
         * Play / Pause button
         */
        reading = digitalRead(playButtonPin);
        // Check to see if you just pressed the button 
        // (i.e. the input went from HIGH to LOW),  and you've waited 
        // long enough since the last press to ignore any noise:  
        // If the switch changed, due to noise or pressing:
        if (reading != lastPlayButtonState)
            lastPlayDebounceTime = millis(); // reset the debouncing timer
        if ((millis() - lastPlayDebounceTime) > debounceDelay)
        {
            // Whatever the reading is at, it's been there for longer
            // than the debounce delay, so take it as the actual current state:
            // if the button state has changed:
            if (reading != playButtonState)
            {
                playButtonState = reading;
                if (playButtonState == LOW)
                {
                    // do something
                    printf("play/pause button\n");
                }
            }
        }
        // Save the reading. Next time through the loop, it'll be the lastButtonState:
        lastPlayButtonState = reading;
        /*
         * Previous button
         */
        reading = digitalRead(prevButtonPin);
        if (reading != lastPrevButtonState)
            lastPrevDebounceTime = millis();
        if ((millis() - lastPrevDebounceTime) > debounceDelay)
        {
            if (reading != prevButtonState)
            {
                prevButtonState = reading;
                if (prevButtonState == LOW)
                {
                    // do something
                    printf("previous button\n");
                }
            }
        }
        lastPrevButtonState = reading;
        /*
         * Next button
         */
        reading = digitalRead(nextButtonPin);
        if (reading != lastNextButtonState)
            lastNextDebounceTime = millis();
        if ((millis() - lastNextDebounceTime) > debounceDelay)
        {
            if (reading != nextButtonState)
            {
                nextButtonState = reading;
                if (nextButtonState == LOW)
                {
                    // do something
                    printf("next button\n");
                }
            }
        }
        lastNextButtonState = reading;
        /*
         * Info button
         */
        reading = digitalRead(infoButtonPin);
        if (reading != lastInfoButtonState)
            lastInfoDebounceTime = millis();
        if ((millis() - lastInfoDebounceTime) > debounceDelay)
        {
            if (reading != infoButtonState)
            {
                infoButtonState = reading;
                if (infoButtonState == LOW)
                {
                    // do something
                    printf("info button\n");
                }
            }
        }
        lastInfoButtonState = reading;
        /*
         * Quit button
         */
        reading = digitalRead(quitButtonPin);
        if (reading != lastQuitButtonState)
            lastQuitDebounceTime = millis();
        if ((millis() - lastQuitDebounceTime) > debounceDelay)
        {
            if (reading != quitButtonState)
            {
                quitButtonState = reading;
                if (quitButtonState == LOW)
                {
                    // do something
                    printf("quit button\n");
                    break;
                }
            }
        }
        lastQuitButtonState = reading;
        /*
         * Shuffle button
         */
        reading = digitalRead(shufButtonPin);
        if (reading != lastShufButtonState)
            lastShufDebounceTime = millis();
        if ((millis() - lastShufDebounceTime) > debounceDelay)
        {
            if (reading != shufButtonState)
            {
                shufButtonState = reading;
                if (shufButtonState == LOW)
                {
                    // do something
                    printf("shuffle button\n");
                }
            }
        }
        lastShufButtonState = reading;
    }
    return EXIT_SUCCESS;
}

