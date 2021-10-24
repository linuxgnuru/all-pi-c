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
    if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 7) return;
    ledData[addr][col] ^= (-b ^ ledData[addr][col]) & (1 << row);
}

void bitClear(int addr, int col, int row)
{
    if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 7) return;
    ledData[addr][col] ^= (0 ^ ledData[addr][col]) & (1 << row);
}

void bitSet(int addr, int col, int row)
{
    if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 7) return;
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

void clearAll() { for (int i = 0; i < 8; i++) SetData(Digits[i], 0b00000000); }

void Draw(int addr, int col, int row)
{
    char mydata = 0x0;

    bitSet(addr, col, row);
    mydata = ledData[addr][col];
    SetData(Digits[col], mydata, addr + 1);
}

/*
void doGraph(int addr, int height, int row)
{
    if (height == 0)
    {
        for (int i = 0; i < 8; i++)
        {
            Draw(addr, i, row, 0);
        }
    }
    else if (height == 8)
    {
        for (int i = 0; i < 8; i++)
        {
            Draw(addr, i, row, 1);
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            Draw(addr, i, row, (i < height));
        }
    }
#if 0
    char tmp[9];

    cout << " addr: " << addr << "height: " << height << " row: " << row << endl;
    if (height == 0)
    {
        ledData[addr][row] = 0x0;
        for (int i = 7; i > -1; i--)
        {
            SetData(Digits[i], ledData[addr][row], addr + 1);
        }
    }
    else if (height == 8)
    {
        ledData[addr][row] = 0xf;
        for (int i = 7; i > -1; i--)
        {
            SetData(Digits[i], ledData[addr][row], addr + 1);
        }
    }
    else
    {
        for (int i = 7; i > -1; i--)
        {
            bitWrite(addr, row, i, (i < height));
            cout << " ledData[addr][row]: ";
            sprintf(tmp, "%d %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c", i, BYTE_TO_BINARY(ledData[addr][row]>>8), BYTE_TO_BINARY(ledData[addr][row]));
            cout << tmp << endl;
            SetData(Digits[i], ledData[addr][row], addr + 1);
        }
    }
#endif
}
*/

static void die(int sig);

int main(int argc, char **argv)
{
    int addr, col, row;
    //char mydata = 0x0;
    //char tmp[9];

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    setup();
    clearAll();
    if (argc >= 4)
    {
        cout << "command line\n";
        for (int i = 1; i < argc; i += 3)
        {
            addr = atoi(argv[i]) - 1;
            col = atoi(argv[i+1]);
            row = atoi(argv[i+2]);
            Draw(addr, col, row);
        }
    }
    else
    {
        cout << "usage: " << argv[0] << " [panel] [row] [column]\n";
        cout << "or [panel2] [row2] [column2]\n";
        return EXIT_FAILURE;
    }
    //bitSet(adr, col, row);
    //mydata = ledData[adr][col];
    //SetData(Digits[col], mydata, adr + 1);
    return 0;
}

void setup()
{
    // The MAX7219 has officially no SPI / Microwire support like the MAX7221 but the
    // serial interface is more or less the same like a SPI connection
    SPI.begin();
    SetDecodeMode(false); // Disable the decode mode because at the moment i dont use 7-Segment displays
    SetScanLimit(7); // Set the number of digits; start to count at 0
    SetIntensity(5); // Set the intensity between 0 and 15. Attention 0 is not off!
    SetShutDown(false); // Disable shutdown mode
    clearAll();
}

static void die(int sig)
{
    clearAll();
    SetShutDown(1);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}
