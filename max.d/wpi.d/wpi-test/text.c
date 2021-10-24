#include <bcm2835.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define uchar unsigned char
#define uint unsigned int


//#define Max7219_pinCLK  RPI_GPIO_P1_11
#define Max7219_pinCS  RPI_GPIO_P1_24
//#define Max7219_pinDIN  RPI_V2_GPIO_P1_13

char characters[37] = { 
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', ' ' 
};

uchar disp1[37][8] = {
    {0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C},//0
    {0x10,0x30,0x50,0x10,0x10,0x10,0x10,0x7C},//1
    {0x3E,0x02,0x02,0x3E,0x20,0x20,0x3E,0x00},//2
    {0x00,0x7C,0x04,0x04,0x7C,0x04,0x04,0x7C},//3
    {0x08,0x18,0x28,0x48,0xFE,0x08,0x08,0x08},//4
    {0x3C,0x20,0x20,0x3C,0x04,0x04,0x3C,0x00},//5
    {0x3C,0x20,0x20,0x3C,0x24,0x24,0x3C,0x00},//6
    {0x3E,0x22,0x04,0x08,0x08,0x08,0x08,0x08},//7
    {0x00,0x3E,0x22,0x22,0x3E,0x22,0x22,0x3E},//8
    {0x3E,0x22,0x22,0x3E,0x02,0x02,0x02,0x3E},//9
    {0x08,0x14,0x22,0x3E,0x22,0x22,0x22,0x22},//A
    {0x3C,0x22,0x22,0x3E,0x22,0x22,0x3C,0x00},//B
    {0x3C,0x40,0x40,0x40,0x40,0x40,0x3C,0x00},//C
    {0x7C,0x42,0x42,0x42,0x42,0x42,0x7C,0x00},//D
    {0x7C,0x40,0x40,0x7C,0x40,0x40,0x40,0x7C},//E
    {0x7C,0x40,0x40,0x7C,0x40,0x40,0x40,0x40},//F
    {0x3C,0x40,0x40,0x40,0x40,0x44,0x44,0x3C},//G
    {0x44,0x44,0x44,0x7C,0x44,0x44,0x44,0x44},//H
    {0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x7C},//I
    {0x3C,0x08,0x08,0x08,0x08,0x08,0x48,0x30},//J
    {0x00,0x24,0x28,0x30,0x20,0x30,0x28,0x24},//K
    {0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7C},//L
    {0x81,0xC3,0xA5,0x99,0x81,0x81,0x81,0x81},//M
    {0x00,0x42,0x62,0x52,0x4A,0x46,0x42,0x00},//N
    {0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C},//O
    {0x3C,0x22,0x22,0x22,0x3C,0x20,0x20,0x20},//P
    {0x1C,0x22,0x22,0x22,0x22,0x26,0x22,0x1D},//Q
    {0x3C,0x22,0x22,0x22,0x3C,0x24,0x22,0x21},//R
    {0x00,0x1E,0x20,0x20,0x3E,0x02,0x02,0x3C},//S
    {0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08},//T
    {0x42,0x42,0x42,0x42,0x42,0x42,0x22,0x1C},//U
    {0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18},//V
    {0x00,0x49,0x49,0x49,0x49,0x2A,0x1C,0x00},//W
    {0x00,0x41,0x22,0x14,0x08,0x14,0x22,0x41},//X
    {0x41,0x22,0x14,0x08,0x08,0x08,0x08,0x08},//Y
    {0x00,0x7F,0x02,0x04,0x08,0x10,0x20,0x7F},//Z
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//space
};

void Delay_xms(uint x)
{
    bcm2835_delay(x);
}
//------------------------

void Write_Max7219_byte(uchar DATA)
{
    bcm2835_gpio_write(Max7219_pinCS,LOW);
    bcm2835_spi_transfer(DATA);
}

void Write_Max7219(uchar address, uchar dat)
{
    bcm2835_gpio_write(Max7219_pinCS, LOW);
    Write_Max7219_byte(address);
    Write_Max7219_byte(dat);
    bcm2835_gpio_write(Max7219_pinCS, HIGH);
}

void Init_MAX7219()
{
    /*
    Write_Max7219(0x09, 0x00, 0x09, 0x00);
    Write_Max7219(0x0a, 0x03, 0x0a, 0x03);
    Write_Max7219(0x0b, 0x07, 0x0b, 0x07);
    Write_Max7219(0x0c, 0x01, 0x0c, 0x01);
    Write_Max7219(0x0f, 0x00, 0x0f, 0x00);
    */
    Write_Max7219(0x09, 0x00);
    Write_Max7219(0x0a, 0x03);
    Write_Max7219(0x0b, 0x07);
    Write_Max7219(0x0c, 0x01);
    Write_Max7219(0x0f, 0x00);
}

uchar find_char(char letter)
{
    for (uchar index = 0; index < 37; index++)
    {
        if (letter == characters[index])
        {
            return index;
        }
    }
    //Character not found, return index of space
    return 36;
}

void led_print(int char1)
{
    for (uchar i = 1; i < 9; i++)
    {
        //Write_Max7219(i, disp1[char2][i-1],i,disp1[char1][i-1]);
        Write_Max7219(i, disp1[char1][i-1]);
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Enter text as an argument.\n");
        return 1;
    }
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
    bcm2835_gpio_write(disp1[0][0], HIGH);
    Delay_xms(50);
    Init_MAX7219();

    size_t index = 0;
    while (index < strlen(argv[1]))
    {
        // Retrive first letter
        char letter1 = toupper(argv[1][index]);
        index++;
        // Retrieve second letter
        // If it does not exist use space
        char letter2 = (index < strlen(argv[1])) ? toupper(argv[1][index]) : (char) 0;
        //led_print(find_char(letter1), find_char(letter2));
        led_print(find_char(letter1));
        led_print(find_char(letter2));
        Delay_xms(1000);
        index++;
    }
    // Clear screen
    //led_print(36, 36);
    led_print(36);
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

