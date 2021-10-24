#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#define DO_LOOP

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

#define CE 1
#define NUM_CHIPS 10

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

unsigned char data[NUM_CHIPS] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

unsigned char backup_data[NUM_CHIPS] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

void bitWrite(int c, int n, int b)
{
    if (c < 0 || c > NUM_CHIPS) return;
    if (n <= 7 && n >= 0) data[c] ^= (-b ^ data[c]) & (1 << n);
}

void bitClear(int c, int n)
{
    if (c < 0 || c > NUM_CHIPS) return;
    if (n <= 7 && n >= 0) data[c] ^= (0 ^ data[c]) & (1 << n);
}

void bitSet(int c, int n)
{
    if (c < 0 || c > NUM_CHIPS) return;
    if (n <= 7 && n >= 0) data[c] ^= (-1 ^ data[c]) & (1 << n);
}

static void die(int sig)
{
    for (int i = 0; i < NUM_CHIPS; i++)
        data[i] = 0b00000000;
    wiringPiSPIDataRW(CE, data, NUM_CHIPS);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    int i, j, k;
    int x = 75;
    //int number = 0;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(CE, 500000);
#ifdef DEBUG
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
    bitSet(3);
    wiringPiSPIDataRW(CE, data, 1);
    bitSet(5);
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
    wiringPiSPIDataRW(CE, data, 1);
    bitClear(3);
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
    wiringPiSPIDataRW(CE, data, 1);
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
#endif
    while (1)
    {
        for (i = 0; i < NUM_CHIPS; i++)
        {
            for (j = 0; j < 8; j++)
            {
                bitSet(i, j);
                //j = abs(i - 7);
                //bitSet(0, j);
                //bitSet(1, i);
                for (k = 0; k < NUM_CHIPS; k++)
                    backup_data[k] = data[k];
                wiringPiSPIDataRW(CE, data, NUM_CHIPS);
                for (k = 0; k < NUM_CHIPS; k++)
                    data[k] = backup_data[k];
                delay(x);
            }
            for (j = 0; j < 8; j++)
            {
                //j = abs(i - 7);
                //bitClear(0, j);
                //bitClear(1, i);
                bitClear(i, j);
                for (k = 0; k < NUM_CHIPS; k++)
                    backup_data[k] = data[k];
                wiringPiSPIDataRW(CE, data, NUM_CHIPS);
                for (k = 0; k < NUM_CHIPS; k++)
                    data[k] = backup_data[k];
                delay(x);
            }
        }
    }
    return EXIT_SUCCESS;
}

