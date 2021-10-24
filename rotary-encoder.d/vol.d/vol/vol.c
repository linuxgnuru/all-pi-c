/*
    HEYJOHN: The rotary encoder I have REQUIRES the vin to be plugged in to work.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <ncurses.h>

#include <wiringPi.h>

#include "rotary-encoder.h"

#define FALSE 0
#define TRUE 1

#define MY_8574 100

const addr = 0x20;

int aPin = 22;
int bPin = 23;
int switchPin = 24;

// debouncing
int  buttonState; // current reading from the input pin
int  lastButtonState = LOW; // previous reading from the input pin
long lastDebounceTime = 0; // last time the output pin was toggled

const long debounceDelay = 50; // debounce time; increase if the output flickers

void sig_handler(int signo)
{
    int i;

    if (signo == SIGINT || signo == SIGTERM)
    {
        //printf("SIGINT recieved\n");
        for (i = 0; i < 8; i++)
            digitalWrite(MY_8574 + i, LOW);
        printf("\nGood bye.\n");
        endwin();
        exit(1);
    }
}

void usage(char *name) { printf("usage: %s [A pin] [B pin] [button pin]\n", name); }

int main(int argc, char **argv)
{
    int i;
    int ctr = 0;
    int stopFlag = FALSE;
    int reading;

    /*
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    */
    if (argc > 3)
    {
        aPin = atoi(argv[1]);
        bPin = atoi(argv[2]);
        switchPin = atoi(argv[3]);
    }
    /*
    else
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    */
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\n Can't catch SIGINT. \n");
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pcf8574Setup(MY_8574, addr);
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
    for (i = 0; i < 8; i++)
        digitalWrite(MY_8574 + i, LOW);
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
        printw("encoder->value: %d", encoder->value);
#if 0
        if (oldvalue > encoder->value + 2)
        {
            if (ctr + 1 <= 7)
                ctr++;
            digitalWrite(MY_8574 + ctr, HIGH);
        }
        if (oldvalue < encoder->value - 2)
        {
            if (ctr - 1 >= 0)
                ctr--;
            digitalWrite(MY_8574 + ctr, LOW);
        }
        delay(25);
#endif
//#if 0
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
#if 0
            for (i = 0; i < 8; i++)
            {
//                digitalWrite(MY_8574 + i, (change > 0 ? HIGH : LOW));
                switch (i)
                {
                    case 0:
                        if (i < new_val) digitalWrite(MY_8574 + i, HIGH);
                        else digitalWrite(MY_8574 + i, LOW);
                        break;
                    case 1:
                        if (i < new_val) digitalWrite(gpioPin1, HIGH);
                        else digitalWrite(gpioPin1, LOW);
                        break;
                    case 2:
                        if (i < new_val) digitalWrite(gpioPin2, HIGH);
                        else digitalWrite(gpioPin2, LOW);
                        break;
                    case 3:
                        if (i < new_val) digitalWrite(gpioPin3, HIGH);
                        else digitalWrite(gpioPin3, LOW);
                        break;
                    case 4:
                        if (i < new_val) digitalWrite(gpioPin4, HIGH);
                        else digitalWrite(gpioPin4, LOW);
                        break;
                    case 5:
                        if (i < new_val) digitalWrite(gpioPin5, HIGH);
                        else digitalWrite(gpioPin5, LOW);
                        break;
                    case 6:
                        if (i < new_val) digitalWrite(gpioPin6, HIGH);
                        else digitalWrite(gpioPin6, LOW);
                        break;
                    case 7:
                        if (i < new_val) digitalWrite(gpioPin7, HIGH);
                        else digitalWrite(gpioPin7, LOW);
                        break;
                }
            }
#endif
//#if 0
            if (change > 0) // going right
            {
                if (ctr + 1 <= 7)
                    ctr++;
//                digitalWrite(MY_8574 + ctr, HIGH);
            }
            else // left
            {
                if (ctr - 1 >= 0)
                    ctr--;
//                digitalWrite(MY_8574 + ctr, LOW);
            }
            move(5, 0);
            printw("ctr: %02d", ctr);
            for (i = 0; i < ctr + 1; i++)
                digitalWrite(MY_8574 + i, HIGH);
            for (i = ctr + 1; i < 8; i++)
                digitalWrite(MY_8574 + i, LOW);
//#endif
            oldvalue = encoder->value;
        }
//#endif
        refresh();
        delay(50);
    }
    endwin();
    for (i = 0; i < 8; i++)
        digitalWrite(MY_8574 + i, LOW);
    return 0;
}
