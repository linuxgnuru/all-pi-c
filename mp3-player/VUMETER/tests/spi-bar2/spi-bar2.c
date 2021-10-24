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

unsigned char data[2] = { 0x0, 0x0 };
unsigned char backup_data[2] = { 0x0, 0x0 };

void bitWrite(int ab, unsigned int n, int b)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (-b ^ data[ab]) & (1 << n);
}

void bitClear(int ab, unsigned int n)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (0 ^ data[ab]) & (1 << n);
}

void bitSet(int ab, unsigned int n)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (-1 ^ data[ab]) & (1 << n);
}

void Off()
{
    data[0] = data[1] = 0x0;
    backup_data[0] = data[0];
    backup_data[1] = data[1];
    wiringPiSPIDataRW(0, data, 2);
    data[0] = backup_data[0];
    data[1] = backup_data[1];
}

void Off_r()
{
    data[1] = 0b00000000;
    backup_data[0] = data[0];
    backup_data[1] = data[1];
    wiringPiSPIDataRW(0, data, 2);
    data[0] = backup_data[0];
    data[1] = backup_data[1];
}

void Off_l()
{
    data[0] = 0b00000000;
    backup_data[0] = data[0];
    backup_data[1] = data[1];
    wiringPiSPIDataRW(0, data, 2);
    data[0] = backup_data[0];
    data[1] = backup_data[1];
}

void doGraph(int num_r, int num_l)
{
    int i, thisLed_l, thisLed_r;
    _Bool toggle_l, toggle_r;
    _Bool con_l, con_r;

    con_l = con_r = 1;
    if (num_r < 0 || num_r > 8) return;
    else if (num_l < 0 || num_l > 8) return;
    else
    {
        // left side
        for (i = 0; i < 8; i++)
        {
            thisLed_r = i;
            thisLed_l = abs(7 - i);
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
                toggle_l = (thisLed_l < num_l);
                bitWrite(0, thisLed_l, toggle_l);
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
            }
            if (con_r)
            {
                toggle_r = (thisLed_l < num_r);
                bitWrite(1, thisLed_r, toggle_r);
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
            }
            con_l = con_r = 1;
        }
    }
}

void die(int sig)
{
    data[0] = data[1] = 0b00000000;
    wiringPiSPIDataRW(0, data, 2);
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
      doGraph(i, i);
      delay(100);
    }
    for (i = 8; i > -1; i--)
    {
      doGraph(i, i);
      delay(100);
    }
#if 0
    for (j = 0; j < 8; j++)
    {
      //data[0] = 0b00000001;
      //data[0] = 0x01 << i;
      //wiringPiSPIDataRW(0, data, sizeof(data));
      //bitWrite(data[0], i, 1);
      //bitWrite(i, 1);
      bitSet(0, j);
      bitSet(1, j);
      //printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
      backup_data[0] = data[0];
      backup_data[1] = data[1];
      wiringPiSPIDataRW(0, data, 2);
      data[0] = backup_data[0];
      data[1] = backup_data[1];
      delay(x);
    }
    for (i = 7; i > -1; i--)
    {
      bitClear(0, i);
      bitClear(1, i);
      backup_data[0] = data[0];
      backup_data[1] = data[1];
      wiringPiSPIDataRW(0, data, 2);
      data[0] = backup_data[0];
      data[1] = backup_data[1];
      delay(x);
    }
#endif
  }
  return EXIT_SUCCESS;
}

