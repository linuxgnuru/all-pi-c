#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>

#define CE 1

int main(int argc, char **argv)
{
    static uint8_t data[2] = {0x0,0x0};
    static uint8_t heart[8] = {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};             
    int i;
    int x = 2;

    wiringPiSetup();
    wiringPiSPISetup(CE, 500000);
    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            data[0] = 0b00000001;
            data[1] = 0b10000000;
            /*
            data[0] = ~heart[i];
            data[2] = 0xFF;
            data[1] = 0xFF;
            data[3] = 0x01 << i;
            */
            wiringPiSPIDataRW(CE, data, sizeof(data));
            delay(x);
        }
    }
    return EXIT_SUCCESS;
}
