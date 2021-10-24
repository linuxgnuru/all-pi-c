// blink.c
//
// Example program for bcm2835 library
// Blinks a pin on an off every 0.5 secs
//
// After installing bcm2835, you can build this 
// with something like:
// make or gcc -o led led.c -lbcm2835
// sudo ./led
/*
             define from bcm2835.h                       define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
    RPI_V2_GPIO_P1_03 | | 5V               ->                  SDA | | 5V
    RPI_V2_GPIO_P1_05 | | GND              ->                  SCL | | GND
       RPI_GPIO_P1_07 | | RPI_GPIO_P1_08   ->                  IO7 | | TX
                  GND | | RPI_GPIO_P1_10   ->                  GND | | RX
       RPI_GPIO_P1_11 | | RPI_GPIO_P1_12   ->                  IO0 | | IO1
    RPI_V2_GPIO_P1_13 | | GND              ->                  IO2 | | GND
       RPI_GPIO_P1_15 | | RPI_GPIO_P1_16   ->                  IO3 | | IO4
                  VCC | | RPI_GPIO_P1_18   ->                  VCC | | IO5
       RPI_GPIO_P1_19 | | GND              ->                 MOSI | | GND
       RPI_GPIO_P1_21 | | RPI_GPIO_P1_22   ->                 MISO | | IO6
       RPI_GPIO_P1_23 | | RPI_GPIO_P1_24   ->                  SCK | | CE0
                  GND | | RPI_GPIO_P1_26   ->                  GND | | CE1

::if your raspberry Pi is version 1 or rev 1 or rev A
RPI_V2_GPIO_P1_03->RPI_GPIO_P1_03
RPI_V2_GPIO_P1_05->RPI_GPIO_P1_05
RPI_V2_GPIO_P1_13->RPI_GPIO_P1_13:

*/
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// for signal catching
#include <errno.h>
#include <signal.h>

//#include <wiringPi.h>

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


//#define Max7219_pinCLK  RPI_GPIO_P1_11
#define Max7219_pinCS  RPI_GPIO_P1_24
//#define Max7219_pinDIN  RPI_V2_GPIO_P1_13

/*
unsigned char data[11][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//space
    {0b10000001,0b01000010,0b00100100,0b00011000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
    {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}
};
*/

unsigned char data[8];

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

void Delay_xms(unsigned int x)
{
    bcm2835_delay(x);
}

void bitWrite(int col, int row, int b)
{
    if (row < 0 || row > 8 || col < 0 || col > 8) return;
    data[col] ^= (-b ^ data[col]) & (1 << row);
}

void bitClear(int col, int row)
{
    if (row < 0 || row > 8 || col < 0 || col > 8) return;
    data[col] ^= (0 ^ data[col]) & (1 << row);
}

void bitSet(int col, int row)
{ 
    if (row < 0 || row > 8 || col < 0 || col > 8) return;
    data[col] ^= (-1 ^ data[col]) & (1 << row);
}

void Write_Max7219_byte(unsigned char DATA)
{
    bcm2835_gpio_write(Max7219_pinCS, LOW);
    bcm2835_spi_transfer(DATA);
}

void Write_Max7219(unsigned char address1, unsigned char dat1)
{
    bcm2835_gpio_write(Max7219_pinCS, LOW);
    Write_Max7219_byte(address1);
    Write_Max7219_byte(dat1);
    bcm2835_gpio_write(Max7219_pinCS, HIGH);
}

void Init_MAX7219()
{
    Write_Max7219(0x09,0x00);
    Write_Max7219(0x0a,0x03);
    Write_Max7219(0x0b,0x07);
    Write_Max7219(0x0c,0x01);
    Write_Max7219(0x0f,0x00);
}

int main(int argc, char *argv[])
{
    int i, j;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (!bcm2835_init())
    {
        printf("Unable to init bcm2835.\n");
        return 2;
    }

    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default
    bcm2835_gpio_fsel(Max7219_pinCS, BCM2835_GPIO_FSEL_OUTP); 
    bcm2835_gpio_write(data[0], HIGH);

    Delay_xms(50);
    Init_MAX7219();
    for (i = 0; i < 8; i++)
    {
        data[i] = 0x0;
        Write_Max7219(i + 1, data[i]);
    }
    //printf("i,j\n");
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            bitSet(j, i);
            //if (data[7] == 0xff) break;
            //printf("%d,%d\t", i, j);
            //printf("data[j]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[j]>>8), BYTE_TO_BINARY(data[j]));
            //Write_Max7219(i, disp1[char2][i-1], i, disp1[char1][i-1]);
            //Write_Max7219(i, data[c][i-1]);
            Write_Max7219(j + 1, data[j]);
            Delay_xms(50);
        }
        //if (data[7] == 0xff) break;
    }
    for (i = 0; i < 8; i++)
        Write_Max7219(i + 1, 0x00);
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

