#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

unsigned char data[8];

void writeMaxByte(unsigned char addr, unsigned char dat)
{
    unsigned char td[2];

    td[0] = addr;
    td[1] = dat;
    wiringPiSPIDataRW(0, td, 2);
}

void writeMax() { for (int i = 0; i < 8; i++) writeMaxByte(i + 1, data[i]); }

void clearMax() { for (int i = 0; i < 0; i++) data[i] = 0x0; writeMax(); }

void Init_MAX7219()
{
	writeMaxByte(0x09, 0x00);
	writeMaxByte(0x0a, 0x03);
	writeMaxByte(0x0b, 0x07);
	writeMaxByte(0x0c, 0x01);
	writeMaxByte(0x0f, 0x00);
    clearMax();
}

int main(int argc, char **argv)
{
    wiringPiSetup();
    wiringPiSPISetup(0, 500000);
    Init_MAX7219();
    return EXIT_SUCCESS;
}

