#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CE 1

const int extraLedPins[] = { 29, 28 }; // for bar graph leds 9 and 10

unsigned char data[1] = { 0x0 };
unsigned char backup_data[1] = { 0x0 };

void bitWrite(int n, int b) { if (n <= 7 && n >= 0) data[0] ^= (-b ^ data[0]) & (1 << n); }
void bitClear(int n) { if (n <= 7 && n >= 0) data[0] ^= (0 ^ data[0]) & (1 << n); }
void bitSet(int n) { if (n <= 7 && n >= 0) data[0] ^= (-1 ^ data[0]) & (1 << n); }

void Off()
{
    data[0] = 0x0;
    backup_data[0] = data[0];
    wiringPiSPIDataRW(CE, data, 1);
    data[0] = backup_data[0];
    digitalWrite(extraLedPins[0], LOW);
    digitalWrite(extraLedPins[1], LOW);
}

void On()
{
    data[0] = 0b11111111;
    backup_data[0] = data[0];
    wiringPiSPIDataRW(CE, data, 1);
    data[0] = backup_data[0];
    digitalWrite(extraLedPins[0], HIGH);
    digitalWrite(extraLedPins[1], HIGH);
}

void doGraph(int num)
{
    int thisLed, lednum;
    _Bool toggle;

    if (num < 0 || num > 10) return;
    if (num == 0) Off();
    else if (num == 10) On();
    else
    {
        for (thisLed = 0; thisLed < 10; thisLed++)
        {
            lednum = thisLed;
            toggle = (thisLed < num);
            if (thisLed < 8)
            {
                lednum = 7 - lednum;
                bitWrite(lednum, toggle);
                backup_data[0] = data[0];
                wiringPiSPIDataRW(CE, data, 1);
                data[0] = backup_data[0];
            }
            else
            {
                lednum -= 8;
                digitalWrite(extraLedPins[lednum], toggle);
            }
        }
    }
}

double map(float x, float x0, float x1, float y0, float y1)
{
    float y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    double z = (double)y;
    return z;
}

int main(int argc, char **argv)
{
    int num;
    if (argc == 1)
    {
        printf("usage: %s [num]\n", argv[0]);
        return EXIT_FAILURE;
    }
    num = atoi(argv[1]);
    if (num < 0 || num > 10)
    {
        printf("num must be 0 - 10\n");
        printf("usage: %s [num]\n", argv[0]);
        return EXIT_FAILURE;
    }
    wiringPiSetup();
    wiringPiSPISetup(CE, 500000);
    doGraph(num);
    return 0;
}

