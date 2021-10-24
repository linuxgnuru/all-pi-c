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

//unsigned char data[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
unsigned char spidata[16];
unsigned char status[64];

unsigned long lm = 0;

void bitWrite(int col, int row, int b)
{
    if (col < 0 || col > 8 || row < 0 || row > 8) return;
    data[col] ^= (-b ^ data[col]) & (1 << row);
}

void bitClear(int col, int row)
{
    if (col < 0 || col > 8 || row < 0 || row > 8) return;
    data[col] ^= (0 ^ data[col]) & (1 << row);
}

void bitSet(int col, int row)
{
    if (col < 0 || col > 8 || row < 0 || row > 8) return;
    data[col] ^= (-1 ^ data[col]) & (1 << row);
}

void writeMax(unsigned char addr, unsigned char opcode, unsigned char dat)
{
    //unsigned char td[2];
    int offset = addr * 2;
    int maxbytes = 8; // maxDevices (4) * 2

    for (int i = 0; i < maxbytes; i++)
        spidata[i] = 0x0;
    spidata[offset + 1] = opcode;
    spidata[offset] = dat;
    wiringPiSPIDataRW(CE, spidata, 16);
    //td[0] = addr;
    //td[1] = dat;
    //wiringPiSPIDataRW(CE, td, 2);
}

void clearMax(int addr)
{
    int offset;

    if (addr == 0) // clear all
    {
        for (int i = 0; i < 4; i++)
        {
            offset = i * 8;
            for (int j = 0; j < 8; j++)
            {
                status[offset + j] = 0;
                writeMax(addr, j + 1, status[offset + j]);
            }
        }
    }
    else
    {
        offset = addr * 8;
        for (int i = 0; i < 8; i++)
        {
            status[offset + i] = 0;
            writeMax(addr, i + 1, status[offset + i]);
        }
    }
}

void Init_MAX7219()
{
    for (int i = 0; i < 4; i++)
    {
        writeMax(0x09, 0x00); //  9 decode mode
        writeMax(0x0a, 0x03); // 10 intensity
        writeMax(0x0b, 0x07); // 11 scanlimit
        writeMax(0x0c, 0x01); // 12 shutdown
        writeMax(0x0f, 0x00); // 15 displaytest
        clearMax(i + 1);
    }
}

void setLed(int addr, int row, int column, _Bool state)
{
    int offset;
    unsigned char val = 0x00;

    if (addr < 0 || addr >= 4) return;
    if (row < 0 || row > 7 || column < 0 || column > 7) return;
    offset = addr * 8;
    val = B10000000 >> column;
    if (state)
        status[offset + row] = status[offset + row] | val;
    else
    {
        val =~ val;
        status[offset + row] = status[offset + row] & val;
    }
    writeMax(addr, row + 1, status[offset + row]);
}

void doGraph(int addr, int c, int r)
{
    //c = 7 - c;
    for (int i = 0; i < 8; i++)
    {
        //if (r == 0) data[c] = 0x0;
        //else bitWrite(c, i, (i < r));
        setLed(addr, c, r, (i < r));
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
    int addr;
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
    i = addr = 0;
    while (1)
    {
        cm = millis();
        if (cm - lm >= 25)
        {
            lm = cm;
            int ran = rand() % 9;
            doGraph(addr, i, ran);
            if (++addr > 4)
                addr = 0;
            if (++i > 8)
                i = 0;
        }
    }
    return EXIT_SUCCESS;
}

