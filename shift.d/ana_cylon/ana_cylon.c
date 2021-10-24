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

int delayTime = 50;
int addr = 100;

int showText = 1;

int bits = 24;

const int dataPin = 7; // blue
const int latchPin = 22; // green
const int clockPin = 21; // yellow

// catch signals and clean up GPIO
static void die(int sig)
{
  int i;
  for (i = 0; i < bits; i++)
      digitalWrite(addr + i, 0);
  if (sig != 0 && sig != 2)
           (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
  if (sig == 2)
           (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    if (showText)
      endwin();
  exit(0);
}

double map(double x, double x0, double x1, double y0, double y1)
{
    double y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    return y;
}

int main(int argc, char **argv)
{
  int i;
  int snakeLen = 4;
  int bitStop;

    if (argc > 1)
    {
        if (strcmp(argv[1], "-d") == 0)
            showText = 0;
    }
  /*
  chipNum = 8;
  switch (chipNum)
  {
    case 1: case 2: case 3: case 4: bits = chipNum * 8; Bbits = 0; break
    case 5: case 6: case 7: case 8: bits = Bbits = (chipNum / 2) * 8; break;
  }
  */
#if 0
    // delayTime was previously set by arv[1]
  if (delayTime != 50 && (delayTime < 5 && delayTime > 300))
  {
    printf("usage: %s [optional delay time (10-300)]\n", argv[0]);
    return EXIT_FAILURE;
  }
  if (delayTime < 60)
    snakeLen = 6;
#endif
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
  sr595Setup(addr, bits, dataPin, clockPin, latchPin);
  // setup curses (for debuging)
  // turn everything off
  if (showText)
  {
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    //printw("Cur Led (A/B #)");
    printw("pulse %d", delayTime);
    //                  0123456789
    move(1, 0);
    //printw("1111111111222222222233");
    for (i = 0; i < 32; i++)
    {
      printw("%d", i / 10);
    }
    move(2, 0);
    //printw("01234567890123456789012345678901");
    for (i = 0; i < 32; i++)
      printw("%d", i % 10);
    refresh();
  }
  for (i = 0; i < bits; i++)
    digitalWrite(addr + i, 0);
  //printf("Raspberry Pi Shift Register\n");
  if (snakeLen == 4)
    bitStop = bits + 2;
  else
    bitStop = bits + 5;
  for (;;)
  {
    // start left to right first 32 bits
    for (i = 0; i < bitStop; i++)
    {
      /*
      move(1, 0); printw("A %2d", i);
      refresh();
      */
      if (i == 1)
      {
               digitalWrite(addr + i - 1, HIGH);
      }
      if (i == 2)
      {
               digitalWrite(addr + i - 1, HIGH);
               digitalWrite(addr + i - 2, HIGH);
      }
      if (snakeLen == 4)
      {
        if (i >= 3 && i < 30)
        {
                 digitalWrite(addr + i - 1, HIGH);
                 digitalWrite(addr + i - 2, HIGH);
                 digitalWrite(addr + i - 3, HIGH);
        }
        else
        {
          if (i == 30)
          {
                   digitalWrite(addr + 29, HIGH);
                   digitalWrite(addr + 28, HIGH);
                   digitalWrite(addr + 27, HIGH);
          }
          if (i == 31)
          {
                   digitalWrite(addr + 29, HIGH);
                   digitalWrite(addr + 28, HIGH);
          }
        }
      }
      else if (snakeLen == 6)
      {
        if (i == 3)
        {
                 digitalWrite(addr + i - 1, HIGH);
                 digitalWrite(addr + i - 2, HIGH);
                 digitalWrite(addr + i - 3, HIGH);
        }
        if (i == 4)
        {
                 digitalWrite(addr + i - 1, HIGH);
                 digitalWrite(addr + i - 2, HIGH);
                 digitalWrite(addr + i - 3, HIGH);
                 digitalWrite(addr + i - 4, HIGH);
        }
        if (i == 5)
        {
                 digitalWrite(addr + i - 1, HIGH);
                 digitalWrite(addr + i - 2, HIGH);
                 digitalWrite(addr + i - 3, HIGH);
                 digitalWrite(addr + i - 4, HIGH);
                 digitalWrite(addr + i - 5, HIGH);
        }
        if (i >= 6 && i < 30)
        {
                 digitalWrite(addr + i - 1, HIGH);
                 digitalWrite(addr + i - 2, HIGH);
                 digitalWrite(addr + i - 3, HIGH);
                 digitalWrite(addr + i - 4, HIGH);
                 digitalWrite(addr + i - 5, HIGH);
                 digitalWrite(addr + i - 6, HIGH);
        }
        else
        {
          switch (i)
          {
            case 30:
                   digitalWrite(addr + 29, HIGH);
                   digitalWrite(addr + 28, HIGH);
                   digitalWrite(addr + 27, HIGH);
                   digitalWrite(addr + 26, HIGH);
                   digitalWrite(addr + 25, HIGH);
                   digitalWrite(addr + 24, HIGH);
            break;
            case 31:
                   digitalWrite(addr + 29, HIGH);
                   digitalWrite(addr + 28, HIGH);
                   digitalWrite(addr + 27, HIGH);
                   digitalWrite(addr + 26, HIGH);
                   digitalWrite(addr + 25, HIGH);
            break;
            case 32:
                   digitalWrite(addr + 29, HIGH);
                   digitalWrite(addr + 28, HIGH);
                   digitalWrite(addr + 27, HIGH);
                   digitalWrite(addr + 26, HIGH);
            break;
            case 33:
                   digitalWrite(addr + 29, HIGH);
                   digitalWrite(addr + 28, HIGH);
                   digitalWrite(addr + 27, HIGH);
            break;
            case 34:
                   digitalWrite(addr + 29, HIGH);
                   digitalWrite(addr + 28, HIGH);
            break;
          }
        }
      }
      digitalWrite(addr + i, HIGH);
      if (showText)
      {
        move(3, i); printw("*");
        move(4, 0); printw("%02d", i);
        refresh();
      }
      // FIXME XXX TODO
      // FIXME XXX TODO
      // Need to use millis instead of the delay
      delay(delayTime);
      // Need to use millis instead of the delay
      // FIXME XXX TODO
      // FIXME XXX TODO
      // now turn off the LED
      if (i == 1)
      {
        digitalWrite(addr + i - 1, LOW);
      }
      if (i == 2)
      {
        digitalWrite(addr + i - 1, LOW);
        digitalWrite(addr + i - 2, LOW);
      }
      if (snakeLen == 4)
      {
        if (i >= 3 && i < 30)
        {
          digitalWrite(addr + i - 1, LOW);
          digitalWrite(addr + i - 2, LOW);
          digitalWrite(addr + i - 3, LOW);
        }
        else
        {
          if (i == 30)
          {
            //digitalWrite(addr + 29, LOW);
            digitalWrite(addr + 28, LOW);
            digitalWrite(addr + 27, LOW);
          }
          if (i == 31)
          {
            //digitalWrite(addr + 29, LOW);
            digitalWrite(addr + 28, LOW);
          }
        }
      }
      else if (snakeLen == 6)
      {
        if (i == 3)
        {
          digitalWrite(addr + i - 1, LOW);
          digitalWrite(addr + i - 2, LOW);
          digitalWrite(addr + i - 3, LOW);
        }
        if (i == 4)
        {
          digitalWrite(addr + i - 1, LOW);
          digitalWrite(addr + i - 2, LOW);
          digitalWrite(addr + i - 3, LOW);
          digitalWrite(addr + i - 4, LOW);
        }
        if (i == 5)
        {
          digitalWrite(addr + i - 1, LOW);
          digitalWrite(addr + i - 2, LOW);
          digitalWrite(addr + i - 3, LOW);
          digitalWrite(addr + i - 4, LOW);
          digitalWrite(addr + i - 5, LOW);
        }
        if (i >= 6 && i < 30)
        {
          digitalWrite(addr + i - 1, LOW);
          digitalWrite(addr + i - 2, LOW);
          digitalWrite(addr + i - 3, LOW);
          digitalWrite(addr + i - 4, LOW);
          digitalWrite(addr + i - 5, LOW);
          digitalWrite(addr + i - 6, LOW);
        }
        else
        {
          switch (i)
          {
            case 30:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              digitalWrite(addr + 26, LOW);
              digitalWrite(addr + 25, LOW);
              digitalWrite(addr + 24, LOW);
              break;
            case 31:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              digitalWrite(addr + 26, LOW);
              digitalWrite(addr + 25, LOW);
              break;
            case 32:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              digitalWrite(addr + 26, LOW);
              break;
            case 33:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              break;
            case 34:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              break;
          }
        }
      }
      digitalWrite(addr + i, LOW);
      if (showText)
      {
         move(3, i); printw(" ");
         refresh();
      }
      //delay(delayTime);
    }
    delayTime = map(analogRead(MY_8591), 0, 255, 20, 300);
    //digitalWrite(addr + bits - 1, 0);
    // now go the other way
    delay(delayTime);
    for (i = bits - 1; i > -1; i--)
    {
      /*
      move(1, 0); printw("A %2d", i);
      refresh();
      */
      if (snakeLen == 4)
      {
        if (i >= 3 && i < 28)
        {
           digitalWrite(addr + i + 1, HIGH);
           digitalWrite(addr + i + 2, HIGH);
           digitalWrite(addr + i + 3, HIGH);
        }
        if (i == 28)
        {
           digitalWrite(addr + 28, HIGH);
           digitalWrite(addr + 29, HIGH);
        }
      }
      else if (snakeLen == 6)
      {
        if (i == 3)
        {
           digitalWrite(addr + i + 1, HIGH);
           digitalWrite(addr + i + 2, HIGH);
           digitalWrite(addr + i + 3, HIGH);
        }
        if (i == 4)
        {
            digitalWrite(addr + i + 1, HIGH);
            digitalWrite(addr + i + 2, HIGH);
            digitalWrite(addr + i + 3, HIGH);
            digitalWrite(addr + i + 4, HIGH);
        }
        if (i == 5)
        {
            digitalWrite(addr + i + 1, HIGH);
            digitalWrite(addr + i + 2, HIGH);
            digitalWrite(addr + i + 3, HIGH);
            digitalWrite(addr + i + 4, HIGH);
            digitalWrite(addr + i + 5, HIGH);
        }
        if (i >= 6 && i < 24)
        {
            digitalWrite(addr + i + 1, HIGH);
            digitalWrite(addr + i + 2, HIGH);
            digitalWrite(addr + i + 3, HIGH);
            digitalWrite(addr + i + 4, HIGH);
            digitalWrite(addr + i + 5, HIGH);
            digitalWrite(addr + i + 6, HIGH);
        }
        else
        {
          switch (i)
          {
            case 24:
               digitalWrite(addr + 29, HIGH);
               digitalWrite(addr + 28, HIGH);
               digitalWrite(addr + 27, HIGH);
               digitalWrite(addr + 26, HIGH);
               digitalWrite(addr + 25, HIGH);
               digitalWrite(addr + 24, HIGH);
              break;
            case 25:
              digitalWrite(addr + 29, HIGH);
              digitalWrite(addr + 28, HIGH);
              digitalWrite(addr + 27, HIGH);
              digitalWrite(addr + 26, HIGH);
              digitalWrite(addr + 25, HIGH);
              break;
            case 26:
              digitalWrite(addr + 29, HIGH);
              digitalWrite(addr + 28, HIGH);
              digitalWrite(addr + 27, HIGH);
              digitalWrite(addr + 26, HIGH);
              break;
            case 27:
              digitalWrite(addr + 29, HIGH);
              digitalWrite(addr + 28, HIGH);
              digitalWrite(addr + 27, HIGH);
              break;
            case 28:
              digitalWrite(addr + 29, HIGH);
              digitalWrite(addr + 28, HIGH);
              break;
          }
        }
      }
      if (i == 2)
      {
         digitalWrite(addr + i + 1, HIGH);
         digitalWrite(addr + i + 2, HIGH);
      }
      if (i == 1)
      {
        digitalWrite(addr + i + 1, HIGH);
      }
      digitalWrite(addr + i, HIGH);
      if (showText)
      {
          move(3, i); printw("*");
          move(4, 0); printw("%02d", i);
          refresh();
      }
      delayTime = map(analogRead(MY_8591), 0, 255, 20, 300);
      delay(delayTime);
      if (snakeLen == 4)
      {
        if (i >= 3 && i < 28)
        {
             digitalWrite(addr + i + 1, LOW);
             digitalWrite(addr + i + 2, LOW);
             digitalWrite(addr + i + 3, LOW);
        }
        if (i == 28)
        {
             digitalWrite(addr + 28, LOW);
             digitalWrite(addr + 29, LOW);
        }
      }
      else if (snakeLen == 6)
      {
        if (i == 3)
        {
             digitalWrite(addr + i + 1, LOW);
             digitalWrite(addr + i + 2, LOW);
             digitalWrite(addr + i + 3, LOW);
        }
        if (i == 4)
        {
             digitalWrite(addr + i + 1, LOW);
             digitalWrite(addr + i + 2, LOW);
             digitalWrite(addr + i + 3, LOW);
             digitalWrite(addr + i + 4, LOW);
        }
        if (i == 5)
        {
             digitalWrite(addr + i + 1, LOW);
             digitalWrite(addr + i + 2, LOW);
             digitalWrite(addr + i + 3, LOW);
             digitalWrite(addr + i + 4, LOW);
             digitalWrite(addr + i + 5, LOW);
        }
        if (i >= 6 && i < 24)
        {
           digitalWrite(addr + i + 1, LOW);
           digitalWrite(addr + i + 2, LOW);
           digitalWrite(addr + i + 3, LOW);
           digitalWrite(addr + i + 4, LOW);
           digitalWrite(addr + i + 5, LOW);
           digitalWrite(addr + i + 6, LOW);
        }
        else
        {
          switch (i)
          {
            case 24:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              digitalWrite(addr + 26, LOW);
              digitalWrite(addr + 25, LOW);
              digitalWrite(addr + 24, LOW);
              break;
            case 25:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              digitalWrite(addr + 26, LOW);
              digitalWrite(addr + 25, LOW);
              break;
            case 26:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              digitalWrite(addr + 26, LOW);
              break;
            case 27:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              digitalWrite(addr + 27, LOW);
              break;
            case 28:
              digitalWrite(addr + 29, LOW);
              digitalWrite(addr + 28, LOW);
              break;
          }
        }
      }
      if (i == 2)
      {
         digitalWrite(addr + i + 1, LOW);
         digitalWrite(addr + i + 2, LOW);
      }
      if (i == 1)
      {
         digitalWrite(addr + i + 1, LOW);
      }
      digitalWrite(addr + i, LOW);
      if (showText)
      {
        move(3, i); printw(" ");
        refresh();
      }
      //delay(delayTime);
    }
    delayTime = map(analogRead(MY_8591), 0, 255, 20, 300);
    delay(delayTime);
  }
  return EXIT_SUCCESS;
}

