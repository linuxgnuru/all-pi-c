#include<stdio.h>
#include<wiringPi.h>
#include<wiringPiSPI.h>
#include<stdint.h>

#define RED_DATA 0
#define BLUE_DATA 1
#define GREEN_DATA 2

#define CE 1

int main(void)
{
    static uint8_t data[4] = {0x0,0x0,0x0,0x0};
    wiringPiSetup();
    wiringPiSPISetup(CE,500000);
    while(1)
    {
        static uint8_t heart[8] = {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};             
        int j;
        int x=2;
        for ( j=0;j<8;j++)
        {
            data[0] = ~heart[j];
            data[2] = 0xFF;
            data[1] = 0xFF;
            data[3] = 0x01 << j ;
            wiringPiSPIDataRW(CE,data,sizeof(data));
            delay(x);
        };
    };
}
