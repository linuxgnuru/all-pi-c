#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// for signal catching
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

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

unsigned char data[8];

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

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

int main(int argc, char *argv[])
{
    int i, j;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%d] [%s] [%s] Error trying to setup wiringPi - oops: %s\n", __LINE__, __FILE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(0, 500000);
    /*
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default
    bcm2835_gpio_fsel(Max7219_pinCS, BCM2835_GPIO_FSEL_OUTP); 
    bcm2835_gpio_write(data[0], HIGH);
    */
    Init_MAX7219();
    for (i = 0; i < 8; i++)
    {
        data[i] = 0x0;
        writeMax7219(i + 1, data[i]);
    }
    doGraph(4, 0);
    //for (i = 0; i < 8; i++) writeMax7219(i + 1, 0x00);
    return EXIT_SUCCESS;
}

