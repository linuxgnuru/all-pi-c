#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

//#define USE_N

#ifdef USE_N
#include <ncurses.h>
#endif

#include <wiringPi.h>
#include <wiringPiSPI.h>
//#include <sr595.h>

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
*/

unsigned char data[2] = { 0x0, 0x0 };
unsigned char backup_data[2] = { 0x0, 0x0 };

//void bitWrite(uint8_t &x, unsigned int n, _Bool b)
void bitWrite(int ab, int n, int b)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (-b ^ data[ab]) & (1 << n);
    /*
    if (n <= 7 && n >= 0)
    {
        if (b) data[0] |= (1u << n);
        else data[0] &= ~(1u << n);
    }
    */
}

void bitClear(int ab, int n)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (0 ^ data[ab]) & (1 << n);
    // if (n <= 7 && n >= 0) data[0] &= ~(1u << n);
}

void bitSet(int ab, int n)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (-1 ^ data[ab]) & (1 << n);
    //if (n <= 7 && n >= 0) data[0] |= (1u << n);
}

static void die(int sig)
{
    //onOff(0);
    data[0] = data[1] = 0b00000000;
    wiringPiSPIDataRW(CE, data, 2);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
#ifdef USE_N
    endwin();
#endif
    exit(0);
}

int main(int argc, char **argv)
{
    //int loc = 0;
    //static uint8_t data[2] = { 0x0, 0x0 };
//    unsigned char data[1] = { 0x0 };
    /*
    _Bool reading;
    _Bool quitFlag = FALSE;
    _Bool flipflopflag;
    _Bool lastStatus;
    _Bool currentStatus;
    long lastDebounceTime = 0;
    int buttonState = HIGH;
    int lastButtonState = HIGH;
    */
    //int number = 0;
    int i, j;
#ifdef USE_N
    int key;
#endif
    int x = 25;

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
    //sr595Setup(ADDR, BITS, dataPin, clockPin, latchPin);
    wiringPiSPISetup(CE, 500000);
#ifdef USE_N
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("hello world");
    refresh();
#endif
    /*
    //bitWrite(loc, 1);
    bitSet(loc);
    wiringPiSPIDataRW(CE, data, 1);
    delay(500);
    bitClear(loc);
    wiringPiSPIDataRW(CE, data, 1);
     */

    /*
       printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
       bitSet(3);
       wiringPiSPIDataRW(CE, data, 1);
       bitSet(5);
       printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
       wiringPiSPIDataRW(CE, data, 1);
       bitClear(3);
       printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
       wiringPiSPIDataRW(CE, data, 1);
       printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
     */
    while (1)
    {
    for (j = 0; j < 8; j++)
    {
        //data[0] = 0b00000001;
        //data[0] = 0x01 << i;
        //wiringPiSPIDataRW(CE, data, sizeof(data));
        //bitWrite(data[0], i, 1);
        //bitWrite(i, 1);
        bitSet(0, j);
        bitSet(1, j);
        //printf("data[0]: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(data[0]>>8), BYTE_TO_BINARY(data[0]));
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
    //for (i = 0; i < 10; i++) ledState[i] = 0;
    /*
       onOff(0);
       while (quitFlag == FALSE)
       {
       reading = digitalRead(encoderButton);
       if (reading != lastButtonState)
       lastDebounceTime = millis();
       if ((millis() - lastDebounceTime) > debounceDelay)
       {
       if (reading != buttonState)
       {
       buttonState = reading;
       if (buttonState == LOW)
       quitFlag = TRUE;
       }
       }
       lastButtonState = reading;

       lastStatus = digitalRead(encoderPinB);
       while (!digitalRead(encoderPinA))
       {
       currentStatus = digitalRead(encoderPinB);
       flipflopflag = TRUE;
       }
       if (flipflopflag == TRUE)
       {
       flipflopflag = FALSE;
       if (!lastStatus && currentStatus) number++;
       if (lastStatus && !currentStatus) number--;
    //if ((lastStatus == 0) && (currentStatus == 1)) number++;
    //if ((lastStatus == 1) && (currentStatus == 0)) number--;
    if (number < 0) number = 0;
    if (number > 10) number = 10;
    doGraph(number);
#ifdef USE_N
move(1, 0);
printw("number: %2d", number);
refresh();
#endif
}
}
onOff(0);
     */
#ifdef USE_N
endwin();
#endif
return EXIT_SUCCESS;
}

