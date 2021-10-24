#include <stdio.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

int main()
{
    wiringPiSetup();
    wiringPiSPISetup(1, 500000);
    wiringPiSPIDataRW(1, 0x0, 1);
    pinMode(29, OUTPUT);
    pinMode(28, OUTPUT);
    digitalWrite(29, LOW);
    digitalWrite(28, LOW);
    return 0;
}
