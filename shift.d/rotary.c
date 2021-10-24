#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

// Rotary Encoder
#define encoderPinB  21
#define encoderPinA  22
#define encoderButton  28

const long debounceDelay = 50; // for button

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
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
    pinMode(encoderButton, INPUT);
    pullUpDnControl(encoderButton, PUD_UP);
    pinMode(encoderPinB, INPUT);
    pinMode(encoderPinA, INPUT);
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
            if (number < 0) number = 0;
            if (number > 10) number = 10;
        }
    }
    return EXIT_SUCCESS;
}

