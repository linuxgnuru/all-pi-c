#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

int main()
{
    wiringPiSetup();
    wiringPiSPISetup(1, 500000);
    wiringPiSPIDataRW(1, 0x0, 1);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
    digitalWrite(28, LOW);
    digitalWrite(29, LOW);
    return 0;
}
