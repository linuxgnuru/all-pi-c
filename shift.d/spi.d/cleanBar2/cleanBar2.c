#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

#define CE 1

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

/*
   void doGraph(int num)
   {
   int thisLed;
   _Bool toggle;

   if (num < 0 || num > 10)
   return;
   if (num == 0)
   onOff(0);
   else if (num == 10)
   onOff(1);
   else
   {
   for (thisLed = 0; thisLed < 10; thisLed++)
   {
   toggle = (thisLed < num);
//ledState[thisLed] = toggle;
if (thisLed < 8) digitalWrite(ADDR + thisLed, toggle);
else digitalWrite(ledPins[thisLed - 8], toggle);
}
}
}
 */

unsigned char data[2] = { 0x0, 0x0 };
unsigned char backup_data[2] = { 0x0, 0x0 };

//void bitWrite(uint8_t &x, unsigned int n, _Bool b)
void bitWrite(int ab, unsigned int n, int b)
{
    if (ab < 0 || ab > 1)
        return;
    if (n <= 7 && n >= 0)
    {
        data[ab] ^= (-b ^ data[ab]) & (1 << n);
        /*
           if (b) data[0] |= (1u << n);
           else data[0] &= ~(1u << n);
         */
    }
}

void bitClear(int ab, unsigned int n)
{
    if (ab < 0 || ab > 1)
        return;
    if (n <= 7 && n >= 0)
        //data[0] &= ~(1u << n);
        data[ab] ^= (0 ^ data[ab]) & (1 << n);
}

void bitSet(int ab, unsigned int n)
{
    if (ab < 0 || ab > 1)
        return;
    if (n <= 7 && n >= 0)
        //data[0] |= (1u << n);
        data[ab] ^= (-1 ^ data[ab]) & (1 << n);
}

static void die(int sig)
{
    data[0] = data[1] = 0b00000000;
    wiringPiSPIDataRW(CE, data, 2);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    //static uint8_t data[2] = { 0x0, 0x0 };
    //    unsigned char data[1] = { 0x0 };
    int number = 0;
    int i;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    //if (argc > 1) { loc = atoi(argv[1]); }
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(CE, 500000);
    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            bitSet(0, i);
            bitSet(1, i);
            backup_data[0] = data[0];
            backup_data[1] = data[1];
            wiringPiSPIDataRW(CE, data, 2);
            data[0] = backup_data[0];
            data[1] = backup_data[1];
            delay(x);
        }
        for (i = 7; i > -1; i--)
        {
            bitClear(0, i);
            bitClear(1, i);
            backup_data[0] = data[0];
            backup_data[1] = data[1];
            wiringPiSPIDataRW(CE, data, 2);
            data[0] = backup_data[0];
            data[1] = backup_data[1];
            delay(x);
        }
    }
    //doGraph(number);
    return EXIT_SUCCESS;
}

