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

const int ledPins[2][8] = {
  { 7, 6, 5, 4, 3, 2, 1, 0 },
  { 21, 22, 23, 24, 25, 26, 27, 28 }
};

void Off() { int i, j; for (i = 0; i < 2; i++) for (j = 0; j < 8; j++) digitalWrite(ledPins[i][j], LOW); }
void Off_r() { int i; for (i = 0; i < 8; i++) digitalWrite(ledPins[1][i], LOW); } 
void Off_l() { int i; for (i = 0; i < 8; i++) digitalWrite(ledPins[0][i], LOW); }

void doGraph(int num_l, int num_r)
{
  int i;
  _Bool toggle_l, toggle_r;
  _Bool con_l, con_r;

  con_l = con_r = 1;
  if (num_r < 0 || num_r > 8) return;
  else if (num_l < 0 || num_l > 8) return;
  else
  {
    for (i = 0; i < 8; i++)
    {
      if (num_r == 0)
      {
        Off_r();
        con_r = 0;
      }
      if (num_l == 0)
      {
        Off_l();
        con_l = 0;
      }
      if (con_l)
      {
        toggle_l = (i < num_l);
        digitalWrite(ledPins[0][i], toggle_l);
      }
      if (con_r)
      {
        toggle_r = (i < num_r);
        digitalWrite(ledPins[1][i], toggle_r);
      }
      con_l = con_r = 1;
    }
  }
}

static void die(int sig)
{
  Off();
  if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
  if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
  exit(0);
}

int main(int argc, char **argv)
{
  _Bool quitFlag = FALSE;
  int number = 0;
  int i;

  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
  (void)signal(SIGTERM, die);
  (void)signal(SIGABRT, die);
  if (wiringPiSetup() == -1)
  {
    fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
    exit(1);
  }
  for (i = 0; i < 2; i++)
    for (number = 0; number < 8; number++)
      pinMode(ledPins[i][number], OUTPUT);
  number = 0;
  Off();
  while (quitFlag == FALSE)
  {
    for (i = 0; i < 9; i++)
    {
      //j = abs(i - 8);
      doGraph(i, i);
      delay(100);
    }
  }
  Off();
  return EXIT_SUCCESS;
}

