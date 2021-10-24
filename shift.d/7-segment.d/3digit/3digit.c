/*
shift register pins:
       +--------+
 2out -+  1  16 +- Vin
 3out -+  2  15 +- 1out
 4out -+  3  14 +- data (blue)
 5out -+  4  13 +- ground
 6out -+  5  12 +- latch (green + 1u cap if first)
 7out -+  6  11 +- lock (yellow)
 8out -+  7  10 +- Vin
  gnd -+  8   9 +- serial out
       +--------+
*/
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include <wiringPi.h>
#include <sr595.h>

const int addr = 100;

const int dataPin  = 21; // blue (pin 14)
const int latchPin = 22; // green (pin 12)
const int clockPin = 23; // yellow (pin 11)

const int digit1Pin = 25; // turn on digit 2
const int digit2Pin = 24; // turn on digit 1
const int digit3Pin = 26; // turn on digit 3

int bits = 8;

#if 0
static const int ledPins[] = { 
/*
    ---       A
   |   |    F   B
    ---       G
   |   |    E   C
    ---       D     DP (H)
 */
  //  ABCDEFGH
    0b00000001, // 0
    0b00000010, // 1
    0b00000001, // 2
    0b00000010, // 3
    0b00000001, // 4
    0b00000010, // 5
    0b00000001, // 6
    0b00000010, // 7
    0b00000010, // 8
    0b00000010  // 9
};
#endif

//  5      24
// 0 6   25  23
//  1      26
// 2 4   28  21
//  3      22
//    7

static void die(int sig)
{
//    for (int i = 0; i < bits; i++) digitalWrite(addr + i, 0);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

void usage(char *name)
{
    printf("usage: %s [digit]\n", name);
}

void clear()
{
    int i;
    for (i = 0; i < 8; i++)
        digitalWrite(addr + i, LOW);
}

int main(int argc, char **argv)
{
    int i, j;
    int badFlag = 0;
    int pin = 0;
    int pos = 1;
    int nPin;
    int digit;
    int dig[] = { digit1Pin, digit2Pin, digit3Pin };

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (argc > 1)
        digit = atoi(argv[1]);
    else
        return EXIT_FAILURE;
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    sr595Setup(addr, bits, dataPin, clockPin, latchPin);
    pinMode(digit1Pin, OUTPUT);
    pinMode(digit2Pin, OUTPUT);
    pinMode(digit3Pin, OUTPUT);
/*
    digitalWrite(digit1Pin, HIGH);
    digitalWrite(digit2Pin, HIGH);
    digitalWrite(digit3Pin, HIGH);
*/
    digitalWrite(digit1Pin, HIGH);
    digitalWrite(addr + 3, HIGH);
    digitalWrite(dig[0], LOW);
    digitalWrite(addr + digit, HIGH);
//    digitalWrite(dig[0], HIGH);
/*
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 8; j++)
        {
            printf("%d:%d\n", i, j);
            digitalWrite(dig[i], LOW);
            digitalWrite(addr + j, HIGH);
            delay(300);
            digitalWrite(dig[i], HIGH);
        }
        clear();
    }
*/
    return EXIT_SUCCESS;
}
#if 0
    if (argc > 2)
    {
        pin = atoi(argv[1]);
        pos = atoi(argv[2]);
    }
    if (pin > 7 || pin < 0)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    switch (pos)
    {
        case 1:
            digitalWrite(digit1Pin, LOW);
            digitalWrite(digit2Pin, HIGH);
            digitalWrite(digit3Pin, HIGH);
            break;
        case 2:
            digitalWrite(digit1Pin, HIGH);
            digitalWrite(digit2Pin, LOW);
            digitalWrite(digit3Pin, HIGH);
            break;
        case 3:
            digitalWrite(digit1Pin, HIGH);
            digitalWrite(digit2Pin, HIGH);
            digitalWrite(digit3Pin, LOW);
            break;
        default:
            digitalWrite(digit1Pin, LOW);
            digitalWrite(digit2Pin, HIGH);
            digitalWrite(digit3Pin, HIGH);
            break;
    }
    delay(100);
//    digitalWrite(digit1Pin, HIGH);
//    digitalWrite(addr + pin, LOW);
    return EXIT_SUCCESS;
}
    if (argc > 1)
    {
    /*
        digit = atoi(argv[1]);
        if (digit > 10 && digit != 60) badFlag = 1;
    */
    }
    else
        badFlag = 1;
    /*
    printf("digit: %d\n", digit);
    for (i = 0; i < 7; i++) printf("%d ", leds[digit][i]);
    printf("\n");
    */
    if (badFlag == 1)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    for (i = 0; i < 8; i++)
    {
    /*
           if (leds[digit][i] != 30) digitalWrite(addr + leds[digit][i], HIGH);
           if (leds[digit][7] == 40) digitalWrite(addr + 7, HIGH);
        else digitalWrite(addr + 7, LOW);
    */
    }
#endif
