#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <time.h> // for random

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

#define CE 0

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

unsigned char data[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

unsigned long lm = 0;

void bitWrite(int col, int row, int b)
{
    if (col < 0 || col > 7 || row < 0 || row > 7) return;
    data[col] ^= (-b ^ data[col]) & (1 << row);
}

void bitClear(int col, int row)
{
    if (col < 0 || col > 7 || row < 0 || row > 7) return;
    data[col] ^= (0 ^ data[col]) & (1 << row);
}

void bitSet(int col, int row)
{
    if (col < 0 || col > 7 || row < 0 || row > 7) return;
    data[col] ^= (-1 ^ data[col]) & (1 << row);
}

void writeMaxByte(unsigned char addr, unsigned char dat)
{
    unsigned char td[2];

    td[0] = addr;
    td[1] = dat;
    wiringPiSPIDataRW(0, td, 2);
}

void writeMax()
{
    for (int i = 0; i < 8; i++)
        writeMaxByte(i + 1, data[i]);
}

void clearMax()
{
    for (int i = 0; i < 8; i++)
        writeMaxByte(i + 1, 0x0);
}

void Init_MAX7219()
{
    writeMaxByte(0x09, 0x00);
    writeMaxByte(0x0a, 0x03);
    writeMaxByte(0x0b, 0x07);
    writeMaxByte(0x0c, 0x01);
    writeMaxByte(0x0f, 0x00);
    clearMax();
}

void doGraph(int c, int r)
{
    c = abs(c - 7);
    for (int i = 0; i < 8; i++)
    {
        if (r == 0)
            data[c] = 0x0;
        else
            bitWrite(c, i, (i < r));
        writeMax();
    }
}

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    int i;
    unsigned long cm;

    srand((unsigned)time(NULL));
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%d] [%s] [%s] Error trying to setup wiringPi - oops: %s\n", __LINE__, __FILE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(CE, 500000);
    Init_MAX7219();
    i = 0;
    while (1)
    {
        cm = millis();
        if (cm - lm >= 25)
        {
            lm = cm;
            int ran = rand() % 9;
            doGraph(i, ran);
            if (++i > 8)
                i = 0;
        }
    }
    return EXIT_SUCCESS;
}

