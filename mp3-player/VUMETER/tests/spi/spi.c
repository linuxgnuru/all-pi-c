#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#define DO_LOOP
//#define DO_TOG

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

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

#if 0
void onOff(_Bool b)
{
    int thisLed;

    for (thisLed = 0; thisLed < 8; thisLed++)
    {
        //ledState[thisLed] = b;
        if (thisLed < 8) digitalWrite(ADDR + thisLed, b);
        else digitalWrite(ledPins[thisLed - 8], b);
    }
}

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
#endif

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
    //onOff(0);
    data[0] = data[1] = 0b00000000;
    wiringPiSPIDataRW(0, data, 2);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
#ifdef DO_TOG
    int loc = 0;
    int chip = 0;
#endif
#ifdef DO_LOOP_1
    int i, j, k;
    int x = 75;
#endif
#ifdef DO_LOOP
    int i, j;
    int x = 75;
#endif
    //int number = 0;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
#ifdef DO_TOG
    if (argc > 2)
    {
        chip = atoi(argv[1]);
        loc = atoi(argv[2]);
    }
    else
    {
        printf("usage: %s [chip (0/1)] [led (0-7)]\n", argv[0]);
        return EXIT_FAILURE;
    }
#endif
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(0, 500000);
#ifdef DO_TOG
    if (chip == 2)
    {
        data[0] = data[1] = 0b00000000;
        wiringPiSPIDataRW(0, data, 2);
    }
    else
    {
        //bitWrite(loc, 1);
        bitSet(chip, loc);
        wiringPiSPIDataRW(0, data, 2);
        delay(500);
        bitClear(chip, loc);
        wiringPiSPIDataRW(0, data, 2);
    }
#endif

#ifdef DEBUG
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
    bitSet(3);
    wiringPiSPIDataRW(0, data, 1);
    bitSet(5);
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
    wiringPiSPIDataRW(0, data, 1);
    bitClear(3);
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
    wiringPiSPIDataRW(0, data, 1);
    printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
#endif

#ifdef DO_LOOP_1 // loop through each led
    while (1)
    {
        for (i = 0; i < 2; i++)
        {
            for (j = 7; j > -1; j--)
            {
                k = (i == 0 ? abs(j - 7) : j);
                bitSet(i, k);
#ifdef DEBUG
                printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
#endif
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                //backup_data[k] = data[k]; // this does not work.
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
                //data[k] = backup_data[k]; // this does not work.
                printf("%2d, %2d\n", k, j);
                delay(x);
            }
        }
        for (i = 0; i < 2; i++)
        {
            for (j = 0; j < 8; j++)
            {
                k = (i == 0 ? abs(j - 7) : j);
                bitClear(i, k);
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                //backup_data[k] = data[k];
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
                //data[k] = backup_data[k];
                delay(x);
            }
        }
    }
#endif // end loop
#ifdef DO_LOOP
    while (1)
    {
            for (i = 7; i > -1; i--)
            {
                j = abs(i - 7);
                bitSet(0, j);
                bitSet(1, i);
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                //backup_data[k] = data[k]; // this does not work.
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
                //data[k] = backup_data[k]; // this does not work.
#ifdef DEBUG
                printf("%2d, %2d\n", k, j);
#endif
                delay(x);
            }
            for (i = 0; i < 8; i++)
            {
                j = abs(i - 7);
                bitClear(0, j);
                bitClear(1, i);
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                //backup_data[k] = data[k];
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
                //data[k] = backup_data[k];
                delay(x);
            }
    }
#endif // end loop
    //for (i = 0; i < 10; i++) ledState[i] = 0;
    return EXIT_SUCCESS;
}
#if 0

bitSet(0, i);
bitSet(1, i);
//printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
backup_data[0] = data[0];
backup_data[1] = data[1];
wiringPiSPIDataRW(0, data, 2);
data[0] = backup_data[0];
data[1] = backup_data[1];
delay(x);
}
for (j = 7; j > -1; j--)
{
    for (i = 0; i < 2; i++)
    {
        bitClear(0, i);
        bitClear(1, i);
        backup_data[0] = data[0];
        backup_data[1] = data[1];
        wiringPiSPIDataRW(0, data, 2);
        data[0] = backup_data[0];
        data[1] = backup_data[1];
        delay(x);
    }
}
#endif
