#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <string.h> // atoi

#include <wiringPi.h>
#include <sr595.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

const int dataPin  = 6; // blue
const int latchPin = 5; // green
const int clockPin = 4; // yellow

const int baseAddr = 100;
const int bits = 32;

static void die(int sig)
{
     if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
     if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
     exit(0);
}

int main(int argc, char **argv)
{
     int seg;
     int col;
     int newPin;
     _Bool bf = FALSE;

     // note: we're assuming BSD-style reliable signals here
     (void)signal(SIGINT, die);
     (void)signal(SIGHUP, die);
     if (argc > 2)
     {
          seg = atoi(argv[1]);
          col = atoi(argv[2]);
          bf = ((seg < 0 || seg > 7) || (col < 0 || col > 3));
     }
     else
          bf = TRUE;
     if (bf)
     {
          printf("usage %s [pin] [col] (pin 0-7, col 0-3)\n", argv[0]);
          return EXIT_FAILURE;
     }
     if (wiringPiSetup() == -1)
     {
          fprintf(stdout, "oops: %s\n", strerror(errno));
          return EXIT_FAILURE;
     }
     sr595Setup(baseAddr, bits, dataPin, clockPin, latchPin);
     //newPin = abs(seg - (bits - 1)) - (col * 8);
     newPin = ((seg - (bits - 1)) * -1) - (col * 8);
     digitalWrite(baseAddr + newPin, 1);
     return EXIT_SUCCESS;
}
