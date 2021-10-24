#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

/*
useage:
    printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
    BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
    */

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

unsigned char data[1] = { 0x0 };
unsigned char backup_data[1] = { 0x0 };

void bitWrite(int n, int b) { if (n <= 7 && n >= 0) data[0] ^= (-b ^ data[0]) & (1 << n); }

void bitClear(int n) { if (n <= 7 && n >= 0) data[0] ^= (0 ^ data[0]) & (1 << n); }

void bitSet(int n) { if (n <= 7 && n >= 0) data[0] ^= (-1 ^ data[0]) & (1 << n); }

void Off()
{
    data[0] = 0x0;
    backup_data[0] = data[0];
    wiringPiSPIDataRW(0, data, 1);
    data[0] = backup_data[0];
}

void doGraph(int num)
{
    int i, thisLed;
    _Bool toggle;
    _Bool con;

    con = 1;
    if (num) return;
    else
    {
        for (i = 0; i < 8; i++)
        {
            thisLed = i;
            if (num == 0)
            {
                Off(); 
                con = 0;
            }
            if (con)
            {
                toggle = (thisLed < num);
                bitWrite(thisLed, toggle);
                backup_data[0] = data[0];
                wiringPiSPIDataRW(0, data, 1);
                data[0] = backup_data[0];
            }
            con = 1;
        }
    }
}

void die(int sig)
{
    data[0] = 0b00000000;
    wiringPiSPIDataRW(0, data, 1);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
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
  wiringPiSPISetup(0, 500000);
  /*
     printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
     bitSet(3);
     wiringPiSPIDataRW(0, data, 1);
     bitSet(5);
     printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
     wiringPiSPIDataRW(0, data, 1);
     bitClear(3);
     printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
     wiringPiSPIDataRW(0, data, 1);
     printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
   */
  while (1)
  {
    for (i = 0; i < 9; i++)
    {
      doGraph(i);
      delay(100);
    }
    for (i = 8; i > -1; i--)
    {
      doGraph(i);
      delay(100);
    }
  }
  return EXIT_SUCCESS;
}

