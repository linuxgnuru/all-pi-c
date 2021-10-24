#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <sr595.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

const int dataPin  = 6; // blue (pin 14)
const int latchPin = 5; // green (pin 12)
const int clockPin = 4; // yellow (pin 11)
const int addr = 100;
int bits = 32;

/*
static const int leds[10][8] = {
     // E   B   G   F  dp   A   D   C
     {  7,  0, -1,  2, -1,  1,  6,  5 }, // 0
     { -1,  0, -1, -1, -1, -1, -1,  5 }, // 1
     {  7,  0,  3, -1, -1,  1,  6, -1 }, // 2
     { -1,  0,  3, -1, -1,  1,  6,  5 }, // 3
     { -1,  0,  3,  2, -1, -1, -1,  5 }, // 4
     { -1, -1,  3,  2, -1,  1,  6,  5 }, // 5
     {  7, -1,  3,  2, -1,  1,  6,  5 }, // 6
     { -1,  0, -1, -1, -1,  1, -1,  5 }, // 7
     {  7,  0,  3,  2, -1,  1,  6,  5 }, // 8
     { -1,  0,  3,  2, -1,  1,  6,  5 }  // 9
};
*/

static const int leds[10][8] = {
     // E   B   G   F  dp   A   D   C
     {  1,  6, -1,  7, -1,  3,  2,  4 }, // 0
     { -1,  6, -1, -1, -1, -1, -1,  4 }, // 1
     {  1,  6,  0, -1, -1,  3,  2, -1 }, // 2
     { -1,  6,  0, -1, -1,  3,  2,  4 }, // 3
     { -1,  6,  0,  7, -1, -1, -1,  4 }, // 4
     { -1, -1,  0,  7, -1,  3,  2,  4 }, // 5
     {  1, -1,  0,  7, -1,  3,  2,  4 }, // 6
     { -1,  6, -1, -1, -1,  3, -1,  4 }, // 7
     {  1,  6,  0,  7, -1,  3,  2,  4 }, // 8
     { -1,  6,  0,  7, -1,  3,  2,  4 }  // 9
};

static void die(int sig)
{
     int i;
     for (i = 0; i < bits; i++) digitalWrite(addr + i, 0);
     if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
     if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
     exit(0);
}

void makeDigit(int col, int digit, _Bool dp)
{
     int i;
     int newPin;
     for (i = 0; i < 8; i++)
     {
          if (digit == -1)
               digitalWrite(addr + i, LOW);
          else
          {
               if (leds[digit][i] != -1)
               {
                    newPin = abs(leds[digit][i] - 31) - (col * 8);
                    digitalWrite(addr + newPin, HIGH);
               }
               /*
               // dp
               digitalWrite(addr + 4, HIGH);
               digitalWrite(addr + 4, LOW);
               */
          }
     }
     digitalWrite(addr + 4, dp);
}

int main(int argc, char **argv)
{
     //int h, i, j;
     int digit = 0;
     int col = 0;
     //char c;
     _Bool badFlag = FALSE;

     // note: we're assuming BSD-style reliable signals here
     (void)signal(SIGINT, die);
     (void)signal(SIGHUP, die);
     if (argc > 2)
     {
          col = atoi(argv[1]);
          digit = atoi(argv[2]);
     }
     else
          badFlag = TRUE;
     if (badFlag)
     {
          printf("usage: %s [col] [digit]\n", argv[0]);
          //printf("(to clear, use -1 for digit)\n\n");
          return EXIT_FAILURE;
     }
     if (wiringPiSetup () == -1)
     {
          fprintf(stdout, "oops: %s\n", strerror(errno));
          return EXIT_FAILURE;
     }
     sr595Setup(addr, bits, dataPin, clockPin, latchPin);
     makeDigit(col, digit, 0);
     /*
        printf("digit: %d\n", digit);
        for (i = 0; i < 7; i++) printf("%d ", leds[digit][i]);
        printf("\n");
      */
     return EXIT_SUCCESS;
}

