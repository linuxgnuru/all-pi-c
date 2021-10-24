/*        (y) (g) (b) (k) (r)
           C   L   D   G   V
           |   |   |   |   |
           |   |   |   |   |
     7   6   5   4   3   2   1   0
   +---+---+---+---+---+---+---+---+
 7 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 6 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 5 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 4 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 3 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 0 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
           |   |   |   |   |
           |   |   |   |   |

*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <time.h> // for srand

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CE 0

#define uchar unsigned char
#define uint unsigned int

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

uchar data[8] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

void bitWrite(int col, int row, int b)
{
    if (col < 0 || col > 8 || row < 0 || row > 8)
        return;
    data[col] ^= (-b ^ data[col]) & (1 << row);
}

void bitClear(int col, int row)
{
    if (col < 0 || col > 8 || row < 0 || row > 8)
        return;
    data[col] ^= (0 ^ data[col]) & (1 << row);
}

void bitSet(int col, int row)
{
    if (col < 0 || col > 8 || row < 0 || row > 8)
        return;
    data[col] ^= (-1 ^ data[col]) & (1 << row);
}

void writeMaxByte(uchar addr, uchar dat)
{
    uchar td[2];

    td[0] = addr;
    td[1] = dat;
    wiringPiSPIDataRW(0, td, 2);
}

void writeMax()
{
    for (int i = 0; i < 8; i++)
        writeMaxByte(i + 1, data[i]);
}

void writeMaxByte4(uchar addr, uchar dat)
{
    uchar td[8];

    for (int i = 0; i < 4; i++)
    {
        td[i] = addr;
        td[i + 1] = dat;
    }
    wiringPiSPIDataRW(CE, td, 8);
}

void clearMax()
{
    for (int i = 0; i < 8; i++)
        writeMaxByte(i + 1, 0x0);
}

void Init_MAX7219()
{
    writeMaxByte4(0x09, 0x00);
    writeMaxByte4(0x0a, 0x03);
    writeMaxByte4(0x0b, 0x07);
    writeMaxByte4(0x0c, 0x01);
    writeMaxByte4(0x0f, 0x00);
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
    int col[8];

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (argc != 9 && argc != 2)
    {
        fprintf(stdout, "usage: %s [col1] ... [col8]\n", argv[0]);
        return EXIT_FAILURE;
    }
    srand(time(NULL));
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%d] [%s] [%s] Error trying to setup wiringPi - oops: %s\n", __LINE__, __FILE__, __func__, strerror(errno));
        exit(1);
    }
    for (int i = 1; i < 9; i++)
    {
        col[i - 1] = (argc == 9 ? atoi(argv[i]) : rand() % 8);
        if (col[i - 1] < 0 || col[i - 1] > 8)
        {
            fprintf(stdout, "col must be between 0 and 8\n");
            return EXIT_FAILURE;
        }
    }
    wiringPiSPISetup(CE, 500000);
    Init_MAX7219();
    for (int i = 0; i < 8; i++)
        doGraph(i, col[i]);
    /*
       for (i = 0; i < 8; i++)
       {
       for (j = 0; j < 8; j++)
       {
       bitSet(j, i);
    //backup_data[j] = data[j];
    writeMax(j + 1, data[j]);
    //data[j] = backup_data[j];
    delay(100);
    }
    }
     */
    return EXIT_SUCCESS;
}

