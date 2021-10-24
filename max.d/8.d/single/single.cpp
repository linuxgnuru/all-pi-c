#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "RasPiSPI.h"

using namespace std;

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

//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp        = 0x00;
const char Digits[8] = {
 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};
const char DecodeMode  = 0x09;
const char Intensity   = 0x0A;
const char ScanLimit   = 0x0B;
const char ShutDown    = 0x0C;
const char DisplayTest = 0x0F;

const char numOfDevices = 4;

unsigned char ledData[4][8] = {
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }
};

void setup();

void bitWrite(int addr, int col, int row, int b)
{
    if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 8) return;
    ledData[addr][col] ^= (-b ^ ledData[addr][col]) & (1 << row);
}

void bitClear(int addr, int col, int row)
{
    if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 8) return;
    ledData[addr][col] ^= (0 ^ ledData[addr][col]) & (1 << row);
}

void bitSet(int addr, int col, int row)
{
    if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 8) return;
    ledData[addr][col] ^= (-1 ^ ledData[addr][col]) & (1 << row);
}

RasPiSPI SPI;

// Writes data to the selected device or does broadcast if device number is 255
void SetData(char adr, char data, char device)
{
    // Count from top to bottom because first data which is sent is for the last device in the chain
    for (int i = numOfDevices; i > 0; i--)
    {
        if ((i == device) || (device == 255))
        {
            SPI.transfer(adr);
            SPI.transfer(data);
        }
        else // if its not the selected device send the noop command
        {
            SPI.transfer(NoOp);
            SPI.transfer(0);
        }
    }
    SPI.endTransfer();
    delay(1);
}

// Writes the same data to all devices
void SetData(char adr, char data) { SetData(adr, data, 255); } // write to all devices (255 = Broadcast) 

void SetShutDown(char Mode) { SetData(ShutDown, !Mode); }
void SetScanLimit(char Digits) { SetData(ScanLimit, Digits); }
void SetIntensity(char intense) { SetData(Intensity, intense); }
void SetDecodeMode(char Mode) { SetData(DecodeMode, Mode); }

void clearAll()
{
    for (int i = 0; i < 8; i++)
    {
        SetData(Digits[i], 0b00000000);
    }
}

void doGraph(int addr, int c, int r)
{
    int myaddr = addr - 1;
    char tmp[9];

    if (addr == 3)
    {
        c = abs(c - 7);
        r = abs(r - 7);
    }
    cout << "addr: " << addr << " c: " << c << " r: " << r;
    for (int i = 0; i < 8; i++)
    {
        if (r == 0)
            ledData[myaddr][c] = 0x0;
        else
            bitWrite(myaddr, c, i, (i < r));
    }
    cout << " ledData[myaddr][c]: ";
    sprintf(tmp, "%c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(ledData[myaddr][c]>>8), BYTE_TO_BINARY(ledData[myaddr][c]));
    cout << tmp << endl;
    SetData(Digits[c], ledData[myaddr][c], addr);
        /*
    SetData(Digit1, 0b01000000, addr);
    SetData(Digit2, 0b00100000, addr);
    SetData(Digit3, 0b00010000, addr);
    SetData(Digit4, 0b00001000, addr);
    SetData(Digit5, 0b00000100, addr);
    SetData(Digit6, 0b00000010, addr);
    SetData(Digit7, 0b00000001, addr);
    */
}

static void die(int sig)
{
    clearAll();
    SetShutDown(1);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    int adr, col, row;
    char mydata = 0x0;
    char tmp[9];

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (argc == 4)
    {
        adr = atoi(argv[1]) - 1;
        col = atoi(argv[2]);
        row = atoi(argv[3]);
        if (adr == 2)
        {
            col = abs(col - 7);
            row = abs(row - 7);
        }
    }
    else
    {
        printf("usage: %s [panel] [row] [column]\n", argv[0]);
        return EXIT_FAILURE;
    }
    setup();
    clearAll();
    //cout << "before bitSet ledData[adr][col]: [" << adr << "] [" << col << "] "; sprintf(tmp, "%c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(ledData[adr][col]>>8), BYTE_TO_BINARY(ledData[adr][col])); cout << tmp << endl;
    bitSet(adr, col, row);
    //cout << " after bitSet ledData[adr][col]: [" << adr << "] [" << col << "] "; sprintf(tmp, "%c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(ledData[adr][col]>>8), BYTE_TO_BINARY(ledData[adr][col])); cout << tmp << endl;
    mydata = ledData[adr][col];
    cout << "mydata: "; sprintf(tmp, "%c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(mydata>>8), BYTE_TO_BINARY(mydata)); cout << tmp << endl;
    SetData(Digits[col], mydata, adr + 1);
    return 0;
}

void setup()
{
    // The MAX7219 has officially no SPI / Microwire support like the MAX7221 but the
    // serial interface is more or less the same like a SPI connection
    SPI.begin();
    // Disable the decode mode because at the moment i dont use 7-Segment displays
    SetDecodeMode(false);
    // Set the number of digits; start to count at 0
    SetScanLimit(7);
    // Set the intensity between 0 and 15. Attention 0 is not off!
    SetIntensity(5);
    // Disable shutdown mode
    SetShutDown(false);
    clearAll();
#if 0
    // Write some patterns
    SetData(Digit0, 0b10000000, 1);
    SetData(Digit1, 0b01000000, 1);
    SetData(Digit2, 0b00100000, 1);
    SetData(Digit3, 0b00010000, 1);
    SetData(Digit4, 0b00001000, 1);
    SetData(Digit5, 0b00000100, 1);
    SetData(Digit6, 0b00000010, 1);
    SetData(Digit7, 0b00000001, 1);

    SetData(Digit0, 0b00000001, 2);
    SetData(Digit1, 0b00000010, 2);
    SetData(Digit2, 0b00000100, 2);
    SetData(Digit3, 0b00001000, 2);
    SetData(Digit4, 0b00010000, 2);
    SetData(Digit5, 0b00100000, 2);
    SetData(Digit6, 0b01000000, 2);
    SetData(Digit7, 0b10000000, 2);

    SetData(Digit0, 0b10000000, 3);
    SetData(Digit1, 0b01000000, 3);
    SetData(Digit2, 0b00100000, 3);
    SetData(Digit3, 0b00010000, 3);
    SetData(Digit4, 0b00001000, 3);
    SetData(Digit5, 0b00000100, 3);
    SetData(Digit6, 0b00000010, 3);
    SetData(Digit7, 0b00000001, 3);

    SetData(Digit0, 0b00000001, 4);
    SetData(Digit1, 0b00000010, 4);
    SetData(Digit2, 0b00000100, 4);
    SetData(Digit3, 0b00001000, 4);
    SetData(Digit4, 0b00010000, 4);
    SetData(Digit5, 0b00100000, 4);
    SetData(Digit6, 0b01000000, 4);
    SetData(Digit7, 0b10000000, 4);
    delay(1000);
#endif
}
