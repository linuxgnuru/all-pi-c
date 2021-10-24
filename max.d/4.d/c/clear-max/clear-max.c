#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CE 0

//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp        = 0x00;
const char Digits[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
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

unsigned char TxBuffer[1024];
int TxBufferIndex = 0;
void transfer(char);
void endTransfer();

void begin(int channel, int speed)
{
    if (wiringPiSPISetup(channel, speed) < 0) printf("Failed to open SPI port %d! Please try with sudo\n", channel);
}

void transfer(char c)
{
    TxBuffer[TxBufferIndex] = c;
    TxBufferIndex++;
}

void endTransfer()
{
    wiringPiSPIDataRW(CE, TxBuffer, TxBufferIndex);
    TxBufferIndex = 0;
}

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

_Bool bitRead(int addr, int col, int row) { return (ledData[addr][col] >> row) & 0x01; }

// Writes data to the selected device or does broadcast if device number is 255
void SetData(char adr, char data, char device)
{
    // Count from top to bottom because first data which is sent is for the last device in the chain
    for (int i = numOfDevices; i > 0; i--)
    {
        if ((i == device) || (device == 255))
        {
            transfer(adr);
            transfer(data);
        }
        else // if its not the selected device send the noop command
        {
            transfer(NoOp);
            transfer(0);
        }
    }
    endTransfer();
    delay(1);
}

// Writes the same data to all devices
void SetData_all(char adr, char data) { SetData(adr, data, 255); } // write to all devices (255 = Broadcast) 

void SetShutDown(char Mode) { SetData_all(ShutDown, !Mode); }
void SetScanLimit(char Digits) { SetData_all(ScanLimit, Digits); }
void SetIntensity(char intense) { SetData_all(Intensity, intense); }
void SetDecodeMode(char Mode) { SetData_all(DecodeMode, Mode); }

void clearAll() { for (int i = 0; i < 8; i++) SetData_all(Digits[i], 0b00000000); }

int main(int argc, char **argv)
{
    setup();
    return EXIT_SUCCESS;
}

void setup()
{
    // The MAX7219 has officially no SPI / Microwire support like the MAX7221 but the
    // serial interface is more or less the same like a SPI connection
    // default use channel 0 and 1MHz clock speed
    begin(CE, 1000000);
    SetDecodeMode(FALSE); // Disable the decode mode because at the moment i dont use 7-Segment displays
    SetScanLimit(7); // Set the number of digits; start to count at 0
    SetIntensity(5); // Set the intensity between 0 and 15. Attention 0 is not off!
    SetShutDown(FALSE); // Disable shutdown mode
    clearAll();
}

