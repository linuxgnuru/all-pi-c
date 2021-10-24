#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

// Global debug variable
//#define DEBUG_ACTIVE

#define CE 0

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

// Defines and Constants

//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp        = 0x00;
const char Digit0      = 0x01;
const char Digit1      = 0x02;
const char Digit2      = 0x03;
const char Digit3      = 0x04;
const char Digit4      = 0x05;
const char Digit5      = 0x06;
const char Digit6      = 0x07;
const char Digit7      = 0x08;
const char DecodeMode  = 0x09;
const char Intensity   = 0x0A;
const char ScanLimit   = 0x0B;
const char ShutDown    = 0x0C;
const char DisplayTest = 0x0F;

const int numOfDevices = 4;

// Global Variables

unsigned char TxBuffer[1024];
//char RxBuffer[1024];

int TxBufferIndex = 0;
//int RxBufferIndex = 0;

void transfer(unsigned char);
void endTransfer();

void begin(int channel, int speed)
{
    // Open port for reading and writing
    if (wiringPiSPISetup (channel, speed) < 0)
    {
        printf("Failed to open SPI port %d! Please try with sudo\n", channel);
    }
}

void transfer(unsigned char c)
{
    TxBuffer[TxBufferIndex] = c;
    TxBufferIndex++;
}

void endTransfer()
{
    //int temp = write(SpiFd, TxBuffer, TxBufferIndex); // Write the data from TxBuffer to the SPI bus...
    //int temp = wiringPiSPIDataRW(CE, TxBuffer, TxBufferIndex);
    wiringPiSPIDataRW(CE, TxBuffer, TxBufferIndex);
    // Debug level 2
#if DEBUG_ACTIVE == 2
    printf("Written: %d Index: %d Buffer: ", temp, TxBufferIndex);
    for(int i = 0; i < TxBufferIndex; i++)
    {
        printf("%d ", int(TxBuffer[i]));
    }
    printf("\n");
#endif
    TxBufferIndex = 0; // ...and reset the index
}

// Writes data to the selected device or does broadcast if device number is 255
void SetData(char adr, unsigned char data, unsigned char device)
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

// Setup
void setup()
{
    // The MAX7219 has officially no SPI / Microwire support like the MAX7221 but the
    // serial interface is more or less the same like a SPI connection
    begin(CE, 1000000);
    // Disable the decode mode because at the moment i dont use 7-Segment displays
    SetDecodeMode(0);
    // Set the number of digits; start to count at 0
    SetScanLimit(7);
    // Set the intensity between 0 and 15. Attention 0 is not off!
    SetIntensity(5);
    // Disable shutdown mode
    SetShutDown(0);
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

    SetData(Digit0, 0b00000001, 3);
    SetData(Digit1, 0b00000010, 3);
    SetData(Digit2, 0b00000100, 3);
    SetData(Digit3, 0b00001000, 3);
    SetData(Digit4, 0b00010000, 3);
    SetData(Digit5, 0b00100000, 3);
    SetData(Digit6, 0b01000000, 3);
    SetData(Digit7, 0b10000000, 3);

    SetData(Digit0, 0b10000000, 4);
    SetData(Digit1, 0b01000000, 4);
    SetData(Digit2, 0b00100000, 4);
    SetData(Digit3, 0b00010000, 4);
    SetData(Digit4, 0b00001000, 4);
    SetData(Digit5, 0b00000100, 4);
    SetData(Digit6, 0b00000010, 4);
    SetData(Digit7, 0b00000001, 4);
    delay(1000);

}

void loop()
{
    unsigned char tmp;
    //int j = 8;
    //you may know this from space invaders
    unsigned int rowBuffer[]=
    {
        0b0010000010000000,
        0b0001000100000000,
        0b0011111110000000,
        0b0110111011000000,
        0b1111111111100000,
        0b1011111110100000,
        0b1010000010100000,
        0b0001101100000000
    };
    //while (1)
    for (int i = 0; i < 2; i++)
    {
        for (int shiftCounter = 0; 15 >= shiftCounter; shiftCounter++)
        {
            for (int rowCounter = 0; 7 >= rowCounter; rowCounter++)
            {
                // roll the 16bits...
                // The information how to roll is from http://arduino.cc/forum/index.php?topic=124188.0 
                rowBuffer[rowCounter] = ((rowBuffer[rowCounter] & 0x8000) ? 0x01 : 0x00) | (rowBuffer[rowCounter] << 1);
                // ...and then write them to the two devices
                tmp = (char)rowBuffer[rowCounter];
                SetData(rowCounter+1, tmp, 1);

                tmp = (char)rowBuffer[rowCounter];
                printf("8 atmp: "BYTE_TO_BINARY_PATTERN"\t", BYTE_TO_BINARY(tmp));
                tmp = (char)rowBuffer[rowCounter]>>8;
                printf("8 btmp: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(tmp));
                SetData(rowCounter+1, tmp, 2);

                tmp = (char)rowBuffer[rowCounter];
                printf("16 atmp: "BYTE_TO_BINARY_PATTERN"\t", BYTE_TO_BINARY(tmp));
                tmp = (char)rowBuffer[rowCounter]>>16;
                printf("16 btmp: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(tmp));
                SetData(rowCounter+1, tmp, 3);

                tmp = (char)rowBuffer[rowCounter];
                printf("24 atmp: "BYTE_TO_BINARY_PATTERN"\t", BYTE_TO_BINARY(tmp));
                tmp = (char)rowBuffer[rowCounter]>>24;
                printf("24 btmp: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(tmp));
                SetData(rowCounter+1, tmp, 4);
                /*
                for (int i = 2; i < numOfDevices + 1; i++)
                {
                    SetData(rowCounter+1, (char)rowBuffer[rowCounter]>>y, i);
                    y += 8;
                }
                */
                /*
                for (int i = 1; i < numOfDevices + 1; i++)
                {
                    if (i == 1)
                        SetData(rowCounter+1, (char)rowBuffer[rowCounter], i);
                    else
                    {
                        j += 8;
                        SetData(rowCounter+1, (char)rowBuffer[rowCounter]>>j, i);
                    }
                }
                */
            }    
            delay(100);
        }
    }
}

static void die(int sig)
{
    SetShutDown(1);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

// Main
int main(int argc, char **argv)
{
    int initOnly = 0;

    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    //printf("%d\n", argc);
    for(int i = 0; i < argc; i++) 
    {
        if (strcmp (argv[i],"-init") == 0)
        {
            initOnly = 1;
            printf("Attention only initialisation will be done\n");
        }
        if (strcmp (argv[i],"-DEBUG") == 0)
        {
#define DEBUG_ACTIVE 2
            printf("!!! DEBUG ACTIVE !!!\n");
        }
    }
    setup();
    if (initOnly != 0)
    {
        return 0;
    }
    loop();
    return 0;
}

