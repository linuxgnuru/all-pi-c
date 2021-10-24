/*
    HEYJOHN: The rotary encoder I have REQUIRES the vin to be plugged in to work.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <ncurses.h>

#include <wiringPi.h>

#include "rotary-encoder.h"

#define FALSE 0
#define TRUE 1

int aPin = 22;
int bPin = 21;
int switchPin = 28;

// debouncing
int  buttonState; // current reading from the input pin
int  lastButtonState = HIGH; // previous reading from the input pin
long lastDebounceTime = 0; // last time the output pin was toggled

const long debounceDelay = 50; // debounce time; increase if the output flickers

void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    endwin();
    exit(1);
}

void usage(char *name) { printf("usage: %s [A pin] [B pin] [button pin]\n", name); }

int main(int argc, char **argv)
{
    int ctr = 0;
    int stopFlag = FALSE;
    int reading;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (argc > 3)
    {
        aPin = atoi(argv[1]);
        bPin = atoi(argv[2]);
        switchPin = atoi(argv[3]);
    }
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pinMode(switchPin, INPUT);
	pullUpDnControl(switchPin, PUD_UP);
    struct encoder *encoder = setupEncoder(aPin, bPin);
    if(encoder == NULL)
        exit(1);
    int oldvalue = encoder->value;
    // standard ncurses init stuff
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("rotary encoder");
    //mvaddstr(1, 0, "Fun!");
    refresh();
    while (stopFlag == FALSE)
    {
        reading = digitalRead(switchPin);
        if (reading != lastButtonState)
          lastDebounceTime = millis();
        if ((millis() - lastDebounceTime) > debounceDelay)
        {
          if (reading != buttonState)
          {
              buttonState = reading;
              if (buttonState == LOW)
                  stopFlag = TRUE;
          }
        }
        lastButtonState = reading;
        move(1, 0);
        printw("encoder->value: %3d", encoder->value);
        if (oldvalue != encoder->value)
        {
            int change = encoder->value - oldvalue;
            /*
            move(2, 0);
            printw("encoder->value: %d", encoder->value);
            */
            move(3, 0);
            printw("oldvalue: %03d", oldvalue);
            move(4, 0);
            printw("change: %03d", change);
            if (change > 0) // going right
            {
                if (ctr + 1 <= 7)
                    ctr++;
            }
            else // left
            {
                if (ctr - 1 >= 0)
                    ctr--;
            }
            move(5, 0);
            printw("ctr: %02d", ctr);
            oldvalue = encoder->value;
        }
        refresh();
        delay(50);
    }
    endwin();
    return 0;
}
