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
//unsigned char backup_data[8];

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
  {
    //b_data[i] = data[i];
    writeMaxByte(i + 1, data[i]);
    //data[i] = b_data[i];
  }
}

void clearMax()
{
  for (int i = 0; i < 8; i++)
    writeMax(i + 1, 0x0);
}

void Init_MAX7219()
{
  writeMaxByte(0x09, 0x00);
  writeMaxByte(0x0a, 0x03);
  writeMaxByte(0x0b, 0x07);
  writeMaxByte(0x0c, 0x01);
  writeMaxByte(0x0f, 0x00);
  clearMax();
  /*
     unsigned char init[2];

     init[0] = 0x09; init[1] = 0x00; wiringPiSPIDataRW(0, init, 2);
     init[0] = 0x0a; init[1] = 0x03; wiringPiSPIDataRW(0, init, 2);
     init[0] = 0x0b; init[1] = 0x07; wiringPiSPIDataRW(0, init, 2);
     init[0] = 0x0c; init[1] = 0x01; wiringPiSPIDataRW(0, init, 2);
     init[0] = 0x0f; init[1] = 0x00; wiringPiSPIDataRW(0, init, 2);
   */
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
    data[i] = 0x0;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
    {
      printf("b %d,%d data[j]: "BYTE_TO_BINARY_PATTERN"\t", i, j, BYTE_TO_BINARY(data[j]));
      bitSet(j, i);
      //backup_data[j] = data[j];
      writeMax(j + 1, data[j]);
      printf("a %d,%d data[j]: "BYTE_TO_BINARY_PATTERN"\n", i, j, BYTE_TO_BINARY(data[j]));
      //data[j] = backup_data[j];
      delay(100);
    }
  }
  //clearMax();
  return EXIT_SUCCESS;
}

