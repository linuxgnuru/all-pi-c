#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

#define CE 0

#define uchar unsigned char
#define uint unsigned int

uchar data[8] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};
//unsigned char backup_data[8];
//unsigned long lm = 0;

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
    wiringPiSPIDataRW(CE, td, 2);
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

void writeMax()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 8; j++)
            writeMaxByte(j + 1, data[j]);
}

void clearMax()
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            writeMaxByte(i + 1, 0x0);
        }
    }
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

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    int i, j;
    //unsigned long cm;

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
#if 0
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default
    bcm2835_gpio_fsel(Max7219_pinCS, BCM2835_GPIO_FSEL_OUTP); 
    bcm2835_gpio_write(data[0], HIGH);
#endif
    wiringPiSPISetup(CE, 500000);
    Init_MAX7219();
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
    clearMax();
    return EXIT_SUCCESS;
}

