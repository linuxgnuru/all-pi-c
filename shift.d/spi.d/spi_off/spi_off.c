#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h> // for fprintf
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CE 1

int main(void)
{
    unsigned char data[2] = { 0b00000000, 0b00000000 };
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%d] [%s] [%s] Error trying to setup wiringPi - oops: %s\n", __LINE__, __FILE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(CE, 500000);
    wiringPiSPIDataRW(CE, data, 2);
    return EXIT_SUCCESS;
}

