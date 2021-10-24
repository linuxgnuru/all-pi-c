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

unsigned char data[8];
//unsigned char backup_data[8];
//unsigned long lm = 0;

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

void Init_MAX7219()
{
    unsigned char init[2];

    init[0] = 0x09; init[1] = 0x00; wiringPiSPIDataRW(0, init, 2);
    init[0] = 0x0a; init[1] = 0x03; wiringPiSPIDataRW(0, init, 2);
    init[0] = 0x0b; init[1] = 0x07; wiringPiSPIDataRW(0, init, 2);
    init[0] = 0x0c; init[1] = 0x01; wiringPiSPIDataRW(0, init, 2);
    init[0] = 0x0f; init[1] = 0x00; wiringPiSPIDataRW(0, init, 2);
}

void writeMax7219(unsigned char addr, unsigned char dat)
{
    unsigned char td[2];

    td[0] = addr;
    td[1] = dat;
    wiringPiSPIDataRW(0, td, 2);
}

void doGraph(int num, int col)
{
    for (int i = 0; i < 8; i++)
    {
        bitWrite(col, i, (i < num));
        writeMax7219(i + 1, data[col]);
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
    wiringPiSPISetup(0, 500000);
    Init_MAX7219();
    //for (i = 0; i < 8; i++) data[i] = backup_data[i] = 0x0;
    for (i = 0; i < 8; i++)
    {
        data[i] = 0x0;
        writeMax7219(i + 1, data[i]);
    }

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            bitSet(j, i);
            //backup_data[j] = data[j];
            writeMax7219(j + 1, data[j]);
            //data[j] = backup_data[j];
            delay(100);
        }
    }
    return EXIT_SUCCESS;
}

